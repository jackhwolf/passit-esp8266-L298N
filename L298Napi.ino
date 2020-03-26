#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h> 
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>   

ESP8266WiFiMulti wifiMulti;     
ESP8266WebServer server(80);    

void handleRoot();              
void handleNotFound();
void handleMotor();

// ===============
// CODE FOR MOTOR 
// ===============

int ena = 4;
int in1 = 0;
int in2 = 2;
int DIRECTION = 0;
int POWER = 0;

void motor_setup() {
  pinMode(ena, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);  
//  digitalWrite(ena, HIGH);
  digitalWrite(ena, 50  );
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}

void motor_dir0() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  DIRECTION = 0;
  POWER = 1;
}

void motor_dir1() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  DIRECTION = 1;
  POWER = 1;  
}

void motor_flip_direction() {
  if (POWER == 1) {
    digitalWrite(in1, !digitalRead(in1));
    digitalWrite(in2, !digitalRead(in2));
    if (DIRECTION == 0) { DIRECTION = 1; }
    else { DIRECTION = 0; }
  } 
}

void motor_off() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  POWER = 0;
}


// setup wifi and bind responses
void setup(void){
  pinMode(LED_BUILTIN, OUTPUT);
  motor_setup();
  
  Serial.begin(115200);         
  delay(10);
  Serial.println('\n');
  wifiMulti.addAP("SSID", "PASSWORD");   
  Serial.println("Connecting ...");

  while (wifiMulti.run() != WL_CONNECTED) { 
    delay(250);
    Serial.print('.');
  }
  
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           

  if (MDNS.begin("esp8266")) {              
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  server.on("/", handleRoot);               
  server.on("/motor", handleMotor);
  server.onNotFound(handleNotFound);

  server.begin();                           
  Serial.println("HTTP server started");
}

// handle incoming requests
void loop(void){
  server.handleClient();                    
}

void handleRoot() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "ESP8266 web server running!");
  delay(1000);
}

void handleNotFound(){
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(404, "text/plain", "404: Not found"); 
  delay(1000);
}

void handleMotor() {
  int status = 200;
  String msg = "MOTOR: ";
  if (server.hasArg("power")) {
    int power = server.arg("power").toInt();
    if (power == POWER) {
      msg += "no power change needed.";
    } else if (power == 1) {
      msg += "power on default direction (0).";
      motor_dir0();
    } else if (power == 0) {
      msg += "power off.";
      motor_off();
    } else {
      status = 400;
      msg += "invalid value for power.";
    }
    POWER = power;
  }
  if (POWER == 1 && server.hasArg("direction")) {
    int direction = server.arg("direction").toInt();
    if (direction == DIRECTION) {
      msg += " no direction change needed.";
    } else if (direction == 0) {
      msg += " moving in direction 0."; 
      motor_dir0();
    } else if (direction == 1) {
      msg += " moving in direction 1.";
      motor_dir1();
    } else if (direction == 2) {
      msg += " switching direction.";
      motor_flip_direction();
    }
    DIRECTION = direction;
  }
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(status, "text/plain", msg);
}
