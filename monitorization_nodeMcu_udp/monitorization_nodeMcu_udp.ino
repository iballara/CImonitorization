#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Servo.h>

int incomingByte = 0;
int jenkins_count = 1;
int tomcat_count = 1;

//WIFI 
const char* ssid = "IgnasiB";

// UDP variables
WiFiUDP Udp;
char packetBuffer[255]; //buffer to hold incoming packet
const int localPort = 2390;
int packetSize;
char inBytes[4];

// GPIO PORTS
const int led_red = 2;
const int led_yellow1 = 12;
const int led_yellow2 = 14;
const int led_green1 = 15;
const int led_green2 = 13;
const int tomcat_led_green = 0;
const int tomcat_led_red = 4;
const int servo_pin = 5;
//Servo servo;

void setup() {
  pinMode(led_red, OUTPUT);
  pinMode(led_green1, OUTPUT);
  pinMode(led_green2, OUTPUT);
  pinMode(led_yellow1, OUTPUT);
  pinMode(led_yellow2, OUTPUT);
  pinMode(tomcat_led_green, OUTPUT);
  pinMode(tomcat_led_red, OUTPUT);
  servo.attach(servo_pin);
  Serial.begin(9600); // Opens serial port, sets data rate to 9600 bps
  connect_to_wifi();
  connect_UDP();
  welcome();
}

void loop() {

  packetSize = Udp.parsePacket();
  if (packetSize) { 
    packetBuffer[0] = '\0';
    packetBuffer[1] = '\0';
    packetBuffer[2] = '\0';
    packetBuffer[3] = '\0';
    int len = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    if (len > 0) {
        packetBuffer[len] = '\0';
    }
  }
  
  if (packetBuffer[0] == 'b') {
    jenkins_green();
    jenkins_count = 1;
  }
  else if (packetBuffer[0] == 'y') {
    jenkins_yellow();
    jenkins_count = 1;
  }
  else if (packetBuffer[0] == 'r') {
    jenkins_red();
    if (jenkins_count == 1) {
      ring_bell();
      jenkins_count = 0;
    }
  }
  else if (packetBuffer[0] == 'a') {
    jenkins_building();
    jenkins_count = 1;
  }

  if (packetBuffer[2] == '1') {
    tomcat_green();
    tomcat_count = 1;
  }
  else if (packetBuffer[2] == '0') {
    tomcat_red();
    if (tomcat_count == 1) {
      ring_bell();
      tomcat_count = 0;
    }
  }
}

void jenkins_green() {
  digitalWrite(led_green1, HIGH);
  digitalWrite(led_green2, HIGH);
  digitalWrite(led_yellow1, LOW);
  digitalWrite(led_yellow2, LOW);
  digitalWrite(led_red, LOW);
  delay(1000);
}

void jenkins_yellow() {
  digitalWrite(led_green1, LOW);
  digitalWrite(led_green2, LOW);
  digitalWrite(led_yellow1, HIGH);
  digitalWrite(led_yellow2, HIGH);
  digitalWrite(led_red, LOW);
  delay(1000);
}

void jenkins_red() {
  digitalWrite(led_green1, LOW);
  digitalWrite(led_green2, LOW);
  digitalWrite(led_yellow1, LOW);
  digitalWrite(led_yellow2, LOW);
  digitalWrite(led_red, HIGH);
  delay(1000);
}

void jenkins_building() {
  // ANADA
  digitalWrite(led_green1, HIGH);
  delay(100);
  digitalWrite(led_green2, HIGH);
  delay(100);
  digitalWrite(led_yellow1, HIGH);
  delay(100);
  digitalWrite(led_yellow2, HIGH);
  delay(100);
  digitalWrite(led_red, HIGH);
  delay(175);
  // TORNADA
  digitalWrite(led_green1, LOW);
  delay(100);
  digitalWrite(led_green2, LOW);
  delay(100);
  digitalWrite(led_yellow1, LOW);
  delay(100);
  digitalWrite(led_yellow2, LOW);
  delay(100);
  digitalWrite(led_red, LOW);
  delay(100);
}

void ring_bell() {
  //servo.write(0);
  //delay(300);
  //servo.write(180);
}

void tomcat_green() {
  digitalWrite(tomcat_led_green, HIGH);
  digitalWrite(tomcat_led_red, LOW);
}

void tomcat_red() {
  digitalWrite(tomcat_led_green, LOW);
  digitalWrite(tomcat_led_red, HIGH);
}


/**********************************
 ******** SETUP METHODS ***********
 *********************************/

void connect_to_wifi() {
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    while(true);  // don't continue
  } 

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid);
  while ( WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  // print your WiFi shield's IP address:
  Serial.println("CONNECTED!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void connect_UDP(){
  Serial.print("");
  Serial.println("Connecting to UDP");

  if(Udp.begin(localPort) == 1) {
    Serial.println("Connetion succesful");
  } else {
    Serial.println("Connection failed");
  }
}

void welcome() {

  for (int i = 0; i < 5; i++) {
    digitalWrite(led_red, HIGH);
    digitalWrite(led_green1, HIGH);
    digitalWrite(led_green2, HIGH);
    digitalWrite(led_yellow1, HIGH);
    digitalWrite(led_yellow2, HIGH);
    digitalWrite(tomcat_led_green, HIGH);
    digitalWrite(tomcat_led_red, HIGH);
    delay(200);
    digitalWrite(led_red, LOW);
    digitalWrite(led_green1, LOW);
    digitalWrite(led_green2, LOW);
    digitalWrite(led_yellow1, LOW);
    digitalWrite(led_yellow2, LOW);
    digitalWrite(tomcat_led_green, LOW);
    digitalWrite(tomcat_led_red, LOW);
    delay(200);
  }
  //servo.write(180);
}

