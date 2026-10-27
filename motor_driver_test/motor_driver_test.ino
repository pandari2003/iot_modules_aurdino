// Define the pins connected to the DRV8833 IN1 and IN2
const int IN1 = 10;
const int IN2 = 9;

void setup() {
  // Initialize serial communication at 9600 baud rate
  Serial.begin(9600);
  
  // Set the motor control pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  
  // Ensure the motor starts in a stopped state
  motorStop();
  
  // Print instructions to the Serial Monitor
  Serial.println("DRV8833 Motor Control Ready!");
  Serial.println("Send: 'f' for Forward, 'b' for Backward, 's' for Stop");
}

void loop() {
  // Check if there is data available in the Serial window
  if (Serial.available() > 0) {
    char command = Serial.read(); // Read the incoming character
    
    switch (command) {
      
      case 'f':
      case 'F':
        motorForward();
        break;
        
      case 'b':
      case 'B':
        motorBackward();
        break;
        
      case 's':
      case 'S':
        motorStop();
        break;
        
      // Ignore newline or carriage return characters
      case '\n':
      case '\r':
        break;
        
      default:
        Serial.print("Unknown command: ");
        Serial.println(command);
        break;
    }
  }
}

// Function to move the motor forward
void motorForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  Serial.println("Motor moving FORWARD");
}

// Function to move the motor backward
void motorBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  Serial.println("Motor moving BACKWARD");
}

// Function to stop the motor
void motorStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  Serial.println("Motor STOPPED");
}
/*
// Define the GPIO pins connected to the DRV8833 IN1 and IN2
// Using standard GPIO pins on the ESP32 (Avoid TX0/RX0 or strapping pins)
const int IN1 = 17; 
const int IN2 = 16;

void setup() {
  // Initialize serial communication at 115200 baud rate (Standard for ESP32)
  Serial.begin(115200);
  
  // Wait for the serial port to connect (Useful for native USB ESP32 boards)
  while (!Serial) {
    delay(10); 
  }
  
  // Set the motor control pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  
  // Ensure the motor starts in a stopped state
  motorStop();
  
  // Print instructions to the Serial Monitor
  Serial.println("\nDRV8833 Motor Control Ready!");
  Serial.println("Send: 'f' for Forward, 'b' for Backward, 's' for Stop");
}

void loop() {
  // Check if there is data available in the Serial window
  if (Serial.available() > 0) {
    char command = Serial.read(); // Read the incoming character
    
    switch (command) {
      case 'f':
      case 'F':
        motorForward();
        break;
        
      case 'b':
      case 'B':
        motorBackward();
        break;
        
      case 's':
      case 'S':
        motorStop();
        break;
        
      // Ignore newline or carriage return characters
      case '\n':
      case '\r':
        break;
        
      default:
        Serial.print("Unknown command: ");
        Serial.println(command);
        break;
    }
  }
}

// Function to move the motor forward
void motorForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  Serial.println("Motor moving FORWARD");
}

// Function to move the motor backward
void motorBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  Serial.println("Motor moving BACKWARD");
}

// Function to stop the motor
void motorStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  Serial.println("Motor STOPPED");
}
*/