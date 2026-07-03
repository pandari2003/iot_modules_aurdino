//Includes the Arduino Stepper Library
#include <Stepper.h>

// Defines the number of steps per rotation
const int stepsPerRevolution = 2048;

// Creates an instance of stepper class
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);

void setup() {
  // Nothing to do (Stepper Library sets pins as outputs)
}

void loop() {
  // Rotate CW slowly at 5 RPM
  myStepper.setSpeed(5);
  myStepper.step(stepsPerRevolution);
  delay(10);

  // Rotate CCW quickly at 10 RPM
  myStepper.setSpeed(5);
  myStepper.step(-stepsPerRevolution);
  delay(10);
}