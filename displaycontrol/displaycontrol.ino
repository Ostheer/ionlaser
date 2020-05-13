/* Timing constants */
#define DL 100 // Amount of clock cycles per display update cycle = Display Length / resolution
#define UPDATECOUNT 1000 // Amount of clock cycles per frame. One frame takes DL*UPDATECOUNT clock cycles

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
bool buffer[DL] = {};

void setup() {
  /* Initialise pins */
  pinMode(PWMPIN,OUTPUT);
  pinMode(CLKPIN,OUTPUT);
  pinMode(RSTPIN,INPUT);
  digitalWrite(RSTPIN,LOW);

  delay(5000);
  /* Flush the counter IC's until they reset */
  for (int i = 0; i < DL; i++){
    digitalWrite(CLKPIN,LOW);
    delayMicroseconds(10);
    if (digitalRead(RSTPIN)) break;
    delayMicroseconds(10);
    digitalWrite(CLKPIN,HIGH); 
    delayMicroseconds(20);
  }
}
  
void loop() {
  /* Write buffer */
  digitalWrite(PWMPIN,!buffer[k]);
  
  /* Animations */
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
  if (s>UPDATECOUNT) {a++;s=0;}
  
  /* Generate clock signal */
  if (clk) {
    k++;
    if (k == DL) k = 0;
  }
  clk = !clk;
  digitalWrite(CLKPIN,clk);
}
