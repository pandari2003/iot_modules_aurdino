#include <Servo.h>

Servo servoBa1;
Servo servoBa2;
Servo servoBa3;
Servo servoBa4;

Servo servoHa1;
Servo servoHa2;
Servo servoHa3;
Servo servoHa4;

Servo servoKNa1;
Servo servoKNa2;
Servo servoKNa3;
Servo servoKNa4;

const int servoBa1pin = 2;   //front right
const int servoBa2pin = 5;   //front left
const int servoBa3pin = 8;   //back right
const int servoBa4pin = 11;  //back left

const int servoHa1Pin = 3;   // Front Right
const int servoHa2Pin = 6;   // front left
const int servoHa3Pin = 9;   // back right
const int servoHa4Pin = 12;  // back Left

const int servoKNa1Pin = 4;   // Front Right
const int servoKNa2Pin = 7;   // front left
const int servoKNa3Pin = 10;  // back right
const int servoKNa4Pin = 13;  // back Left

void Set() {

  // Body servos
  servoBa1.write(90);
  servoBa3.write(90);
  servoBa2.write(90);
  servoBa4.write(90);

  // Hip servos
  servoHa3.write(90);
  servoHa1.write(90);
  servoHa4.write(90);
  servoHa2.write(90);

  // Knee servos
  servoKNa3.write(90);
  servoKNa1.write(90);
  servoKNa4.write(90);
  servoKNa2.write(90);

  delay(500);
}


void Sit() {

  for (int i = 90; i >= 45; i--) {

    // Left pair
    servoHa1.write(i);
    servoHa4.write(i);
    servoKNa1.write(i - 45);
    servoKNa4.write(i - 45);

    // Right pair
    servoHa3.write(180 - i);
    servoHa2.write(180 - i);
    servoKNa3.write(230 - i);
    servoKNa2.write(230 - i);

    delay(80);
  }
}

void Stand() {

  for (int i = 45; i <= 90; i++) {

    // Left pair
    servoHa1.write(i);
    servoHa4.write(i);

    servoKNa1.write(i - 45);  // 0 -> 45
    servoKNa4.write(i - 45);

    // Right pair
    servoHa3.write(180 - i);  // 135 -> 90
    servoHa2.write(180 - i);

    servoKNa3.write(230 - i);  // 180 -> 140
    servoKNa2.write(230 - i);

    delay(80);
  }

  Set();
}

void walkForward() {

  for (int stp = 0; stp < 20; stp++) {

    //===========================
    // Pair 1 : Leg1 & Leg4
    //===========================

    for (int i = 0; i <= 20; i++) {

      // Lift Pair 1
      servoKNa1.write(90 - i);
      servoKNa4.write(90 - i);

      // Reverse hip swing
      servoHa1.write(90 + i);
      servoHa2.write(90 + i);

      servoHa3.write(90 - i);
      servoHa4.write(90 - i);

      delay(35);
    }

    // Pair 1 touches ground and pushes body backward
    for (int i = 0; i <= 20; i++) {

      servoKNa1.write(70 + i);
      servoKNa4.write(70 + i);

      servoHa1.write(110 - i);
      servoHa2.write(110 - i);

      servoHa3.write(70 + i);
      servoHa4.write(70 + i);

      delay(35);
    }

    //===========================
    // Pair 2 : Leg2 & Leg3
    //===========================

    for (int i = 0; i <= 20; i++) {

      // Lift Pair 2
      servoKNa2.write(90 - i);
      servoKNa3.write(90 - i);

      // Reverse hip swing
      servoHa3.write(90 + i);
      servoHa4.write(90 + i);

      servoHa1.write(90 - i);
      servoHa2.write(90 - i);

      delay(35);
    }

    // Pair 2 touches ground and pushes body backward
    for (int i = 0; i <= 20; i++) {

      servoKNa2.write(70 + i);
      servoKNa3.write(70 + i);

      servoHa3.write(110 - i);
      servoHa4.write(110 - i);

      servoHa1.write(70 + i);
      servoHa2.write(70 + i);

      delay(35);
    }
  }
}


void walkBackward() {

  for (int stp = 0; stp < 20; stp++) {

    //===========================
    // Pair 1 : Leg1 & Leg4
    //===========================

    for (int i = 0; i <= 20; i++) {

      // Lift Pair 1
      servoKNa1.write(90 - i);
      servoKNa4.write(90 - i);

      // Swing Pair 1 backward
      servoHa1.write(90 - i);
      servoHa2.write(90 - i);

      // Pair 2 pushes forward
      servoHa3.write(90 + i);
      servoHa4.write(90 + i);

      delay(35);
    }

    // Lower Pair 1
    for (int i = 20; i >= 0; i--) {

      servoKNa1.write(90 - i);
      servoKNa4.write(90 - i);

      servoHa1.write(70 + (20 - i));
      servoHa2.write(70 + (20 - i));

      servoHa3.write(110 - (20 - i));
      servoHa4.write(110 - (20 - i));

      delay(35);
    }

    //===========================
    // Pair 2 : Leg2 & Leg3
    //===========================

    for (int i = 0; i <= 20; i++) {

      // Lift Pair 2
      servoKNa2.write(90 - i);
      servoKNa3.write(90 - i);

      // Swing Pair 2 backward
      servoHa3.write(90 - i);
      servoHa4.write(90 - i);

      // Pair 1 pushes forward
      servoHa1.write(90 + i);
      servoHa2.write(90 + i);

      delay(35);
    }

    // Lower Pair 2
    for (int i = 20; i >= 0; i--) {

      servoKNa2.write(90 - i);
      servoKNa3.write(90 - i);

      servoHa3.write(70 + (20 - i));
      servoHa4.write(70 + (20 - i));

      servoHa1.write(110 - (20 - i));
      servoHa2.write(110 - (20 - i));

      delay(35);
    }
  }
}


/*****************right move**********************/
void Rightmove() {
  for (int step = 0; step < 5; step++) {
    //
    //---------------- STEP 1 ----------------//
    for (int i = 70; i <= 110; i++) {
      //
      servoBa1.write(90);
      servoBa2.write(i);
      servoKNa1.write(i);
      servoBa3.write(180 - i);
      servoBa4.write(90);

      delay(50);
    }
    for (int i = 110; i >= 70; i--) {
      //
      servoBa1.write(90);
      servoBa2.write(i);
      servoKNa1.write(i);
      servoBa3.write(180 - i);
      servoBa4.write(90);

      delay(50);
    }
    delay(40);
  }
}

/************************left move***************/
void Leftmove() {
  for (int step = 0; step < 5; step++) {
    //
    //---------------- STEP 1 ----------------//
    for (int i = 110; i >= 70; i--) {
      //
      servoBa2.write(90);
      servoBa1.write(i);
      servoKNa2.write(i);
      servoBa4.write(180 - i);
      servoBa3.write(90);

      delay(80);
    }
    delay(40);

    for (int i = 70; i <= 110; i++) {
      //
      servoBa2.write(90);
      servoBa1.write(i);
      servoKNa3.write(i);
      servoBa4.write(180 - i);
      servoBa3.write(90);

      delay(80);
    }
    delay(40);
  }
}
/*******************hand shake*****************/
void Handshake() {

  servoBa3.write(90);
  servoBa2.write(90);
  servoBa4.write(90);

  servoHa3.write(90);
  servoHa4.write(90);
  servoHa2.write(90);

  servoKNa3.write(90);
  servoKNa4.write(90);
  servoKNa2.write(90);

  for (int i = 0; i < 2; i++) {
    //
    servoHa1.write(150);
    servoBa1.write(85);
    delay(800);

    servoBa1.write(95);
    delay(800);

    servoKNa1.write(40);
    delay(200);

    servoKNa1.write(70);
    delay(200);
    servoHa1.write(90);
  }
  delay(500);
  Set();
}


void setup() {
  Serial.begin(115200);

  servoHa3.attach(servoHa3Pin);
  servoHa1.attach(servoHa1Pin);
  servoHa4.attach(servoHa4Pin);
  servoHa2.attach(servoHa2Pin);

  servoKNa3.attach(servoKNa3Pin);
  servoKNa1.attach(servoKNa1Pin);
  servoKNa4.attach(servoKNa4Pin);
  servoKNa2.attach(servoKNa2Pin);

  servoBa1.attach(servoBa1pin);
  servoBa3.attach(servoBa3pin);
  servoBa2.attach(servoBa2pin);
  servoBa4.attach(servoBa4pin);

  Serial.println("Quadruped Robot Started");
}


void loop() {

  Set();
  delay(10000);

  Sit();
  delay(10000);

  Stand();
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

  Handshake();
  delay(10000);

  Set();
  delay(10000);
}
