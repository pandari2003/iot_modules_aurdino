#include <Servo.h>

Servo myServo;

void setup() {
  myServo.attach(9);    // Servo signal connected to Digital Pin 9
}

void loop() {
  myServo.write(0);     // Move to 0°
  delay(500);

  myServo.write(90);    // Move to 90°
  delay(500);
} 