#include <DeloreanClock.h>

// Change this to config.h and update the config.h with your own information.
// config.h is ignored from the git projec to prevent sensitive personal information
// from being made public. Use config_example.h as a template for creating your own
// clock  
#include "config_example.h"

DeloreanClock delorean;

void setup() {
  startSerial();
  connectWifi();//blocking

  //Setup the clock
  delorean.setTimezone(usMT); // List of available timezones can be found in DeloreanClock.h
  delorean.updateConfig(jsonConfig); //Load from config.h or config_example.h files
  delorean.start();
}

void loop() {
  delorean.update();
}

void startSerial(){
  Serial.begin(115200);
  Serial.println();
}


void connectWifi(){
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

