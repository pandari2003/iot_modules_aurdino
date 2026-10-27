#define led 15
#define trigPin 12
#define echoPin 13

const int maxDistance = 100; 
const int minDistance = 2;

void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  int distanceCm = duration * 0.034 / 2;
  
  int distanceScale = map(distanceCm, minDistance, maxDistance, 0, 1000);
  distanceScale = constrain(distanceScale, 0, 1000);
  
  int ledBrightness = map(distanceScale, 0, 1000, 255, 0); 
  analogWrite(led, ledBrightness);
  
  Serial.print("Distance Scale (0-1000): ");
  Serial.println(distanceScale);
  
  delay(6); 
}
