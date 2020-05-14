#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "settings.h"

#ifndef HUEFUNCS_H
#define HUEFUNCS_H

void sendLightCommand(int light, String command);
DynamicJsonDocument getLightStatus(int light);
int getNumLights();
int getMaxLightIndex(int numLights);
void alertLight(int light);

extern WiFiClient client;

#endif
