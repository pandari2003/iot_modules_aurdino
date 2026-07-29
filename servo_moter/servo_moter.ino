#include <Servo.h>

Servo myServo;
int pos = 0;

void setup() {
  myServo.attach(9);
}

void loop() {
  for (pos = 10; pos <= 30; pos += 1) { 
    myServo.write(pos);              
    delay(50);
  }
  
  delay(1000);
  
  for (pos = 30; pos >= 10; pos -= 1) { 
    myServo.write(pos);              
    delay(50);
  }
  
  delay(1000);
}
