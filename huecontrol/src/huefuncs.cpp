#include <Arduino.h>
#include <ArduinoJson.h>
#include "huefuncs.h"

/* Philips Hue */
const char* hueHubIP = HUE_IP;
const char* hueUsername = HUE_USER;
const int hueHubPort = HUE_PORT;

void sendLightCommand(int light, String command){
  if (client.connect(hueHubIP, hueHubPort))
  {
    client.print("PUT /api/");
    client.print(hueUsername);
    client.print("/lights/");
    client.print(light);
    client.println("/state HTTP/1.1");
    client.println("keep-alive");
    client.print("Host: ");
    client.println(hueHubIP);
    client.print("Content-Length: ");
    client.println(command.length());
    client.println("Content-Type: text/plain;charset=UTF-8");
    client.println();
    client.println(command);
    client.stop();
  }
}

DynamicJsonDocument getLightStatus(int light){
  DynamicJsonDocument retdoc(JSON_OBJECT_SIZE(2));
  
  // Send request to Hue
  if (!client.connect(hueHubIP, hueHubPort)) return retdoc;
  client.print("GET /api/");
  client.print(hueUsername);
  client.print("/lights/");
  client.print(light);
  client.println(" HTTP/1.1");
  client.print("Host: ");
  client.println(hueHubIP);
  client.println("Content-type: application/json");
  client.println("keep-alive");
  if (client.println() == 0) {
    //Serial.println(F("Failed to send request"));
    return retdoc;
  }

  // Check HTTP return status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0) {
    //Serial.print(F("Unexpected response: "));
    //Serial.println(status);
    return retdoc;
  }
  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    //Serial.println(F("Invalid response"));
    return retdoc;
  }
  //Allocate JSON object
  const size_t capacity = 4*JSON_ARRAY_SIZE(2) + JSON_ARRAY_SIZE(3) + 4*JSON_OBJECT_SIZE(2) + 
                          JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + 
                          2*JSON_OBJECT_SIZE(11) + 1400;
  DynamicJsonDocument doc(capacity);
  // Parse result
  DeserializationError error = deserializeJson(doc, client);
  if (error) return retdoc;

  // Extract values
  const char* type = doc["type"];
  retdoc["type"] = type;
  retdoc["reachable"] = doc["state"]["reachable"];
  
  client.stop();

  return retdoc;
}

int getNumLights(){
  // Send request to Hue
  if (!client.connect(hueHubIP, hueHubPort)) return 0;
  client.print("GET /api/");
  client.print(hueUsername);
  client.print("/lights");
  client.println(" HTTP/1.1");
  client.print("Host: ");
  client.println(hueHubIP);
  client.println("Content-type: application/json");
  client.println("keep-alive");
  if (client.println() == 0) {
    //Serial.println(F("Failed to send request"));
    return 0;
  }

  // Check HTTP return status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0) {
    //Serial.print(F("Unexpected response: "));
    //Serial.println(status);
    return 0;
  }
  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    //Serial.println(F("Invalid response"));
    return 0;
  }
  //Allocate JSON object
  const size_t capacity = 24*JSON_ARRAY_SIZE(2) + 6*JSON_ARRAY_SIZE(3) + 3*JSON_OBJECT_SIZE(1) + 33*JSON_OBJECT_SIZE(2) + 
                          12*JSON_OBJECT_SIZE(3) + 6*JSON_OBJECT_SIZE(4) + 6*JSON_OBJECT_SIZE(5) + 3*JSON_OBJECT_SIZE(7) + 
                          JSON_OBJECT_SIZE(9) + 12*JSON_OBJECT_SIZE(11) + 3*JSON_OBJECT_SIZE(13) + 5450;
  DynamicJsonDocument doc(capacity);
  
  // Parse result
  DeserializationError error = deserializeJson(doc, client);
  if (error) return 0;

  return doc.size();
  
  client.stop();

} 

void alertLight(int light){
  sendLightCommand(light, "{\"alert\":\"select\"}");
}