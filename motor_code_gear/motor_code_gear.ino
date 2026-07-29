const int motorPin = 23; 

void setup() {
  pinMode(motorPin, OUTPUT);
  ledcAttach(motorPin, 5000, 8);            // 5kHz frequency, 8-bit resolution (0-255)
}

void loop() {
  ledcWrite(motorPin, 255); // Full Speed
  delay(3000);
  
  ledcWrite(motorPin, 150); // Medium Speed
  delay(3000);
  
  ledcWrite(motorPin, 0);   // Stop
  delay(2000);
}
