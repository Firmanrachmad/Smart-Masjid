#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_NAME "Smart Masjid"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define pinLDR 19
#define pinLED_A 22
#define pinLED_B 23
#define pinLED_C 25
#define pinPIR_A 32
#define pinPIR_B 13
#define pinPIR_C 18
#define pinRelay_A 26
#define pinRelay_B 14
#define pinRelay_C 27

#define BLYNK_TEMPLATE_ID "TMPL6GxqT0ZyX"

char auth[] = "gvQ9CKvWVZ9-bbDjdrW4OR-QsGeoA_Hy";

const int numPIRs = 3;  // number of PIR sensors
const int pirPins[numPIRs] = {pinPIR_A, pinPIR_B, pinPIR_C};
const int ledPins[numPIRs] = {pinLED_A, pinLED_B, pinLED_C};
const int relayPins[numPIRs] = {pinRelay_A, pinRelay_B, pinRelay_C};

unsigned long lastMotionTime[numPIRs] = {0};
unsigned long currentTime[numPIRs] = {0};

unsigned long offDelay = 1 * 60 * 1000;

//char ssid[] = "Polinema Hotspot LT8 Barat";
//char pass[] = "";

char ssid[] = "Yusuf";
char pass[] = "jalanteluketna278d";

int switchBlynk1 = 0;
int switchBlynk2 = 0;
int switchBlynk3 = 0;
int virtual3 = 0;
int virtual5 = 0;
int virtual6 = 0;

BlynkTimer timer;

BLYNK_WRITE(V3){
  virtual3 = param.asInt();
  if (virtual3 == 1){
    digitalWrite(relayPins[0], LOW);
    switchBlynk1 = 1;
    Blynk.virtualWrite(V0, switchBlynk1);
    Blynk.syncVirtual(V3);
  }else{
    digitalWrite(relayPins[0], HIGH);
    switchBlynk1 = 0;
    Blynk.virtualWrite(V0, switchBlynk1);
    Blynk.syncVirtual(V3);
  }
}

BLYNK_WRITE(V4){
  int virtual4 = param.asInt();
  if (virtual4 == 1){
    digitalWrite(relayPins[1], LOW);
    switchBlynk2 = 1;
    Blynk.virtualWrite(V1, switchBlynk2);
  } else {
    digitalWrite(relayPins[1], HIGH);
    switchBlynk2 = 0;
    Blynk.virtualWrite(V1, switchBlynk2);
    Blynk.syncVirtual(V4);
  }
}

BLYNK_WRITE(V5){
  int virtual5 = param.asInt();
  if (virtual5 == 1){
    digitalWrite(relayPins[2], LOW);
    switchBlynk3 = 1;
    Blynk.virtualWrite(V2, switchBlynk3);
  } else {
    digitalWrite(relayPins[2], HIGH);
    switchBlynk3 = 0;
    Blynk.virtualWrite(V2, switchBlynk3);
    Blynk.syncVirtual(V5);
  }
}

void triggerBlynk(int index,int status){
    if(index == 0){
      Blynk.virtualWrite(V0, status);
    }else if(index == 1){
      Blynk.virtualWrite(V1, status);
    }else if(index == 2){
      Blynk.virtualWrite(V2, status);
    }
}

void SendSensor () {
  float LDR_val = digitalRead(pinLDR);
  int pirValues[numPIRs] = {0};

  // Read PIR values
  for (int i = 0; i < numPIRs; i++) {
    pirValues[i] = digitalRead(pirPins[i]);
    Serial.print("PIR ");
    Serial.print(i+1);
    Serial.print(" = ");
    Serial.println(pirValues[i]);
  }

  // Cek Variabel
  Serial.print("Lux :");
  Serial.println(LDR_val);

  // Rules Otomatisasi Lampu
  if (LDR_val == 1) {  // Kondisi Cahaya Gelap
    bool motionDetected = false;
    for (int i = 0; i < numPIRs; i++) {
      if (pirValues[i] == 1) {
         Serial.print("lastMotionTime");
         Serial.print(" = ");
         Serial.println(lastMotionTime[i]);
        digitalWrite(ledPins[i], HIGH);
        digitalWrite(relayPins[i], LOW);
        triggerBlynk(i,1);
        lastMotionTime[i] = currentTime[i];
        motionDetected = true;
      } else if (!motionDetected && (currentTime[i] - lastMotionTime[i] > offDelay)) {
      Serial.print("matikan");
      Serial.print(" = ");
      Serial.println(i);
      for (int i = 0; i < numPIRs; i++) {
        // MATI
        digitalWrite(ledPins[i], LOW);
        // MATI
        digitalWrite(relayPins[i], HIGH);
        triggerBlynk(i,0);
      }
    }
   }
    
    
  } else {  // Kondisi Cahaya Terang
    for (int i = 0; i < numPIRs; i++) {
      digitalWrite(ledPins[i], LOW);
      digitalWrite(relayPins[i], HIGH);
      triggerBlynk(i,0);
    }
  }

  for (int i = 0; i < numPIRs; i++) {
    currentTime[i] = millis();
  }
  
  delay(1000);
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < numPIRs; i++) {
    pinMode(pirPins[i], INPUT);
    pinMode(ledPins[i], OUTPUT);
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH);
    currentTime[i] = millis();
  }
  
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(2000L, SendSensor);
}

void loop() {
  Blynk.run();
  timer.run();
}
