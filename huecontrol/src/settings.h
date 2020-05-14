#ifndef SETINGS_H
#define SETINGS_H

#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "pass"

//Static IP address configuration
#define IPP0 10
#define IPP1 0
#define IPP2 0
#define IPP3 80

#define DNS0 208
#define DNS1 67
#define DNS2 222
#define DNS3 222

//Hue
#define HUE_IP "10.0.0.99"
#define HUE_PORT 80
#define HUE_USER "hue username"

//update rate
// Amount of clock cycles per frame. One frame takes DL*UPDATECOUNT clock cycles
#define UPDATECOUNT 1000
#define HUE_SEND_INTERVAL 6 //extra counter for less frequent hue updates
#define IDLE_TIMEOUT 1000000 //when to start animation. 100000 cycles ~ 1 second

//knob resolution
#define KNOB_THRESHOLD 50

#endif 
