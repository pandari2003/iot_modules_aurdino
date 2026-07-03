int flamePin = 10;
int buzzer = 9;

void setup() {
  pinMode(flamePin, INPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int flame = digitalRead(flamePin);

  if (flame == LOW) {      // Flame detected
    digitalWrite(buzzer, 0);
    Serial.println("Flame Detected!");
  } 
  else {
    digitalWrite(buzzer, 1);
    Serial.println("No Flame");
  }

  delay(500);
}