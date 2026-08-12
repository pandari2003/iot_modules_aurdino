#include <ESP32Servo.h>

Servo myServo;

#define SERVO_PIN 13

void setup()
{
  myServo.setPeriodHertz(50);
  myServo.attach(SERVO_PIN, 500, 2400);
}

void loop()
{
  for (int angle = 0; angle <= 57; angle++)
  {
    myServo.write(angle);
    delay(300);
  }

  for (int angle = 57; angle >= 0; angle--)
  {
    myServo.write(angle);
    delay(300);
  }
}