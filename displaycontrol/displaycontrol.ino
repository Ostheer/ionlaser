/* Timing constants */
#define CLKPWMRATIO 100 // Amount of clock cycles per display update cycle
#define UPDATECOUNT 1000 // Amount of clock cycles per frame. One frame takes CLKPWMRATIO*UPDATECOUNT clock cycles

/* Pin definitions */
#define PWMPIN D1
#define CLKPIN D2
#define RSTPIN D5

/* Timekeeping variables */
bool clk = false;
int k = 0;
int s = 0;
int a = 0;

/* Framebuffer */
bool buffer[CLKPWMRATIO] = {};

void setup() {
  pinMode(PWMPIN,OUTPUT);
  pinMode(CLKPIN,OUTPUT);
  pinMode(RSTPIN,INPUT);
  digitalWrite(RSTPIN,LOW);
  flush();

  //set segment permanently on
  //for (int i = 20; i<40; i++) buffer[i]=true;
}
  
void loop() {
  /* Write buffer */
  digitalWrite(PWMPIN,!buffer[k]);
  
  /* Do animation */
  //buffer[a] = !buffer[a]; //fill and clear
  //buffer[a]=true; if (a>0) buffer[a-1]=false; else buffer[CLKPWMRATIO]=false; //walking line
  if (a>CLKPWMRATIO) a=0;
  
  /* Take care of Animation update rate */
  s++;
  if (s>UPDATECOUNT) {a++;s=0;}
  
  /* Generate clock signal */
  if (clk) {
    k++;
    if (k == CLKPWMRATIO) k = 0;
  }
  clk = !clk;
  digitalWrite(CLKPIN,clk);
}

/* Flush the counter IC's until they reset */
void flush() {
  for (int i = 0; i < CLKPWMRATIO; i++){
    digitalWrite(CLKPIN,LOW);
    delayMicroseconds(10);
    if (digitalRead(RSTPIN)) return;
    delayMicroseconds(10);
    digitalWrite(CLKPIN,HIGH); 
    delayMicroseconds(20);
  }
}
