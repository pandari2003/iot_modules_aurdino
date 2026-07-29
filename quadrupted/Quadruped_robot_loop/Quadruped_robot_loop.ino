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

const int servoB1pin = 2;  //front right (servo 1)
const int servoB2pin = 3;  //back right (servo 2)
const int servoB3pin = 4;  //front left (servo 4)
const int servoB4pin = 5;  //back left (servo 3)

const int servoH1Pin = 6;  // back right (servo 8)
const int servoH2Pin = 7;  // Front Right (servo 5)
const int servoH3Pin = 8;  // back Left(servo 7)
const int servoH4Pin = 9;  // front left (servo 6)

const int servoKN1Pin = 10;  // back right (servo 12)
const int servoKN2Pin = 11;  // Front Right (servo 9)
const int servoKN3Pin = 12;  // back Left (servo 11)
const int servoKN4Pin = 13;  // front left (servo 10)

/**************** Set ALL SERVOS TO 90° ****************/
void Set() {

  // Body servos
  servoB1.write(90);
  servoB2.write(90);
  servoB3.write(90);
  servoB4.write(90);

  // Hip servos
  servoH1.write(90);
  servoH2.write(90);
  servoH3.write(90);
  servoH4.write(90);

  // Knee servos
  servoKN1.write(90);
  servoKN2.write(90);
  servoKN3.write(90);
  servoKN4.write(90);

  delay(500);
}

/****************sit*****************/
void Sit() {
  for (int i = 90; i >= 0; i--) {
    // Left pair
    servoH2.write(i);
    servoH3.write(i);
    servoKN2.write(i - 45);
    servoKN3.write(i - 45);

    // Right pair
    servoH1.write(180 - i);
    servoH4.write(180 - i);
    servoKN1.write(230 - i);
    servoKN4.write(230 - i);

    delay(80);  // Same speed as Stand
  }

  delay(60);
}

/******************stand robot********************/  //stand
void Stand() {
  for (int i = 0; i <= 90; i++) {
    // Left pair
    servoH2.write(i);
    servoH3.write(i);
    servoKN2.write(i - 45);
    servoKN3.write(i - 45);

    // Right pair
    servoH1.write(180 - i);
    servoH4.write(180 - i);
    servoKN1.write(230 - i);
    servoKN4.write(230 - i);

    delay(50);  // Adjust speed here
  }

  delay(50);
}
/**************** walk Forward ******************/
void walkForward() {
  // Repeat walking cycle
  for (int step = 0; step < 10; step++) {
    //---------------- STEP 1 ----------------//
    for (int i = 90; i >= 70; i--) {
      servoH1.write(i);
      servoH3.write(i);
      servoH2.write(180 - i);
      servoH4.write(180 - i);

      delay(50);
    }

    // Lift Legs 1 & 4
    for (int k = 90; k >= 70; k--) {
      servoKN1.write(k);
      servoKN3.write(k);
      delay(50);
    }

    // Lower Legs 1 & 4
    for (int k = 70; k <= 90; k++) {
      servoKN1.write(k);
      servoKN3.write(k);
      delay(50);
    }

    //---------------- STEP 2 ----------------//
    for (int i = 70; i <= 110; i++) {
      servoH1.write(i);
      servoH3.write(i);
      servoH2.write(180 - i);
      servoH4.write(180 - i);

      delay(50);
    }

    // Lift Legs 2 & 3
    for (int k = 90; k >= 70; k--) {
      servoKN2.write(k);
      servoKN4.write(k);
      delay(50);
    }

    // Lower Legs 2 & 3
    for (int k = 70; k <= 90; k++) {
      servoKN2.write(k);
      servoKN4.write(k);
      delay(50);
    }

    //---------------- Return Center ----------------//
    for (int i = 110; i >= 90; i--) {
      servoH1.write(i);
      servoH3.write(i);

      servoH2.write(180 - i);
      servoH4.write(180 - i);

      delay(50);
    }
  }
  delay(30);
}
/**************** BACKWARD ****************/
void walkBackward() {

  for (int step = 0; step < 10; step++) {

    //---------------- STEP 1 : Shift Hips ----------------//
    for (int i = 90; i <= 110; i++) {
      servoH1.write(i);
      servoH3.write(i);

      servoH2.write(180 - i);
      servoH4.write(180 - i);

      delay(50);
    }

    //---------------- Lift Legs 1 & 3 ----------------//
    for (int k = 90; k <= 110; k++) {
      servoKN1.write(k);
      servoKN3.write(k);
      delay(50);
    }

    //---------------- Lower Legs 1 & 3 ----------------//
    for (int k = 110; k >= 90; k--) {
      servoKN1.write(k);
      servoKN3.write(k);
      delay(50);
    }

    //---------------- STEP 2 : Shift Hips ----------------//
    for (int i = 110; i >= 70; i--) {
      servoH1.write(i);
      servoH3.write(i);

      servoH2.write(180 - i);
      servoH4.write(180 - i);

      delay(50);
    }

    //---------------- Lift Legs 2 & 4 ----------------//
    for (int k = 90; k <= 110; k++) {
      servoKN2.write(k);
      servoKN4.write(k);
      delay(50);
    }

    //---------------- Lower Legs 2 & 4 ----------------//
    for (int k = 110; k >= 90; k--) {
      servoKN2.write(k);
      servoKN4.write(k);
      delay(50);
    }

    //---------------- Return Center ----------------//
    for (int i = 70; i <= 90; i++) {
      servoH1.write(i);
      servoH3.write(i);

      servoH2.write(180 - i);
      servoH4.write(180 - i);

      delay(50);
    }
  }
  delay(30);
}
/*****************right move**********************/
void Rightmove() {
  for (int step = 0; step < 5; step++) {
    //---------------- STEP 1 ----------------//
    for (int i = 70; i <= 110; i++) {
      servoB1.write(90);
      servoB3.write(i);
      servoKN2.write(i);
      servoB2.write(180 - i);
      servoB4.write(90);

      delay(50);
    }
    for (int i = 110; i >= 70; i--) {
      servoB1.write(90);
      servoB3.write(i);
      servoKN2.write(i);
      servoB2.write(180 - i);
      servoB4.write(90);

      delay(50);
    }
    delay(40);
  }
}

/************************left move***************/
void Leftmove() {
  for (int step = 0; step < 5; step++) {
    //---------------- STEP 1 ----------------//
    for (int i = 110; i >= 70; i--) {
      servoB3.write(90);
      servoB1.write(i);
      servoKN4.write(i);
      servoB4.write(180 - i);
      servoB2.write(90);

      delay(80);
    }
    delay(40);

    for (int i = 70; i <= 110; i++) {
      servoB3.write(90);
      servoB1.write(i);
      servoKN1.write(i);
      servoB4.write(180 - i);
      servoB2.write(90);

      delay(80);
    }
    delay(40);
  }
}
/*******************hand shake*****************/
void Handshake() {

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

  for (int i = 0; i < 20; i++) {

    servoB1.write(70);
    delay(800);

    servoB1.write(80);
    delay(800);

    servoKN2.write(40);
    delay(200);

    servoKN2.write(70);
    delay(200);
  }
  delay(500);
}

/***********************Setup************************/
void setup() {
  Serial.begin(115200);

  servoH1.attach(servoH1Pin);
  servoH2.attach(servoH2Pin);
  servoH3.attach(servoH3Pin);
  servoH4.attach(servoH4Pin);

  servoKN1.attach(servoKN1Pin);
  servoKN2.attach(servoKN2Pin);
  servoKN3.attach(servoKN3Pin);
  servoKN4.attach(servoKN4Pin);

  servoB1.attach(servoB1pin);
  servoB2.attach(servoB2pin);
  servoB3.attach(servoB3pin);
  servoB4.attach(servoB4pin);

  Serial.println("Quadruped Robot Started");
}

void loop() {
  delay(20000);

  Sit();
  delay(10000);

  Stand();
  delay(10000);

  Set();
  delay(10000);

  Handshake();
  delay(10000);

  Set();
  delay(10000);

  walkForward();
  delay(10000);

  Set();
  delay(10000);

  walkBackward();
  delay(10000);

  Set();
  delay(10000);

  Leftmove();
  delay(10000);

  Set();
  delay(10000);

  Rightmove();
  delay(10000);

  Set();
  delay(10000);
}