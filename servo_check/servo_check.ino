#include <Servo.h>

Servo myServo;  // Create servo object to control a servo

void setup() {
  myServo.attach(9);  // Attaches the servo on pin 9 to the servo object
}

void loop() {
  // Sweep from 0 to 180 degrees
  for (int pos = 0; pos <= 180; pos += 1) {
    myServo.write(pos);
    delay(15);  // Waits 15ms for the servo to reach the position
  }
  
  // Sweep back from 180 to 0 degrees
  for (int pos = 180; pos >= 0; pos -= 1) {
    myServo.write(pos);
    delay(15);
  }
}
