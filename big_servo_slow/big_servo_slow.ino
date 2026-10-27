#include <ESP32Servo.h>

Servo myServo;

#define SERVO_PIN 13

void setup()
{               
  Serial.begin(115200); 
  myServo.setPeriodHertz(60);
  myServo.attach(SERVO_PIN, 500, 2400);
}

void loop()
{
  for (int angle = 0; angle <= 50; angle++) 
  {                                                                                                                       
    myServo.write(angle);
    delay(30);
  }

  for (int angle = 50; angle >= 0; angle--)
  {
    myServo.write(angle);
    delay(30);
  }
}
