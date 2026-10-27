#include <Servo.h>

Servo myServo;          
const int servoPin = 13; 

void setup() {
  Serial.begin(9600);       
  myServo.attach(servoPin); 
  
  myServo.write(90); // Initial position
  
  Serial.println("=== Arduino Uno MG995 Control ===");
  Serial.println("Enter angle (0-190):");
}

void loop() {
  // Only proceed if there is actual numerical data waiting
  if (Serial.available() > 0) {
    
    // Check if the next character in buffer is actually a number
    // This stops Serial.parseInt() from reading random noise or blank spaces
    if (isDigit(Serial.peek()) || Serial.peek() == '-') {
      
      int angle = Serial.parseInt(); 
      
      if (angle >= 0 && angle <= 190) {
        Serial.print("SUCCESS: Set position to -> [ ");
        Serial.print(angle); 
        Serial.println(" ]");
        
        myServo.write(angle);
      } else {
        Serial.print("ERROR: Out of bounds -> [ ");
        Serial.print(angle);
        Serial.println(" ]");
      }
    } else {
      // Discard non-numeric characters (like newline '\n' or carriage return '\r')
      Serial.read(); 
    }
  }
}
