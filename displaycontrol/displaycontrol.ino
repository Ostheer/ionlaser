#define PERIOD 1
#define CLKRATIO 2 //amount of loop cycles per clock cycle
#define CLKPWMRATIO 100 //amount of clock cycles per pwm cycle

#define PWMPIN D1
#define CLKPIN D2
#define RSTPIN D5

int pwmduty = 0;
int clkduty = 0;
int j;
int k;
int sweep = 0;

void setup() {
  pinMode(PWMPIN,OUTPUT);
  pinMode(CLKPIN,OUTPUT);
  pinMode(RSTPIN,INPUT);
  digitalWrite(RSTPIN,LOW);
  
  //Serial.begin(9600);
  
  pwmduty = 0;
  clkduty = 1;
  
  j = 0;
  k = 0;
  flush();
}
  
void loop() {
  j++;
  sweep++;

  if (sweep == PERIOD*2500){
    pwmduty = pwmduty + 1;
    if (pwmduty > 100) pwmduty = 0;
    sweep = 0;
  }
  
  if (j == clkduty){
    digitalWrite(CLKPIN,LOW);
    
    if (k == pwmduty){
      digitalWrite(PWMPIN,LOW);
    }
    if (k == CLKPWMRATIO){
      k = 0;
      digitalWrite(PWMPIN,HIGH);
    }
  }
  if (j == CLKRATIO){
    j = 0;
    k++;
    digitalWrite(CLKPIN,HIGH);
  }
  
  //delayMicroseconds(PERIOD);
}

//flush the counter IC's until they reset
int flush() {
  digitalWrite(PWMPIN,LOW);
  

  for (int i = 0; i < CLKPWMRATIO; i++){
    digitalWrite(CLKPIN,HIGH);
    for (int j = 0; j < CLKRATIO; j++){
      if (j == clkduty) digitalWrite(CLKPIN,LOW);
      delayMicroseconds(10);
      if (digitalRead(RSTPIN)) return i;
      delayMicroseconds(10);
    }
  }

  return 0;
}
