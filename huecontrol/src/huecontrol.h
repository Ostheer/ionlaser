#ifndef HUECONTROL_H
#define HUECONTROL_H

void flashleds(int d);
void updateSwitches();
void ledNumberDisplay(unsigned char val);
unsigned char reversebits(unsigned char b);
void updateLight(int light, int knobPosition);
void displayValue(int knobPosition);
bool knobChanged(int knobPosition);
bool switchChanged(int sw, bool currentState);

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
#define SW0 D0
#define SW1 D4

//led masks
#define LIGHTSCALE 0b00000001
#define CURRNTSCLE 0b00000010
#define RUNMODE    0b00000100
#define STNDBYMODE 0b00001000
#define INTERLOCK  0b00010000
#define SUPPLYTEMP 0b00100000
#define HEADTEMP   0b01000000
#define HEADCOOLNG 0b10000000

// Display resolution
#define DL 100

#endif
