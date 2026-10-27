#include <Servo.h>

Servo servoAY1;
Servo servoBX1;
Servo servoP1;

Servo servoCY2;
Servo servoDX2;
Servo servoP2;

// Pins
const int joystickABX = A0;
const int joystickABY = A1;
const int potPin1 = A2;

const int joystickCDX = A3;
const int joystickCDY = A4;
const int potPin2 = A5;

// Servo pins
const int servoAY1Pin = 9;
const int servoBX1Pin = 10;
const int servoP1Pin = 11;

const int servoCY2Pin = 5;
const int servoDX2Pin = 6;
const int servoP2Pin = 7;

// Control button
const int controlButton = 2;

// Current servo angles
int ABxAngle = 90;
int AByAngle = 90;

int CDxAngle = 90;
int CDyAngle = 90;

// Joystick settings
const int center = 512;
const int deadZone = 50;

// -------- JOYSTICK FUNCTION --------
void controlJoystick(int pin, int &angle) {

  int value = analogRead(pin);

  if (value > center + deadZone) {
    angle++;
  }
  else if (value < center - deadZone) {
    angle--;
  }

  angle = constrain(angle, 0, 180);
}


void setup() {

  servoAY1.attach(servoAY1Pin);
  servoBX1.attach(servoBX1Pin);
  servoP1.attach(servoP1Pin);

  servoCY2.attach(servoCY2Pin);
  servoDX2.attach(servoDX2Pin);
  servoP2.attach(servoP2Pin);


  // Start servos at 90 degrees
  servoAY1.write(90);
  servoBX1.write(90);
  servoP1.write(90);

  servoCY2.write(90);
  servoDX2.write(90);
  servoP2.write(90);

  // Button
  pinMode(controlButton, INPUT_PULLUP);

  // Serial Monitor
  Serial.begin(115200);
}

void loop() {

  // Read button
  int buttonState = digitalRead(controlButton);

  // BUTTON ON

  if (buttonState == LOW) {

    // Joystick control
    controlJoystick(joystickABX, ABxAngle);
    controlJoystick(joystickABY, AByAngle);

    controlJoystick(joystickCDX, CDxAngle);
    controlJoystick(joystickCDY, CDyAngle);


    // Move joystick servos
    servoAY1.write(ABxAngle);
    servoBX1.write(AByAngle);

    servoCY2.write(CDxAngle);
    servoDX2.write(CDyAngle);


    // Potentiometer 1
    int pot1Value = analogRead(potPin1);
    int pot1Angle = map(pot1Value, 0, 1023, 0, 180);

    servoP1.write(pot1Angle);

/************************************************/
    // Potentiometer 2
    int pot2Value = analogRead(potPin2);
    int pot2Angle = map(pot2Value, 0, 1023, 0, 180);

    servoP2.write(pot2Angle);

/***********************************************/
    // Serial Monitor
    Serial.print("AB X: ");
    Serial.print(ABxAngle);

    Serial.print("  AB Y: ");
    Serial.print(AByAngle);

    Serial.print("  Pot1: ");
    Serial.print(pot1Angle);

    Serial.print("  |  CD X: ");
    Serial.print(CDxAngle);

    Serial.print("  CD Y: ");
    Serial.print(CDyAngle);

    Serial.print("  Pot2: ");
    Serial.println(pot2Angle);

  }

  // BUTTON OFF

  else {

    Serial.println("on button");

  }

  delay(30);
}