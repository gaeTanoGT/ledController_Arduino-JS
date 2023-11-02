#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>

#include <IRremoteESP8266.h>

#include <Arduino.h>
#include <IRsend.h>

#define RELE D6 //pin 6 rele

const char* ssid = "TIM-19445283";
const char* password = "bhszxPfdYtNHC5YwsO6CnRLw";

WebSocketsServer webSocket(80); // Crea un server WebSocket sulla porta 80

const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRsend irsend(kIrLed);

bool statoLed = false;
bool ledArmadio = true;
int luminosita = 50;  //0 - 50

void setup() {
  pinMode(2, OUTPUT);
  pinMode(RELE, OUTPUT);
  digitalWrite(RELE, LOW);

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.println("Connessione WiFi in corso...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nWiFi " + String(ssid) + " connesso!");
  Serial.println(WiFi.localIP());
  digitalWrite(2, statoLed);

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  irsend.begin();

  setupLuminosita();
}

void loop() {
  webSocket.loop();
}

void gestisciMessaggio(uint8_t * payload, size_t length) {

  String messaggio = "";
  for (size_t i = 0; i < length; i++) {
    messaggio += (char)payload[i];
  }
  
  Serial.println("Messaggio WebSocket: " + messaggio);
  if (messaggio == "ON") {
    statoLed = false;
    irsend.sendNEC(0xF7C03F); //ON
  } else if (messaggio == "OFF") {
    statoLed = true;
    irsend.sendNEC(0xF740BF); //OFF
  } else if(messaggio.substring(0, 3) == "Lum") {
    luminosita = messaggio.substring(4,6).toInt();
    setLuminosita(luminosita);
  } else if(messaggio == "Armadio=ON") {
    ledArmadio = true;
    digitalWrite(RELE, HIGH);
  } else if(messaggio == "Armadio=OFF"){
    ledArmadio = false;
    digitalWrite(RELE, LOW);
  } else {
    if(messaggio == "Color_1"){
        irsend.sendNEC(0xF720DF);
    }
    if(messaggio == "Color_2"){
        irsend.sendNEC(0xF710EF);
    }
    if(messaggio == "Color_3"){
        irsend.sendNEC(0xF730CF);
    }
    if(messaggio == "Color_4"){
        irsend.sendNEC(0xF708F7);
    }
    if(messaggio == "Color_5"){
        irsend.sendNEC(0xF728D7);
    }
    if(messaggio == "Color_6"){
        irsend.sendNEC(0xF7A05F);
    }
    if(messaggio == "Color_7"){
        irsend.sendNEC(0xF7906F);
    }
    if(messaggio == "Color_8"){
        irsend.sendNEC(0xF7B04F);
    }
    if(messaggio == "Color_9"){
        irsend.sendNEC(0xF78877);
    }
    if(messaggio == "Color_10"){
        irsend.sendNEC(0xF7A857);
    }
    if(messaggio == "Color_11"){
        irsend.sendNEC(0xF7609F);
    }
    if(messaggio == "Color_12"){
        irsend.sendNEC(0xF750AF);
    }
    if(messaggio == "Color_13"){
        irsend.sendNEC(0xF7708F);
    }
    if(messaggio == "Color_14"){
        irsend.sendNEC(0xF748B7);
    }
    if(messaggio == "Color_15"){
        irsend.sendNEC(0xF76897);
    }
    if(messaggio == "Color_16"){
        irsend.sendNEC(0xF7E01F);
    }
    if(messaggio == "Color_17"){
        irsend.sendNEC(0xF7C837);
    }
    if(messaggio == "Color_18"){
        irsend.sendNEC(0xF7E817);
    }
  }

  digitalWrite(2, statoLed);

  //do riscontro
  inviaInBroadcast(messaggio);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("Host %u disconnesso.\n", num);
      break;
    case WStype_CONNECTED:{
      Serial.printf("Cliente %u connesso.\n", num);
      String msg;
      if(statoLed){
        msg = "OFF";
      }
      else{
        msg = "ON";
      }

      webSocket.sendTXT(num, (uint8_t*)msg.c_str(), msg.length());
      if(ledArmadio){
        msg = "Armadio=ON";
      }
      else{
        msg = "Armadio=OFF";
      }
      webSocket.sendTXT(num, (uint8_t*)msg.c_str(), msg.length());

      msg = "Lum=" + String(luminosita);
      //Serial.println(msg);
      webSocket.sendTXT(num, (uint8_t*)msg.c_str(), msg.length());
      break;
    }
    case WStype_TEXT:
      gestisciMessaggio(payload, length);
      break;
  }
}

  //scorro tutti i client connessi per inviare il messaggio in broadcast
void inviaInBroadcast(String payload) {
  for (uint8_t i = 0; i < webSocket.connectedClients(); i++) {
    webSocket.sendTXT(i, (uint8_t*)payload.c_str(), payload.length());
  }
}

void setupLuminosita(){
    for(int i = 0; i <= 50; i++){
        irsend.sendNEC(0xF700FF);
    }

    luminosita = 50;

    irsend.begin();
}

void setLuminosita(int newVal){
    if(newVal > luminosita){
        for(int i = luminosita; i <= newVal; i++){
            irsend.sendNEC(0xF700FF);
            delay(50);
        }       
    }else{
        for(int i = luminosita; i >= newVal; i--){
            irsend.sendNEC(0xF7807F);
            delay(50);
        } 
    }
}