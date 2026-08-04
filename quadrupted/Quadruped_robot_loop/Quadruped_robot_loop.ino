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

// ADJUSTED CONFIGURATION FOR SMALLER ANGLES AND SMOOTHNESS
const int stepDelay = 15;     // Small delay per micro-step for smooth speed transitions
const int liftAngle = 25;     // REDUCED: Lower knee lift to prevent excessive leg shaking
const int swingAngle = 12;    // REDUCED: Shorter hip stride to keep movements small and balanced
const int microSteps = 60;    // Added loop resolution for ultra-smooth sub-degree calculations

/**************** SET ALL SERVOS TO 90° ****************/
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

/****************sit*****************/
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
  delay(60);
}

/******************stand robot********************/
void Stand() {
  for (int i = 45; i <= 90; i++) {
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

    delay(50);  
  }
  delay(50);
  Set();
}

/**************** REDUCED ANGLE & ULTRA-SMOOTH WALK FORWARD ******************/
void walkForward() {
  for (int step = 0; step < 10; step++) {
    
    //---------------- STEP 1: Micro-Lift Pair A & Micro-Swing Hips ----------------//
    for (int i = 0; i <= microSteps; i++) {
      int progressKN = map(i, 0, microSteps, 0, liftAngle);
      int progressHA = map(i, 0, microSteps, 0, swingAngle);
      
      // Smooth vertical lift (Front Right + Back Left)
      servoKNa1.write(90 - progressKN); 
      servoKNa4.write(90 - progressKN);
      
      // Smooth micro hip movement
      servoHa1.write(90 + progressHA); 
      servoHa4.write(90 - progressHA); 
      servoHa2.write(90 + progressHA); 
      servoHa3.write(90 - progressHA); 
      
      delay(stepDelay);
    }

    //---------------- STEP 2: Ground Pair A Smoothly ----------------//
    for (int i = microSteps; i >= 0; i--) {
      int progressKN = map(i, 0, microSteps, 0, liftAngle);
      servoKNa1.write(90 - progressKN);
      servoKNa4.write(90 - progressKN);
      delay(stepDelay);
    }

    //---------------- STEP 3: Micro-Lift Pair B & Micro-Swing Hips Opposite ----------------//
    for (int i = 0; i <= microSteps; i++) {
      int progressKN = map(i, 0, microSteps, 0, liftAngle);
      int progressHA = map(i, 0, microSteps, 0, swingAngle);
      
      // Smooth vertical lift (Front Left + Back Right)
      servoKNa2.write(90 - progressKN); 
      servoKNa3.write(90 - progressKN);
      
      // Smooth micro hip movement opposite direction
      servoHa2.write(90 - progressHA); 
      servoHa3.write(90 + progressHA); 
      servoHa1.write(90 - progressHA); 
      servoHa4.write(90 + progressHA); 
      
      delay(stepDelay);
    }

    //---------------- STEP 4: Ground Pair B Smoothly ----------------//
    for (int i = microSteps; i >= 0; i--) {
      int progressKN = map(i, 0, microSteps, 0, liftAngle);
      servoKNa2.write(90 - progressKN);
      servoKNa3.write(90 - progressKN);
      delay(stepDelay);
    }
  }
  delay(30);
}

/**************** REDUCED ANGLE & ULTRA-SMOOTH WALK BACKWARD ****************/
void walkBackward() {
  for (int step = 0; step < 10; step++) {
    
    //---------------- STEP 1: Micro-Lift Pair A & Micro-Swing Hips Backwards ----------------//
    for (int i = 0; i <= microSteps; i++) {
      int progressKN = map(i, 0, microSteps, 0, liftAngle);
      int progressHA = map(i, 0, microSteps, 0, swingAngle);
      
      servoKNa1.write(90 - progressKN); 
      servoKNa4.write(90 - progressKN);
      
      servoHa1.write(90 - progressHA); 
      servoHa4.write(90 + progressHA); 
      servoHa2.write(90 - progressHA); 
      servoHa3.write(90 + progressHA); 
      
      delay(stepDelay);
    }

    //---------------- STEP 2: Ground Pair A Smoothly ----------------//
    for (int i = microSteps; i >= 0; i--) {
      int progressKN = map(i, 0, microSteps, 0, liftAngle);
      servoKNa1.write(90 - progressKN);
      servoKNa4.write(90 - progressKN);
      delay(stepDelay);
    }

    //---------------- STEP 3: Micro-Lift Pair B & Micro-Swing Hips Opposite Backwards ----------------//
    for (int i = 0; i <= microSteps; i++) {
      int progressKN = map(i, 0, microSteps, 0, liftAngle);
      int progressHA = map(i, 0, microSteps, 0, swingAngle);
      
      servoKNa2.write(90 - progressKN); 
      servoKNa3.write(90 - progressKN);
      
      servoHa2.write(90 + progressHA); 
      servoHa3.write(90 - progressHA); 
      servoHa1.write(90 + progressHA); 
      servoHa4.write(90 - progressHA); 
      
      delay(stepDelay);
    }

    //---------------- STEP 4: Ground Pair B Smoothly ----------------//
    for (int i = microSteps; i >= 0; i--) {
      int progressKN = map(i, 0, microSteps, 0, liftAngle);
      servoKNa2.write(90 - progressKN);
      servoKNa3.write(90 - progressKN);
      delay(stepDelay);
    }
  }
  delay(30);
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
  servoBa3.write(90);   servoBa2.write(90);   servoBa4.write(90);
  servoHa3.write(90);   servoHa4.write(90);   servoHa2.write(90);
  servoKNa3.write(90);  servoKNa4.write(90);  servoKNa2.write(90);

  for (int i = 0; i < 2; i++) {
    servoHa1.write(150); // Reduced sweep angle from 150
    servoBa1.write(85);
    delay(600);

    servoBa1.write(95);
    delay(600);

    servoKNa1.write(40); // Reduced knee sweep angle
    delay(200);

    servoKNa1.write(70);
    delay(200);
    servoHa1.write(90);
  }
  delay(500);
  Set();
}

/***********************Setup**************************/
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

  Set(); 
  Serial.println("Quadruped Robot Started");
}

/***********************Loop **************************/
void loop() {
  delay(500);

  Sit();
  delay(500);

  Stand();
  delay(500);

  Set();
  delay(500);

  Handshake();
  delay(500);

  Set();
  delay(500);

  walkForward();
  delay(500);

  Set();
  delay(500);

  walkBackward();
  delay(500);

  Set();
  delay(500);

  Leftmove();
  delay(500);

  Set();
  delay(500);

  Rightmove();
  delay(500);

  Set();
  delay(500);
}
 