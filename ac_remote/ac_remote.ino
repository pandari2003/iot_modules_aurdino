#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_LG.h>

const uint16_t kIrLedPin = 4;     
const int btnOnPin = 12;          
const int btnOffPin = 14;         

IRLgAc ac(kIrLedPin);

void setup() {
  Serial.begin(115200);
  ac.begin();
  
  pinMode(btnOnPin, INPUT_PULLUP);
  pinMode(btnOffPin, INPUT_PULLUP);
  
  ac.setMode(kLgAcCool);
  ac.setTemp(24);
  ac.setFan(kLgAcFanAuto);
}

void loop() {
  if (digitalRead(btnOnPin) == LOW) {
    Serial.println("ON");
    ac.setPower(true);
    ac.send(); 
    delay(500); 
  }

  if (digitalRead(btnOffPin) == LOW) {
    Serial.println("OFF");
    ac.setPower(false);
    ac.send(); 
    delay(500); 
  }
}
