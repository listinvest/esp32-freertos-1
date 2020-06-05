/*
 Esp32_1 - Gateway
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>            // include libraries
#include <LoRa.h>
#include <Wire.h>
#include <SSD1306.h>
#include <DHTesp.h>
#include "mbedtls/aes.h"

#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DI00 26
#define BAND 915E6

///////////////////////////////// Variables ///////////////////////////////// 

float tempC_a = 0;
float tempC_b = 0;
float tempC_c = 0;

float humid_a = 0;
float humid_b = 0;
float humid_c = 0;

float tempC = 0;
float humid = 0;
String node = "";

const float tempMin = 25;   // 25ºC -> temp mínima

float temperature = 0;
float humidity = 0;
String device = "";

const int buttonPin = 0;
bool _state = false;
bool v = false;
int cont = 0;

const char* ssid = "NET_2G5740A6";
const char* password = "055740A6";

const char* mqttServer = "192.168.0.79";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";

int messageControl;
int stateButton;
int refTime = 1582033560;

int timestamp;

int initRTC = 1;

DHTesp dht;

char txtMsg[17];
char *plainText = txtMsg;
char *key = "automacaoLARAUnB";
unsigned char cipherTextOutput[17];
unsigned char decipheredTextOutput[17];

int onOff = 2;

///////////////////////////////// Iniciation ///////////////////////////////// 

SSD1306 display(0x3c,4,15);
 
WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
 
  Serial.begin(115200);

  dht.setup(13,DHTesp::DHT22);            // GPIO13
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
      Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
      Serial.println("Connecting to MQTT...");
 
      if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
          Serial.println("MQTT connected");  
      }
      else {
          Serial.print("failed with state ");
          Serial.print(client.state());
      }
  }
 
  client.subscribe("automatico/manual");
  client.subscribe("liga/desliga");
  client.subscribe("realTime");
  client.subscribe("onOffState");

  setupDisplay();

  pinMode(buttonPin, INPUT_PULLUP);

  start_LoRa();

  LoRa.onReceive(onReceive);
  LoRa_rxMode();
 
}
 
void loop() {
  client.loop();
  
///////////////////////////////// Sending data /////////////////////////////////

  if (runEvery(500)) { // repeat every 0.5 sec

    String msg = "A";
    //msg += "I'm a Gateway! ";
    msg += tempMin;
    msg += messageControl;
    msg += stateButton;
    
    while(msg.length() < 16) msg += "0";

    msg.toCharArray(txtMsg,17);// String -> Char array
    encrypt(plainText, key, cipherTextOutput); // Encrypting message

    String message = "";
    for (int i = 0; i < msg.length(); i++) {
      message += String((char)cipherTextOutput[i]);// Char array -> String
    }
    
    LoRa_sendMessage(message); // send a message
   
    // Serial.println("Send Message A!");

    Serial.print("\n");
    Serial.print("LoRa message: A = ");
    Serial.println(msg);
    Serial.print("LoRa encrypted message: A = ");
    Serial.println(message);

    char t[8];
    char h[8];
    dtostrf(dht.getTemperature(), 1, 2, t);
    dtostrf(dht.getHumidity(), 1, 2, h);

    client.publish("temperatureA",t);
    client.publish("humidityA",h);

    char o[8];
    dtostrf(onOff, 1, 2, o);
    client.publish("state", o);
  }
  
/********************************* Update Date Logic *********************************/
  if (initRTC == 1){
      
    if (runEveryRTC(500)){ // repeat every 0.5 sec

      String msg = "T";
      msg += refTime;

      while(msg.length() < 16) msg += "0";

      msg.toCharArray(txtMsg,17);// String -> Char array
      encrypt(plainText, key, cipherTextOutput); // Encrypting message
  
      String message = "";
      for (int i = 0; i < msg.length(); i++) {
        message += String((char)cipherTextOutput[i]);// Char array -> String
      }
        
      LoRa_sendMessage(message); // send a message
       
      // Serial.println("Send Message T!");

    Serial.print("\n");
    Serial.print("LoRa message: T = ");
    Serial.println(msg);
    Serial.print("LoRa encrypted message: T = ");
    Serial.println(message);
    }
  }

///////////////////////////////// Using received data /////////////////////////////////

/********************************* Temperature and Humidity *********************************/  
  if (node == "a"){
    /*
      Temperatura interna
    */
    
    tempC_a = tempC;
    humid_a = humid;
  }

  else if (node == "b"){
    /*
      Temperatura externa 1
    */
        
    tempC_b = tempC;
    humid_b = humid;
  }

  else if (node == "c"){
    /*
      Temperatura externa 2
    */
        
    tempC_c = tempC;
    humid_c = humid;
  }

  OLEDButton();

  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.drawString(0,0,"Ambiente: " + device);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0,20,(String) temperature + " ºC");
  display.drawString(0,40,(String) humidity + " %");
  display.display();  
   
//  display.clear();
//  display.setFont(ArialMT_Plain_24);
//  display.drawString(0,0,(String) temperature + "ºC");
//  display.setFont(ArialMT_Plain_10);
//  display.drawString(0,30,"Temperatura " + device);
//  display.display();

}

///////////////////////////////// Functions ///////////////////////////////// 
void start_LoRa(){

  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI00);
  if(!LoRa.begin(BAND)){
    Serial.println("Starting LoRa failed");
    while(1);
  }

  Serial.println("LoRa initiation was a success");
  
  LoRa.setPreambleLength(6);
  LoRa.setTxPower(17,1);
  LoRa.setSpreadingFactor(10);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(4);
  LoRa.setSyncWord(0xAA);
  LoRa.beginPacket(true);
  LoRa.enableCrc();
}

void callback(char* topic, byte* payload, unsigned int length) {

  String controle;

  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  
  for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      controle += (char)payload[i];
  }
  Serial.print("\n");

  //Control implementation via NodeRed:
  /* messageControl: 2 = auto / 1 = on / 0 = off */

  if (String(topic) == "automatico/manual") {
    if(controle == "automatico"){
       messageControl = 2;
       stateButton = 1;
    }
    else if(controle == "manual"){
       stateButton = 2;
    }
  }
  
  else if (String(topic) == "liga/desliga") {
    if(controle == "liga" && stateButton == 2){
       messageControl = 1;
    }
    
    else if(controle == "desliga" && stateButton == 2){
       messageControl = 0;
    }
  }
  //Real Time reference
  if (String(topic) == "realTime"){
    controle.remove(10);
    refTime = controle.toInt();
  }
}

void LoRa_rxMode(){
  LoRa.disableInvertIQ();             // normal mode
  LoRa.receive();                     // set receive mode
}

void LoRa_txMode(){
  LoRa.idle();                        // set standby mode
  LoRa.enableInvertIQ();              // active invert I and Q signals
}

void LoRa_sendMessage(String message) {
  LoRa_txMode();                      // set tx mode
  LoRa.beginPacket();                 // start packet
  LoRa.print(message);                // add payload
  LoRa.endPacket();                   // finish packet and send it
  LoRa_rxMode();                      // set rx mode
}

void onReceive(int packetSize) {
  
  String msg = "";
  String message = "";
  String type = "";
  String typeString = "";
  String nodeString = "";
  String tempCString = "";
  String humidString = "";
  String timestampString = "";
  String initRTCString = "";
  String onOffString = "";
  char cipherVar[17];

  while (LoRa.available()) {
    msg += (char)LoRa.read();
  }

  msg.toCharArray(cipherVar,17);// String -> Char array

  for (int i = 0; i < msg.length(); i++) {
    cipherTextOutput[i] = (unsigned char)cipherVar[i];// Char -> Unsigned char
  }

  decrypt(cipherTextOutput, key, decipheredTextOutput);// decrypting message

  for (int i = 0; i < msg.length(); i++) {
    message += String((char)cipherTextOutput[i]);// Unsigned char array -> String
  }

  typeString = message;
  typeString.remove(1);
  type = typeString;

  if (type == "t"){
    nodeString = message;
    nodeString.remove(0,0);
    nodeString.remove(2);
    node = nodeString;

    initRTCString = message;
    initRTCString.remove(0,1);
    initRTCString.remove(3);
    initRTC = initRTCString.toInt();

    timestampString = message;
    timestampString.remove(0,2);
    timestampString.remove(14);
    timestamp = timestampString.toInt();
  }

  else{
    node = type;
    
    tempCString = message;
    tempCString.remove(0,0);
    tempCString.remove(6);
    tempC = tempCString.toFloat();

    humidString = message;
    humidString.remove(0,5);
    humidString.remove(11);
    humid = humidString.toFloat();

    onOffString= message;
    onOffString.remove(0,10);
    onOffString.remove(12);
    onOff = onOffString.toInt();
  }

  Serial.print("\n");
  Serial.print("Received LoRa message: ");
  Serial.println(msg);
  Serial.print("Deciphered message: ");
  Serial.println(message);

}

boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
  previousMillis = currentMillis;
  return true;
  }
  return false;
}

boolean runEveryRTC(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
  previousMillis = currentMillis;
  return true;
  }
  return false;
}

void setupDisplay(void)
{  
  pinMode(16,OUTPUT);
  digitalWrite(16,LOW);
  digitalWrite(16,HIGH);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);//10,16 e 24
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}

void OLEDButton(void)
{
  if(cont == 0){
    temperature = tempC_a;
    humidity = humid_a;
    device = "interna";
    }
 
  else if(cont == 1){
    //temperature = tempC_b;
    //humidity = humid_b;
    temperature = dht.getTemperature();
    humidity = dht.getHumidity();
    device = "externa 1";
  }
 
  else if(cont == 2){
    temperature = tempC_c;
    humidity = humid_c;
    device = "externa 2";
  }
  
  if(isReleased()){
    if(cont == 2) cont = 0;
    else cont++;
  }
}

bool isReleased()
{
  v = digitalRead(buttonPin);
    
  if (v != _state) {
      _state = v;
      if (_state) return true;
  }
  return false;
}

void encrypt(char * plainText, char * key, unsigned char * outputBuffer){
 
  mbedtls_aes_context aes;
 
  mbedtls_aes_init( &aes );
  mbedtls_aes_setkey_enc( &aes, (const unsigned char*) key, strlen(key) * 8 );
  mbedtls_aes_crypt_ecb( &aes, MBEDTLS_AES_ENCRYPT, (const unsigned char*)plainText, outputBuffer);
  mbedtls_aes_free( &aes );
}

void decrypt(unsigned char * chipherText, char * key, unsigned char * outputBuffer){
 
  mbedtls_aes_context aes;
 
  mbedtls_aes_init( &aes );
  mbedtls_aes_setkey_dec( &aes, (const unsigned char*) key, strlen(key) * 8 );
  mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, (const unsigned char*)chipherText, outputBuffer);
  mbedtls_aes_free( &aes );
}


