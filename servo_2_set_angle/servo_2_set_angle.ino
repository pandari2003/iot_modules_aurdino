#include <Servo.h>

Servo servo1;          
Servo servo2;          

const int servo1Pin = 9;  // First servo signal wire to Digital Pin 9
const int servo2Pin = 10; // Second servo signal wire to Digital Pin 10

void setup() {
  Serial.begin(9600);       
  
  servo1.attach(servo1Pin); 
  servo2.attach(servo2Pin); 
  
  // Start both at 90 degrees initially
  servo1.write(90);
  servo2.write(90);
  
  Serial.println("=== Dual Conditional Servo Control Ready ===");
  Serial.println("Enter a single angle (0-180) and press Enter:");
}

void loop() {
  // Only proceed if numerical data is waiting in the serial buffer
  if (Serial.available() > 0) {
    
    // Check if the next character is a valid digit
    if (isDigit(Serial.peek()) || Serial.peek() == '-') {
      
      int targetAngle = Serial.parseInt(); 
      
      // Validate that the entered angle is within standard 0-180 limits
      if (targetAngle >= 0 && targetAngle <= 180) {
        
        int s1Angle;
        int s2Angle;

        // Check the conditional layout requested by the user
        if (targetAngle == 90) {
          s1Angle = 90;
          s2Angle = 90;
        } else {
          // If 0 is typed: Servo 1 = 0, Servo 2 = (180 - 0) = 180
          // Works for all other angles too (e.g., if 45 is typed, Servo 2 = 135)
          s1Angle = targetAngle;
          s2Angle = targetAngle; 
        }

        // Display the synchronized results cleanly on screen
        Serial.print("SUCCESS -> Input: [ ");
        Serial.print(targetAngle);
        Serial.print("° ] | Servo 1: ");
        Serial.print(s1Angle);
        Serial.print("° | Servo 2: ");
        Serial.print(s2Angle);
        Serial.println("°");
        
        // Execute the movements simultaneously
        servo1.write(s1Angle); 
        servo2.write(s2Angle); 
        
      } else {
        Serial.print("ERROR: '");
        Serial.print(targetAngle);
        Serial.println("' is out of bounds! Enter a number between 0 and 180.");
      }
      
    } else {
      // Discard trailing spaces or line breaks safely
      Serial.read(); 
    }
  }
}
