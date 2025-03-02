#include <Arduino.h>
#include <Wifi.h>
#include <Wire.h>
#include "BluetoothSerial.h"
#include <LiquidCrystal_I2C.h>

// To support SSL/TLS
#include <WiFiClientSecure.h>

// Mqtt Client Library
#include <PubSubClient.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

//LCD Setup
LiquidCrystal_I2C lcd(0x27,16,2); // address, col, rows 

//Wifi Credentials
const char* ssid = "Your_WiFi_SSID"; 
const char* password = "WiFi Pass";
const int timeout = 20000;

//MQTT Broker
const char* mqttServer = "Your_MQTT_BROKER_URL";
const int mqttPort= 8883 ;
const char* mqttUser = "esp32-demo"; //Credentials for connection to MQ Broker
const char* mqttpass = "Demo1234";
unsigned long previousTime = 0; // to replace delay, prevents loop from blocking and allowing messages from mqtt to be read
const long interval = 2000;

//Bluetooth
BluetoothSerial SerialBT;
const int ledPin = 2;

//handling bluetooth message
String message = "";
char incomingChar;

//Reading analog Setup at pin 34
const int potPin = 34;
int potValue = 0;

// Custom Characters:

byte Disconnected[8] = {
  0b11111, //0
  0b11111, //1
  0b01110, //2
  0b00000, //3
  0b00000, //4
  0b01110, //5
  0b11111, //6
  0b11111, //7
};

byte Connected[8] = {
  0b11111, //0
  0b11111, //1
  0b01110, //2
  0b00100, //3
  0b00100, //4
  0b01110, //5
  0b11111, //6
  0b11111, //7
};


WiFiClientSecure espClient ;
PubSubClient client(espClient);

void lcdSetup(){
  lcd.init(); // initialise LCD hardware
  lcd.clear(); // clears lcd and move cursor to (0,0)
  lcd.backlight(); // Make sure backlight is on
  lcd.createChar(0, Disconnected); // Custom characters for showing Bluetooth disconnected
  lcd.createChar(1, Connected); // Custom characters for showing Bluetooth connected
  // optional functions
  // lcd.home();// moves to (0,0) without clearing 
  // lcd.blink(); //displays blinking cursor at next char to be written
  // lcd.cursor(); //unscore the postion at next char to be written
  // lcd.noBlink(); // offs blinking cursor
  // lcd.noCursor(); // hides cursor
  // lcd.scrollDisplayRight() // scrolls contents 1 space to the right
  // lcd.scrollDisplayLeft() // scrolls contents 1 space to the left
}


void callback(char* topic, byte* message, unsigned int length){
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i =0; i<length; i++){
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (String(topic)=="esp32/output"){
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPin,HIGH);
      lcd.setCursor(10,1);
      lcd.print("      "); // clear the last 6 chars
      lcd.setCursor(10,1);
      lcd.print("ON");
    }
    else if (messageTemp=="off"){
      Serial.println("off");
      digitalWrite(ledPin,LOW);
      lcd.setCursor(10,1);
      lcd.print("OFF");
    }
  }
}



void mqttSetup(){
  client.setServer(mqttServer,mqttPort);
  client.setCallback(callback);
  while(!client.connected()){
    Serial.print("MQTT not connected... Trying to connect...");
    String clientId = "ESP32Client";

    // Attempt connection
    if (client.connect(clientId.c_str(),mqttUser,mqttpass)){
      Serial.println("Connected!");
      client.subscribe("esp32/output");
    }
    else{
      Serial.println("Failed, response code=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

// Function to publish data to a specific MQTT topic
void publishData(const char* topic, int value) {
  char message[10];
  itoa(value,message,10);

  if (client.publish(topic, message)) {
    Serial.println("Message Published");
  } else {
    Serial.println("Message failed to publish");
  }
}


void wiFiSetup(wifi_mode_t mode){
  WiFi.mode(mode);
  WiFi.disconnect();
  delay(100);
  Serial.println("WiFi Setup done");
}

void networkScan(){
  Serial.println("Scan start");
  int n = WiFi.scanNetworks();
  Serial.println("Scan done");
  if (n == 0) {
    Serial.println("No networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " Open" : " Encrypted");
      delay(10);
    }
  }
  Serial.println("");
  delay(5000);
}

void connectToWiFi(){
  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid,password);

  // number of milliseconds that have elapsed since the board was powered on or reset. 
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout){
    Serial.print(".");
    delay(1000);
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed WiFi Connection");
  }
  else{
    Serial.print(ssid);
    Serial.println(" Connected");
    Serial.println(WiFi.localIP());
  }
}


void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin,OUTPUT);
  Serial.begin(115200);
  SerialBT.begin("ESP32test");
  wiFiSetup(WIFI_STA);
  connectToWiFi();
  espClient.setInsecure();
  mqttSetup();
  lcdSetup();

  // Add what I want on the screen
  lcd.setCursor(0,0); // Col 0, row 0, redundant as lcd.clear() is called
  lcd.print("Resistance:"); // 11 chars, 5 left
  lcd.setCursor(0,1);
  lcd.print("LED State:"); // 10 chars, 6 left
  lcd.print("OFF"); // Default state
  delay(100);

}

void loop() {

  //Bluetooth comms
  if (Serial.available()){
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()){
    incomingChar = SerialBT.read();
    if (incomingChar != '\n'){
      message += String(incomingChar);
    }
    else{
      message ="";
    }
    Serial.write(incomingChar);
  }
  // incoming bluetooth messages
  if (message == "on"){
    digitalWrite(ledPin,HIGH);
    lcd.setCursor(10,1);
    lcd.print("      "); // clear the last 6 chars
    lcd.setCursor(10,1);
    lcd.print("ON");
  }
  else if (message == "off"){
    digitalWrite(ledPin,LOW);
    lcd.setCursor(10,1);
    lcd.print("OFF");
  }

  // Non blocking loop

  client.loop(); // Checks for missed mqtt packets

  unsigned long currentTime = millis();
  if (currentTime - previousTime >=interval){
    
    previousTime = currentTime;

    // Reading potentiometer value
    potValue = analogRead(potPin);
    Serial.println(potValue);

    // Display LCD
    lcd.setCursor(11,0);
    lcd.print("     "); // clears last 5 char
    lcd.setCursor(11,0);
    lcd.print(String(potValue));
    
    // Update bluetooth connection status
    lcd.setCursor(15,1);
    if (SerialBT.hasClient()) {  
      lcd.write(1);
      Serial.println("✅ Phone is connected via Bluetooth!");
    } else {
      lcd.write(0);
      Serial.println("❌ No Bluetooth connection.");
    }
    // Publish to MQTT Broker
    publishData("home/resistance", potValue);  // Example topic and data
  }

}
