// HC-SR04 Ultrasonic Sensor with Arduino Uno

const int trigPin = 8;
const int echoPin = 9;

long duration;
float distance;

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  digitalWrite(trigPin, LOW);
}

void loop() {

  // Clear trigger pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Send 10 us pulse
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read echo
  duration = pulseIn(echoPin, HIGH, 30000);
    distance = duration * 0.0343 / 2.0;
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  delay(1000);
}