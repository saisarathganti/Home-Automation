#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>  
#include "DHT.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>
#define IR_LED D1
IRsend irsend(IR_LED);
#define FIREBASE_HOST "iotfirebaseproject-8f0c3.firebaseio.com"
#define FIREBASE_AUTH "nim2GI1La5UqBXeUbUTX4MM8c7jtg42FOHzOKDqc"
#define WIFI_SSID "Sarath Chandra"
#define WIFI_PASSWORD "qwerty12"

String email="saisarathganti@gmail.com";
String password="1234567890";
// Pin Definitions
#define DHTPIN D2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
int sensorValue;
int isSmokepin = D4;
int isFlamePin = D3;
int isFlame = HIGH;
int isSmoke = HIGH;
bool smoke1 = 0;
bool smoke2 = 0;
bool flame1 = 0;
bool remote=LOW;
int leng=email.length()-10;

String tru=email.substring(0,leng);
String fin=tru+password;
void setup() 
{
    Serial.begin(9600);
    pinMode(D2, INPUT);
    dht.begin();
    while (!Serial) ;
    Serial.println("start");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("connecting");
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
    }
    Serial.println();
    Serial.print("connected: ");
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Serial.println(WiFi.localIP());
    pinMode(isSmokepin, INPUT);
    pinMode(isFlamePin, INPUT);
    pinMode(D8, OUTPUT);
    pinMode(D5, OUTPUT);
    pinMode(D6, INPUT);
    pinMode(D0, OUTPUT);
    pinMode(D9, OUTPUT);
    irsend.begin();
}

void loop() 
{
  delay(2000);
  smokeRead();
  smokeDetect();
  flameDetect();
  if (flame1 == 1 || smoke1 == 1 || smoke2 == 1) {
    Serial.println("Warnig, Warning, Warning");
  }
  int bul=Firebase.getInt("users/"+fin+"/light");
  int mosen = digitalRead(D6);
  if(mosen){
    digitalWrite(D9,HIGH);
  }
  else{
    digitalWrite(D9,LOW);
  }
  if(bul){
   digitalWrite(D8,HIGH);
   Serial.println(bul);
  }
  else{
    digitalWrite(D8,LOW);
  }
  int trp=Firebase.getInt("users/"+fin+"/trp");
  if(trp==1){
   irsend.sendSony(0xa90, 12, 2);
   Firebase.setInt("users/"+fin+"/trp",0);
  }
  int trpup=Firebase.getInt("users/"+fin+"/trpup");
  if(trpup==1){
   irsend.sendSony(0x0025D5AC5F, 40, 2);
   Firebase.setInt("users/"+fin+"/trpup",0);
  }
  int trpdown=Firebase.getInt("users/"+fin+"/trpdown");
  if(trpdown==1){
   irsend.sendSony(0x00FFE01FUL, 40, 2);
   Firebase.setInt("users/"+fin+"/trpdown",0);
  }
  int all=Firebase.getInt("users/"+fin+"/alarm");
  Serial.println(all);
  if(all){
   digitalWrite(D5,HIGH);
  }
  else{
    digitalWrite(D5,LOW);
  }
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
  }
  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);
  if(Firebase.getInt("users/"+fin+"/temperature")!=2147483647){
  Firebase.setInt("users/"+fin+"/temperature",t);
  Firebase.setInt("users/"+fin+"/humidity",h);
  }
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");    
}

//////////////////////////////////  

void smokeDetect() {
  isSmoke = digitalRead(isSmokepin);
  if (isSmoke == LOW) {
    smoke1 = 1;
    Serial.println("SMOKE, SMOKE, SMOKE");
    Firebase.setInt("users/"+fin+"/DigitalSmoke",1);
    digitalWrite(D5,HIGH);
    digitalWrite(D0,HIGH);
  } else {
    smoke1 = 0;
    Firebase.setInt("users/"+fin+"/DigitalSmoke",0);
    Serial.println("NO SMOKE");
    digitalWrite(D5,LOW);
    digitalWrite(D0,LOW);
  }
}

void flameDetect() {
  isFlame = digitalRead(isFlamePin);
  if (isFlame == LOW) {
    flame1 = 1;
    Serial.println("FLAME, FLAME, FLAME");
    Firebase.setInt("users/"+fin+"/fire",1);
    digitalWrite(D0,HIGH);
  } else {
    flame1 = 0;
    Firebase.setInt("users/"+fin+"/fire",0);
    digitalWrite(D0,LOW);
    Serial.println("no flame");
  }
}

void smokeRead() {
  sensorValue = analogRead(A0);
  Serial.println(sensorValue);
  if (sensorValue > 250) {
    smoke2 = 1;
    Serial.println("SMOKE, SMOKE, SMOKE");
    Firebase.setInt("users/"+fin+"/AnalogSmoke",1);
  } else {
    smoke2 = 0;
    Firebase.setInt("users/"+fin+"/AnalogSmoke",0);
    Serial.println("NO SMOKE");
  }
}
