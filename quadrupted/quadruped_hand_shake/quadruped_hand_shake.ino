#include <Servo.h>

Servo servoB1;
Servo servoB2;
Servo servoB3;
Servo servoB4;

Servo servoH1;
Servo servoH2;
Servo servoH3;
Servo servoH4;

Servo servoKN1;
Servo servoKN2;
Servo servoKN3;
Servo servoKN4;

const int servoB1pin = 2;
const int servoB2pin = 3;
const int servoB3pin = 4;
const int servoB4pin = 5;

const int servoH1Pin = 6;
const int servoH2Pin = 7;
const int servoH3Pin = 8;
const int servoH4Pin = 9;

const int servoKN1Pin = 10;
const int servoKN2Pin = 11;
const int servoKN3Pin = 12;
const int servoKN4Pin = 13;


void frontright()
{
  servoB2.write(120);
  servoB3.write(60);
  servoB4.write(90);

  servoH1.write(80);
  servoH2.write(150);
  servoH3.write(130);
  servoH4.write(80);

  servoKN1.write(90);
 
  servoKN3.write(90);
  servoKN4.write(90);

  for (int i = 0; i <= 5; i=i++) {
    servoB1.write(70);
    delay(800);
   servoB1.write(80);
   delay(800);
    servoKN2.write(40);
    delay(200);
    servoKN2.write(70);
    delay(200);
  }
}


void setup() {
  Serial.begin(9600);

  // Attach servos
  servoB1.attach(servoB1pin);
  servoB2.attach(servoB2pin);
  servoB3.attach(servoB3pin);
  servoB4.attach(servoB4pin);

  servoH1.attach(servoH1Pin);
  servoH2.attach(servoH2Pin);
  servoH3.attach(servoH3Pin);
  servoH4.attach(servoH4Pin);

  servoKN1.attach(servoKN1Pin);
  servoKN2.attach(servoKN2Pin);
  servoKN3.attach(servoKN3Pin);
  servoKN4.attach(servoKN4Pin);

  delay(1000);

  // Set all servos to 90 degrees
  servoB1.write(90);
  servoB2.write(90);
  servoB3.write(90);
  servoB4.write(90);

  servoH1.write(90);
  servoH2.write(90);
  servoH3.write(90);
  servoH4.write(90);

  servoKN1.write(90);
  servoKN2.write(90);
  servoKN3.write(90);
  servoKN4.write(90);

  Serial.println("All servos set to 90 degrees");
}

void loop() {
  frontright();
}