/*******************************************************************
    A sketch for controlling a toy car with using a web page 
    hosted on a ESP8266
    
    Main Hardware:
    - NodeMCU Development Board cp2102 (Look for the one with the square chip beside the USB port)
    - NodeMCU Motor Shield (L2932)
    
    Written by Brian Lough
    https://www.youtube.com/channel/UCezJOfu7OtqGzd5xrP3q6WA
 *******************************************************************/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// These are the pins used to control the motor shield

#define motorb_pwm D2 // Motor B
#define motorb_dir D4

#define motorA_pwm D1 // Motor A
#define motora_dir D3

// drivePower sets how fast the car goes
// Can be set between 0 and 1023 (although car problaly wont move if values are too low)
int drivePower = 600;

// driveDirection sets what direction the car drives
// If the car is moving backwards when you press the forward button, change this to LOW
uint8_t driveDirection = HIGH;

// steeringPower sets how fast the car turns
// Can be set between 0 and 1023 (again, car probably won't steer if the value is too low)
int steeringPower = 600;

// steerDirection sets what direction the car steers
// If the car is steering right when you press the left button, change this to LOW
uint8_t steerDirection = HIGH;

// ----------------
// Set your WiFi SSID and Password here
// ----------------
const char* ssid = "test";
const char* password = "test1234";

ESP8266WebServer server(80);

const char *webpage = 
#include "motorPage.h"
;

void handleRoot() {

  server.send(200, "text/html", webpage);
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void){

  pinMode(motorb_pwm, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  pinMode(motorb_dir, OUTPUT);
  pinMode(motorA_pwm, OUTPUT);
  pinMode(motora_dir, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("WifiCar")) {
    Serial.println("MDNS Responder Started");
  }

  server.on("/", handleRoot);

  server.on("/forward", [](){
    Serial.println("forward");
    analogWrite(motorb_pwm, drivePower);
    digitalWrite(motorb_dir, driveDirection);
    analogWrite(motorA_pwm, steeringPower);
    digitalWrite(motora_dir, !steerDirection);    
    server.send(200, "text/plain", "forward");
  });

  server.on("/driveStop", [](){
    Serial.println("driveStop");
    analogWrite(motorb_pwm, 0);
    analogWrite(motorA_pwm, 0);
    server.send(200, "text/plain", "driveStop");
  });

  server.on("/back", [](){
    Serial.println("back");
    analogWrite(motorb_pwm, drivePower);
    digitalWrite(motorb_dir, !driveDirection);
    analogWrite(motorA_pwm, steeringPower);
    digitalWrite(motora_dir, steerDirection); 
    server.send(200, "text/plain", "back");
  });

  server.on("/right", [](){
    Serial.println("right");
    analogWrite(motorA_pwm, steeringPower);
    digitalWrite(motora_dir, steerDirection);
    analogWrite(motorb_pwm, drivePower);
    digitalWrite(motorb_dir, driveDirection);   
    server.send(200, "text/plain", "right");
  });

  server.on("/left", [](){
    Serial.println("left");
    analogWrite(motorA_pwm, steeringPower);
    digitalWrite(motora_dir, !steerDirection);
    analogWrite(motorb_pwm, drivePower);
    digitalWrite(motorb_dir, !driveDirection);   
    server.send(200, "text/plain", "left");
  });

  server.on("/steerStop", [](){
    Serial.println("steerStop");
    analogWrite(motorA_pwm, 0);
    analogWrite(motorb_pwm, 0);
    server.send(200, "text/plain", "steerStop");
  });

  server.on("/speed25", [](){
    Serial.println("25");
    steeringPower = 250;
    drivePower = 250;
    
    server.send(200, "text/plain", "driveStop");
  });
  server.on("/speed50", [](){
    Serial.println("50");
    steeringPower = 500;
    drivePower = 500;
    
    server.send(200, "text/plain", "driveStop");
  });

  server.on("/speed70", [](){
    Serial.println("50");
    steeringPower = 700;
    drivePower = 700;
    
    server.send(200, "text/plain", "driveStop");
  });

    server.on("/speed100", [](){
    Serial.println("50");
    steeringPower = 1000;
    drivePower = 1000;
    
    server.send(200, "text/plain", "driveStop");
  });

  
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP Server Started");
}

void loop(void){
  server.handleClient();
}
