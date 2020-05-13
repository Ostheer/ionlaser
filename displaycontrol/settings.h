#ifndef __SETINGS_H
#define __SETINGS_H

#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "pass"

//Static IP address configuration
IPAddress staticIP(10, 0, 0, 80);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(208,67,222,222);

#endif 
