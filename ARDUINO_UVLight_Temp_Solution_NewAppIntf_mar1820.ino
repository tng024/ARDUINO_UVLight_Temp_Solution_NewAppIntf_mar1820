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
Relay relay(4); // Pin D2
PIR pir(5); // Pin D1
millisDelay timer1; // 3 minutes waiting
millisDelay timer2; // ON time timer
millisDelay timer3; // human detected timer

// Variable declaration
unsigned long timerDurationCmd, waitingTimeVal, remainingTimer1ValMs, remainingTimer2ValMs;
bool controlModeCmd;
bool relayStateCmd, relayControlCmd, relayPrevStateCmd; 
bool timerCancelCmd, timerStartCmd;
bool waitingTimeFinished;
bool humanIsAround, call_once;
bool timer1ResumeRequest, timer2ResumeRequest;

// Add_waitingTimer_to_Manual_Mode branch

void setup() {
  // PIR sensors
  pir.configuration();

  // Relay
  relay.initialize();

  // Start serial comm. - Debug tool
  Serial.begin(9600);           

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

  // To test with app 
  controlModeCmd = 0;
  timerStartCmd = 0;
  timerDurationCmd = 0;
  relayPrevStateCmd = 0;
  humanIsAround = 0;
  call_once = 0;
  timer1ResumeRequest = 0;
  timer2ResumeRequest = 0;

//  // To test with no app available
//  controlModeCmd = 0; 
//  timerStartCmd = 0;
//  timerDurationCmd = 0;
//  relayPrevStateCmd = 0;
  
  // Variable initialization
  waitingTimeFinished = 0;
  waitingTimeVal = 20000; // ms
}

void loop() {
//  Must ENABLE server.handleClient() when using APP
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

// Get command from terminal (To test without app available)
//  relayControlCmd = Serial.parseInt();
//  Serial.print("Relay control command: ");
//  Serial.println(relayControlCmd);

// Check PIR sensor status
  humanIsAround = pir.humanPresenceCheck();
//  Serial.print("Human Detected: ");
//  Serial.println(humanIsAround);
  
  if(controlModeCmd == 1){ // Mode selection (AUTO mode selected)
    if(timerStartCmd == 1){ //&& (humanIsAround != 1)
      if(!waitingTimeFinished){
          timer1.start(waitingTimeVal); 
          Serial.println("Start moving..."); 
          waitingTimeFinished = !waitingTimeFinished;           
        }
      if((humanIsAround) && (!call_once)){
          Serial.println("Human detected!!!");
          if(timer1.isRunning()){
              remainingTimer1ValMs = timer1.remaining();
              Serial.print("timer1 remaining duration: ");
              Serial.println(remainingTimer1ValMs);
              timer1.stop();
              timer1ResumeRequest = 1;
            }
          if(timer2.isRunning()){
              remainingTimer2ValMs = timer2.remaining(); 
              timer2.stop();
              Serial.print("timer2 remaining duration: ");
              Serial.println(remainingTimer2ValMs);
              relay.OFF();
              Serial.println("Turned off relay - Human detected");   
              timer2ResumeRequest = 1;        
            }
          call_once = !call_once;
        }
      if((!humanIsAround) && (timer1ResumeRequest)){
          timer1ResumeRequest = !timer1ResumeRequest;
          timer1.start(remainingTimer1ValMs);   
          Serial.println("Resuming waiting timer...");   
          call_once = !call_once;  
        }
      if((!humanIsAround) && (timer2ResumeRequest)){
          timer2ResumeRequest = !timer2ResumeRequest;
          timer2.start(remainingTimer2ValMs);  
          relay.ON();    
          Serial.println("Resuming remained ON time before human detected");  
          Serial.println("Relay is ON again");
          call_once = !call_once;
        }
      if(timer1.justFinished()){
          Serial.println("3-minutes waiting time is due....");  
          relay.ON();
          Serial.println("Turned on relay - Auto Mode");  
          Serial.print("Timer2 is set to: ");
          Serial.print(timer2.toMillisec(timerDurationCmd));      
          Serial.print(" ms");                      
          timer2.start(timer2.toMillisec(timerDurationCmd));
        }    
      if(timer2.justFinished()){
          Serial.println("Timer2 is due. Turning off relay...");
          relay.OFF();
          Serial.println("Turned off relay - Auto Mode");
        }
    }
  }
  else{ // Mode selection (MANUAL mode selected)      
    if(relay.stateIsChanged(relayStateCmd, relayPrevStateCmd)){ // Check if control command is changed, then toggle relay
      if((relayStateCmd == 1) && (humanIsAround != 1)){
          timer1.start(waitingTimeVal);
          Serial.print("Human detected: ");
          Serial.println(humanIsAround);
          Serial.println("Start 3-minutes timer...");
          call_once = 0;
        }
      else{
          relay.OFF();
          call_once = 0;
          Serial.println("Turned of relay - Manual Mode");
        }      
    }
    else{
      if(humanIsAround && (!call_once)){
          relay.OFF();
          Serial.print("Human detected: ");
          Serial.println(humanIsAround);
          Serial.println("Turned off relay - Human Detected - Manual Mode");
          call_once = !call_once;
        }
    }
    if(timer1.justFinished()){
        relay.ON();
        Serial.println("Turn on relay - Manual Mode");
      }
    relayPrevStateCmd = relayStateCmd;
  }    
}

void handleRootPath() {
  server.send(200, "text/plain", "Connected!");
}

void handleInit() {
  // Reserved! Do nothing!!!
}

void handleControlCmd(){
  String message = "Feedback command signal from Arduino: ";
  // controlMode
  if (server.hasArg("controlMode")) {
    controlModeCmd = (server.arg("controlMode").toInt());
    message += "controlMode: ";
    message += server.arg("controlMode");
  }
  // relayState
  if (server.hasArg("relayState")) {
    relayStateCmd = (server.arg("relayState").toInt()); 
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
