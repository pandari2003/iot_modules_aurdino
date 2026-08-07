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
  for (int angle = 0; angle <= 55; angle++)
  {
    myServo.write(angle);
    delay(500);
  }

  for (int angle = 55; angle >= 0; angle--)
  {//jjjjjj
    myServo.write(angle);
    delay(500);
  }
}