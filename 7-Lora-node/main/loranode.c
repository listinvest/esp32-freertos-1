/*
 Esp32_2 - Ar condicionado
*/

#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <Wire.h>
#include <SSD1306.h>

#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DI00 26
#define BAND 915E6

float tempC = 0;
int state = 0;
String stateString = "";
String node = "";
String nodeString = "";

String pwmFreqString = "";
int pwmFreq = 0;
int lastPwmFreq = 0;

SSD1306 display(0x3c,4,15);

void setup() {
  Serial.begin(115200);                   // initialize serial
  while (!Serial);

  setupDisplay();
  
  config_PWM(pwmFreq);                       //PWM frequency
  
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI00);

  if (!LoRa.begin(BAND)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  Serial.println("LoRa init succeeded.");
  Serial.println();
  Serial.println("LoRa Simple Node");
  Serial.println("Only receive messages from gateways");
  Serial.println("Tx: invertIQ disable");
  Serial.println("Rx: invertIQ enable");
  Serial.println();

  LoRa.onReceive(onReceive);
  LoRa_rxMode();
}

void loop() {
  if (runEvery(1000)) { // repeat every 1000 millis

    coletaTemp();

    String message = "a ";
    //message += "I'm a Node! ";
    message += tempC;

    LoRa_sendMessage(message); // send a message

    Serial.println("Send Message!");
  }
  if(lastPwmFreq != pwmFreq){
    config_PWM(pwmFreq);
    lastPwmFreq = pwmFreq;
  }
  
  ledcWrite(0, state);
  
}

void LoRa_rxMode(){
  LoRa.enableInvertIQ();                // active invert I and Q signals
  LoRa.receive();                       // set receive mode
}

void LoRa_txMode(){
  LoRa.idle();                          // set standby mode
  LoRa.disableInvertIQ();               // normal mode
}

void LoRa_sendMessage(String message) {
  LoRa_txMode();                        // set tx mode
  LoRa.beginPacket();                   // start packet
  LoRa.print(message);                  // add payload
  LoRa.endPacket();                     // finish packet and send it
  LoRa_rxMode();                        // set rx mode
}

void onReceive(int packetSize) {
  String message = "";

  while (LoRa.available()) {
    message += (char)LoRa.read();
  }
  nodeString = message;
  nodeString.remove(1);
  node = nodeString;

  stateString = message;
  stateString.remove(0,1);
  stateString.remove(6);
  state = stateString.toInt();
  Serial.println(state);

  pwmFreqString = message;
  pwmFreqString.remove(0,9);
  pwmFreq = pwmFreqString.toInt();
  Serial.println(pwmFreq);

  Serial.print("Node Receive: ");
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

void setupDisplay(void){  
  pinMode(16,OUTPUT);
  digitalWrite(16,LOW);
  digitalWrite(16,HIGH);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);//10,16 e 24
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}

void coletaTemp(void){
  const int analogIn = 2;
  int RawValue= 0;
  float Voltage = 0;
  float media[100];
  
  for(int i=0;i<100;i++){
  RawValue = analogRead(analogIn);
  Voltage = (RawValue / 2048.0) * 3300; // 5000 to get millivots.
  media[i] = Voltage * 0.1;
  tempC += media[i];
  }
  tempC /= 100;

  display.clear();
  display.drawString(0,0,(String) tempC + "ºC");
  display.display();
}

void config_PWM(int frequencia){
  // the number of the LED pin
  const int ledPin = 17;  // 17 corresponds to GPIO17

  // setting PWM properties
  const int freq = frequencia;
  const int ledChannel = 0;
  const int resolution = 8;
  
  // configure LED PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledPin, ledChannel);
}


