#include <TLC591x.h>

/* Pin definitions */
//display
#define DPWMPIN D1
#define DCLKPIN D2
#define DRSTPIN D5
//shift register
#define SLEPIN D6
#define SCLKPIN D7
#define SSDIPIN D8
//switches
#define SW1 D3
#define SW2 D4

/* Display stuff */
//timekeeping variables
bool clk = false;
int k = 0;
int s = 0;
int a = 0;
//timing constants
#define DL 100 // Amount of clock cycles per display update cycle = Display Length / resolution
#define UPDATECOUNT 1000 // Amount of clock cycles per frame. One frame takes DL*UPDATECOUNT clock cycles
//framebuffer 
bool buffer[DL] = {};

/* Shift register stuff*/
TLC591x shitreg(1, SSDIPIN, SCLKPIN, SLEPIN);
const int zero = 0;
char leds = 0b00000000;
//led masks
#define LIGHTSCALE 0b00000001
#define CURRNTSCLE 0b00000010
#define RUNMODE    0b00000100
#define STNDBYMODE 0b00001000
#define INTERLOCK  0b00010000
#define SUPPLYTEMP 0b00100000
#define HEADTEMP   0b01000000
#define HEADCOOLNG 0b10000000
inline char inv(char mask){ return 0b11111111^mask; }

void setup() {
  /* Initialise pins */
  pinMode(DPWMPIN,OUTPUT);
  pinMode(DCLKPIN,OUTPUT);
  pinMode(DRSTPIN,INPUT);
  pinMode(SLEPIN,OUTPUT);
  pinMode(SCLKPIN,OUTPUT);
  pinMode(SSDIPIN,OUTPUT);
  pinMode(SW1,INPUT);
  pinMode(SW2,INPUT);
  digitalWrite(DRSTPIN,LOW);
  digitalWrite(SW1,LOW);
  digitalWrite(SW2,LOW);

  delay(5000);
  /* Flush the counter IC's until they reset */
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

  /* Write buffer */
  digitalWrite(DPWMPIN,!buffer[k]);
  
  /* Display Animations */
  //fill and clear
  //if (a>DL) a=0;
  //buffer[a] = !buffer[a]; 
  //walking line
  //if (a>DL) a=0;
  //buffer[a]=true; if (a>0) buffer[a-1]=false; else buffer[DL]=false; 
  //centered pulse
  if (a>2*DL/3) a=0;
  else if (a>DL/3){ buffer[DL/2-DL/3+(a-DL/3)-1] = false; buffer[DL/2+DL/3-(a-DL/3)+1] = false; }
  else{ buffer[DL/2-a]=true; buffer[DL/2+a]=true; }
  //set segment permanently on
  //for (int i = 20; i<40; i++) buffer[i]=true;

  /* Take care of Animation update rate */
  s++;
  if (s>UPDATECOUNT) {
    //update counters
    a++;
    s=0;
    
    //update leds
    int val = analogRead(A0) >> 2;
    leds = 0;
    for (int i = 0; i < 5; i++ ){
      if (val > i*59) leds = (0b11111111>>(4-i));
    leds &= 0b11110000;
    
    if (digitalRead(SW1)) {leds |= CURRNTSCLE; leds &= inv(LIGHTSCALE);}
    else {leds |= LIGHTSCALE; leds &= inv(CURRNTSCLE);}

    if (digitalRead(SW2)) {leds |= STNDBYMODE; leds &= inv(RUNMODE);}
    else {leds |= RUNMODE; leds &= inv(STNDBYMODE);}
    
    shitreg.print(leds);
  }
  
    }
  
  /* Generate clock signal */
  if (clk) {
    k++;
    if (k == DL) k = 0;
  }
  clk = !clk;
  digitalWrite(DCLKPIN,clk);
}
