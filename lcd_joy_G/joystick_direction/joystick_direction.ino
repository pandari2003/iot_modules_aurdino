int joyX = A0;
int joyY = A1;

int upLED = 1;
int downLED = 3;
int leftLED = 4;
int rightLED = 5;

void setup() {
  pinMode(upLED, OUTPUT);
  pinMode(downLED, OUTPUT);
  pinMode(leftLED, OUTPUT);
  pinMode(rightLED, OUTPUT);
}

void loop() {

  int x = analogRead(joyX);
  int y = analogRead(joyY);

  // Turn all LEDs OFF
  digitalWrite(upLED, LOW);
  digitalWrite(downLED, LOW);
  digitalWrite(leftLED, LOW);
  digitalWrite(rightLED, LOW);

  if (y > 700) {
    digitalWrite(upLED, HIGH);      // Joystick UP
  }
  else if (y < 300) {
    digitalWrite(downLED, HIGH);    // Joystick DOWN
  }

  if (x > 700) {
    digitalWrite(rightLED, HIGH);   // Joystick RIGHT
  }
  else if (x < 300) {
    digitalWrite(leftLED, HIGH);    // Joystick LEFT
  }

  delay(100);
}