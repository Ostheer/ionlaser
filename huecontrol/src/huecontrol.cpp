#include <Arduino.h>
#include <TLC591x.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "settings.h"
#include "huecontrol.h"
#include "huefuncs.h"

/* Display stuff */
//timekeeping variables
bool clk = false;
int displayIndex = 0;
int updateCounter = 0;
int animationFrame = 0;
int knobPollCounter = 0;
//framebuffer 
bool buffer[DL] = {};

/* Button status */
bool switches[2] = {};
int knob;

/* Shift register stuff*/
TLC591x shiftreg(1, SSDIPIN, SCLKPIN, SLEPIN);
const int zero = 0;
unsigned char leds = 0b00000000;
inline char inv(char mask){ return 0b11111111^mask; }

/* WiFi connection */
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
WiFiClient client;
IPAddress staticIP(IPP0, IPP1, IPP2, IPP3);
IPAddress gateway(IPP0, IPP1, IPP2, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(DNS0, DNS1, DNS2, DNS3);

/* Philips Hue */
int numLights;
int currentLight;
bool lampUpdated = false;

void setup() {
  /* Initialise pins */
  pinMode(DPWMPIN,OUTPUT);
  pinMode(DCLKPIN,OUTPUT);
  pinMode(DRSTPIN,INPUT);
  pinMode(SLEPIN,OUTPUT);
  pinMode(SCLKPIN,OUTPUT);
  pinMode(SSDIPIN,OUTPUT);
  pinMode(SW0,INPUT);
  pinMode(SW1,INPUT);
  digitalWrite(DRSTPIN,LOW);
  digitalWrite(SW0,LOW);
  digitalWrite(SW1,LOW);

  /* Make WiFi connection */
  WiFi.disconnect();
  WiFi.hostname("IONLASER");
  WiFi.config(staticIP, gateway, subnet, dns);
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  while (WiFi.status() != WL_CONNECTED) flashleds(100);
  
  /* Read button states */
  updateSwitches();
  knob = analogRead(A0);

  /* Get number of Hue lights */
  numLights = getNumLights();
  currentLight = 1;

  /* Initialise display */
  delay(2500);
  // Flush the counter IC's until they reset
  for (int i = 0; i < DL; i++){
    digitalWrite(DCLKPIN,LOW);
    delayMicroseconds(10);
    if (digitalRead(DRSTPIN)) break;
    delayMicroseconds(10);
    digitalWrite(DCLKPIN,HIGH); 
    delayMicroseconds(20);
  }
}
  
void loop() {
  /* Write display buffer element */
  digitalWrite(DPWMPIN,!buffer[DL-1-displayIndex]);
  
  /* Display Animations */
  //if (animationFrame>2*DL/3) animationFrame=0;
  //else if (animationFrame>DL/3){ buffer[DL/2-DL/3+(animationFrame-DL/3)-1] = false; buffer[DL/2+DL/3-(animationFrame-DL/3)+1] = false; }
  //else{ buffer[DL/2-animationFrame]=true; buffer[DL/2+animationFrame]=true; }


  /* Take care of Animation/Action updates */
  updateCounter++;
  if (updateCounter>UPDATECOUNT) {
    updateCounter=0;//reset action counter
    animationFrame++;//update display frame counter
    
    //Update switch leds
    if (digitalRead(SW0)) {leds |= CURRNTSCLE; leds &= inv(LIGHTSCALE);}
    else {leds |= LIGHTSCALE; leds &= inv(CURRNTSCLE);}
    if (digitalRead(SW1)) {leds |= STNDBYMODE; leds &= inv(RUNMODE);}
    else {leds |= RUNMODE; leds &= inv(STNDBYMODE);}

    /* Check if the switch has been used to go to the next light */
    if (switches[1] != digitalRead(SW1)){
      currentLight++;
      if (currentLight > numLights) currentLight = 1;
      DynamicJsonDocument doc = getLightStatus(currentLight);
      //TODO: this is a potential deadlock
      while (doc.isNull()){
        currentLight++;
        if (currentLight > numLights) currentLight = 1;
        doc = getLightStatus(currentLight);
      }

      //perform actions for newly selected light
      lampUpdated = false;
      ledNumberDisplay(currentLight);
      alertLight(currentLight);
    }

    /* Read knob position and update lights */
    knobPollCounter++;
    if (knobPollCounter > HUE_SEND_INTERVAL){
      knobPollCounter = 0;
      int knobPosition = analogRead(A0);
      displayValue(knobPosition);
      updateLight(currentLight, knobPosition);
    }

    shiftreg.print(leds); //write to shift register
    updateSwitches(); //update switch input
  }
  
  /* Generate clock signal for display */
  if (clk) {
    displayIndex++;
    if (displayIndex == DL) displayIndex = 0;
  }
  clk = !clk;
  digitalWrite(DCLKPIN,clk);
}

void flashleds(int d) {
  shiftreg.print(0b11111111);
  delay(d);
  shiftreg.print(zero);
  delay(d);
  shiftreg.print(0b11111111);
  delay(d);
  shiftreg.print(leds);
}

void updateSwitches(){
  switches[0] = digitalRead(SW0);
  switches[1] = digitalRead(SW1);
}

void ledNumberDisplay(unsigned char val){
  //display number (mod 16) on the right 4 leds. Bit 5 (from the right) is MSB (INTERLOCK)
  val &= 0b00001111;
  val = reversebits(val);
  leds &= 0b00001111;
  leds |= val;
  shiftreg.print(val);
}

unsigned char reversebits(unsigned char b){
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

void updateLight(int light, int knobPosition){
  //knobPosition between 0-1023
  if (abs(knob-knobPosition) > KNOB_THRESHOLD){
    if (!switches[0]){
      if (knobPosition < KNOB_THRESHOLD)
        sendLightCommand(light, "{\"on\": false}");
      else{
        unsigned int brval = knobPosition;
        sendLightCommand(light, "{\"on\": true}");
        brval = brval >> 2;
        if (brval > 0xFE) brval=0xFE;
        sendLightCommand(light, "{\"bri\": " + (String)brval + "}");
      }
    }
    else{
      unsigned int hueval = knobPosition;
      hueval = hueval << 6;
      sendLightCommand(light, "{\"hue\": " + (String)hueval + "}");
    }
    knob = knobPosition;
    lampUpdated = true;
  }
}

void displayValue(int knobPosition){
  //knobPosition between 0-1023
  for (int i = 0; i<DL; i++)
    buffer[i]=false;

  knobPosition /= 10;
  if (knobPosition>DL-1) knobPosition=DL-1;
  for (int i = 0; i<knobPosition; i++)
    buffer[i]=true;
}