#include <ESP8266WiFi.h>        
#include <ESP8266WebServer.h>   
#include <WiFiClient.h>

#include "Relay.hpp"
#include "PIR.hpp"
#include "millisDelay.h"

const char *ssid = "UVLightDevice";  
const char *password = "yourpassword"; 
ESP8266WebServer server(80);    

// Instantiate objects
Relay relay(4);
PIR pirSensor(5,3,15,13,12,14);
millisDelay timer1;
millisDelay timer2;


// Variable declaration
unsigned long timerDurationCmd, waitingTimeVal;
bool controlModeCmd;
bool relayStateCmd, relayControlCmd; 
bool timerCancelCmd, timerStartCmd;
bool waitingTimeFinished;


void setup() {
  // PIR sensors
  pirSensor.configuration();
  pirSensor.pirCheck();

  // Relay
  relay.initialize();

  // Start serial comm. - Debug tool
  Serial.begin(115200);           

  // Configure NODEMCU as Access Point
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid); //Password is not necessary
  IPAddress myIP = WiFi.softAPIP(); //Get the IP assigned to itself.
  Serial.print("AP IP address: "); //This is written in the PC console.
  Serial.println(myIP);

  // Handler association
  server.on("/", handleRootPath); 
  server.on("/Init", handleInit);     
  server.on("/controlCmd", handleControlCmd);   

  // Start server
  server.begin();                 
  Serial.println("Server started!");

  controlModeCmd = 0;
  timerStartCmd = 0;
  timerDurationCmd = 0;
  
  // Variable initialization
  waitingTimeFinished = 0;
  waitingTimeVal = 10000; // ms
}

void loop() {
  server.handleClient();          // Handle the actual incoming of HTTP requests
//  Serial.print("Control Mode Cmd (controlModeCmd): ");
//  Serial.println(controlModeCmd);
//  
//  Serial.print("Relay State Cmd (relayStateCmd): ");
//  Serial.println(relayStateCmd);
//  
//  Serial.print("Timer Duration Cmd (timerDurationCmd): ");
//  Serial.println(timerDurationCmd);
//  
//  Serial.print("Timer Cancel Cmd (timerCancelCmd): ");
//  Serial.println(timerCancelCmd);
//  
//  Serial.print("Timer Start Cmd (timerStartCmd): ");
//  Serial.println(timerStartCmd);

  if(controlModeCmd == 1){ // Mode selection (AUTO mode selected)
    if(timerStartCmd == 1){
          if(!waitingTimeFinished){
          timer1.start(waitingTimeVal); 
          Serial.println("Start moving..."); 
          waitingTimeFinished = !waitingTimeFinished;
        }
      if(timer1.justFinished()){
        Serial.println("Check timer....");  
        relay.ON();
        Serial.println("Turn on relay!");  
        Serial.println(timer2.toMillisec(timerDurationCmd));                              
        timer2.start(timer2.toMillisec(timerDurationCmd));
        }    
      if(timer2.justFinished()){
        Serial.println("Timer2 is working...");
        relay.OFF();
        Serial.println("Turning off relay!");
        }
    }

  }
  else{ // Mode selection (MANUAL mode selected)
    Serial.println("Manual mode enabled!");      
    if(relayStateCmd == 1){
      relay.ON();
      Serial.println("Turn on relay");
      }
    else{
      relay.OFF();
      Serial.println("Turn of relay");
      }
  }    
}

void handleRootPath() {
  server.send(200, "text/plain", "Connected!");
}

void handleInit() {
// Reserved! Do nothing!!!
}

void handleControlCmd(){
  String message = "Initialization with: ";
  // controlMode
  if (server.hasArg("controlMode")) {
  controlModeCmd = (server.arg("controlMode").toInt());
  message += "controlMode: ";
  message += server.arg("controlMode");
  }
  // relayState
  if (server.hasArg("relayState")) {
  relayStateCmd = (server.arg("relayState").toInt());
  //Serial.println(temp);
  message += "relayState: ";
  message += server.arg("relayState");
  }
  // timerDuration
  if (server.hasArg("timerDuration")) {
  timerDurationCmd = (server.arg("timerDuration").toInt());
  message += "timerDuration: ";
  message += server.arg("timerDuration");
  }
  //timerCancel
  if (server.hasArg("timerCancel")) {
  timerCancelCmd = (server.arg("Cancel").toInt());
  message += "timerCancel: ";
  message += server.arg("timerCancel");
  }
  //timerStart
  if (server.hasArg("timerStart")) {
  timerStartCmd = (server.arg("timerStart").toInt());
  message += "timerStart: ";
  message += server.arg("timerStart");
  }
  //timerFinished
  if (server.hasArg("timerFinished")) {
  waitingTimeFinished = (server.arg("timerFinished").toInt());
  message += "timerFinished: ";
  message += server.arg("timerFinished");
  }
  server.send(200, "text/plain", message); 
}
