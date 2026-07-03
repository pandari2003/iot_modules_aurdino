// PIR Motion Sensor with LED
// Arduino Uno

const int pirPin = 11;    // PIR output pin
const int ledPin = 13;    // LED pin

void setup()
{
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);

  digitalWrite(ledPin, LOW);

  Serial.begin(9600);
  Serial.println("PIR Motion Sensor Ready");

  // Give the PIR sensor time to stabilize
  delay(3000);   // 30 seconds
}

void loop()
{
  int motion = digitalRead(pirPin);
  delay(100);

  if (motion == HIGH)
  {
    digitalWrite(ledPin, HIGH);
    Serial.println("Motion Detected!");
  }
  else
  {
    digitalWrite(ledPin, LOW);
    Serial.println("No Motion");
  }

  delay(1000);
}