/*
  ARDUINO MEGA + PCA9685 + DRV8833

  DRV8833
  AIN1    D6
  AIN2    D7
  BIN1    D8
  BIN2    D9

  AOUT1   Motor 1
  AOUT2   Motor 1
  BOUT1   Motor 2
  BOUT2   Motor 2
  GND     Arduino GND
  VM      Motor battery +

  ------------------------------------------------

  PCA9685 SERVO DRIVER

  Arduino Mega:
  SDA -> D20
  SCL -> D21
  VCC -> 5V
  GND -> GND
  V+  -> External 5-6V servo power

  Servo             PCA9685 Channel

  servoAY11         CH0
  servoAY1          CH1
  servoBX1          CH2
  servoP1           CH3
  servoCY2          CH4
  servoDX2          CH5
  servoP2           CH6

  ------------------------------------------------

  Button
  One side -> D2
  Other side -> GND

  Indicator LED
  LED -> D3
  LED resistor -> GND

  ------------------------------------------------

  Joystick 1

  AB VRx -> A0
  AB VRy -> A1
  AB VCC -> 5V
  AB GND -> GND

  ------------------------------------------------

  Joystick 2

  CD VRx -> A3
  CD VRy -> A4
  CD VCC -> 5V
  CD GND -> GND

  ------------------------------------------------

  Potentiometer 1

  Center -> A2
  Outer  -> 5V
  Other  -> GND

  ------------------------------------------------

  Potentiometer 2

  Center -> A5
  Outer  -> 5V
  Other  -> GND
*/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// PCA9685

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// PCA9685 servo pulse limits
// 50Hz servo frequency
const int SERVOMIN = 150;
const int SERVOMAX = 600;

// SERVO NAMES

int servoAY1;
int servoAY11;
int servoBX1;
int servoP1;

int servoCY2;
int servoDX2;
int servoP2;

// PCA9685 SERVO CHANNELS

const int servoAY11Channel = 0;
const int servoAY1Channel = 1;
const int servoBX1Channel = 2;
const int servoP1Channel = 3;

const int servoCY2Channel = 4;
const int servoDX2Channel = 5;
const int servoP2Channel = 6;

// JOYSTICK / POT PINS

const int joystickABX = A0;
const int joystickABY = A1;
const int potPin1 = A2;

const int joystickCDX = A3;
const int joystickCDY = A4;
const int potPin2 = A5;

// CONTROL BUTTON

const int controlButton = 2;

// INDICATOR

const int indicator = 3;

// DC MOTOR PINS

const int motor1_IN1 = 6;
const int motor1_IN2 = 7;

const int motor2_IN3 = 8;
const int motor2_IN4 = 9;

// CURRENT SERVO ANGLES

int ABxAngle = 90;
int AByAngle = 90;

int CDxAngle = 90;
int CDyAngle = 90;

// INITIAL DRIVING SPEED

int speedValue = 50;

// JOYSTICK SETTINGS

const int center = 512;
const int deadZone = 50;
const int servoMoveDelay = 30;

// SERVO ANGLE FUNCTION

void writeServo(int channel, int angle) {

  angle = constrain(angle, 0, 180);

  int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);

  pwm.setPWM(channel, 0, pulse);
}

// JOYSTICK FUNCTION

void controlJoystick(int pin, int &angle) {

  int value = analogRead(pin);

  if (value > center + deadZone) {
    angle++;
    angle = constrain(angle, 0, 180);

    delay(servoMoveDelay);
  }
  else if (value < center - deadZone) {
    angle--;
    angle = constrain(angle, 0, 180);

    delay(servoMoveDelay);
  }
}

// SETUP

void setup() {
  // PCA9685 START

  pwm.begin();

  pwm.setOscillatorFrequency(27000000);

  pwm.setPWMFreq(50);

  delay(10);

  // START ALL SERVOS AT 90 DEGREES

  servoAY1 = 90;
  servoAY11 = 90;
  servoBX1 = 90;
  servoP1 = 90;

  servoCY2 = 90;
  servoDX2 = 90;
  servoP2 = 90;

  writeServo(servoAY1Channel, servoAY1);
  writeServo(servoAY11Channel, servoAY11);
  writeServo(servoBX1Channel, servoBX1);

  writeServo(servoP1Channel, servoP1);

  writeServo(servoCY2Channel, servoCY2);
  writeServo(servoDX2Channel, servoDX2);

  writeServo(servoP2Channel, servoP2);

  // BUTTON

  pinMode(controlButton, INPUT_PULLUP);

  // INDICATOR

  pinMode(indicator, OUTPUT);

  digitalWrite(indicator, LOW);

  // DC MOTOR SETUP

  pinMode(motor1_IN1, OUTPUT);
  pinMode(motor1_IN2, OUTPUT);

  pinMode(motor2_IN3, OUTPUT);
  pinMode(motor2_IN4, OUTPUT);

  // ENSURE MOTORS START OFF

  digitalWrite(motor1_IN1, LOW);
  digitalWrite(motor1_IN2, LOW);

  digitalWrite(motor2_IN3, LOW);
  digitalWrite(motor2_IN4, LOW);

  // SERIAL MONITOR

  Serial.begin(115200);
}

// MAIN LOOP

void loop() {
  // READ BUTTON

  int buttonState = digitalRead(controlButton);

  // BUTTON ON = SERVO / WORKING MODE

  if (buttonState == LOW) {

    // Indicator ON
    digitalWrite(indicator, HIGH);

    // STOP DC MOTORS IMMEDIATELY

    digitalWrite(motor1_IN1, LOW);
    digitalWrite(motor1_IN2, LOW);

    digitalWrite(motor2_IN3, LOW);
    digitalWrite(motor2_IN4, LOW);


    // JOYSTICK CONTROL

    controlJoystick(joystickABX, ABxAngle);
    controlJoystick(joystickABY, AByAngle);

    controlJoystick(joystickCDX, CDxAngle);
    controlJoystick(joystickCDY, CDyAngle);


    // MOVE JOYSTICK SERVOS

// MOVE JOYSTICK SERVOS
int ayAngle = map(ABxAngle, 0, 180, 0, 140);
ayAngle = constrain(ayAngle, 0, 140);

servoAY1 = ayAngle;
servoAY11 = 140 - ayAngle;

// AB Y remains 0-180 degrees
servoBX1 = AByAngle;

servoCY2 = CDxAngle;
servoDX2 = CDyAngle;

    writeServo(servoAY1Channel, servoAY1);
    writeServo(servoAY11Channel, servoAY11);
    writeServo(servoBX1Channel, servoBX1);

    writeServo(servoCY2Channel, servoCY2);
    writeServo(servoDX2Channel, servoDX2);

    // POTENTIOMETER 1

    int pot1Value = analogRead(potPin1);

    int pot1Angle = map(
      pot1Value,
      0,
      1023,
      0,
      180);

if (servoP1 < pot1Angle) {
  servoP1++;
  writeServo(servoP1Channel, servoP1);
  delay(servoMoveDelay);
}
else if (servoP1 > pot1Angle) {
  servoP1--;
  writeServo(servoP1Channel, servoP1);
  delay(servoMoveDelay);
}

    // POTENTIOMETER 2

    int pot2Value = analogRead(potPin2);

    int pot2Angle = map(
      pot2Value,
      0,
      1023,
      0,
      180);

if (servoP2 < pot2Angle) {
  servoP2++;
  writeServo(servoP2Channel, servoP2);
  delay(servoMoveDelay);
}
else if (servoP2 > pot2Angle) {
  servoP2--;
  writeServo(servoP2Channel, servoP2);
  delay(servoMoveDelay);
}

    // SERIAL MONITOR

    Serial.print("SERVO MODE | ");

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

  // BUTTON OFF = DRIVING MODE

  else {

    // Indicator OFF
    digitalWrite(indicator, LOW);

    Serial.print("DRIVING MODE | ");


    // READ JOYSTICKS

    // AB joystick X = SPEED

    int abXValue = analogRead(joystickABX);

    // CD joystick X = FORWARD / BACKWARD

    int cdXValue = analogRead(joystickCDX);

    // CD joystick Y = LEFT / RIGHT ROTATION

    int cdYValue = analogRead(joystickCDY);

    // SPEED CONTROL

    // Initial value = 50%

    if (abXValue > center + deadZone) {

      speedValue++;
    }

    else if (abXValue < center - deadZone) {

      speedValue--;
    }

    // Limit speed 0 to 100%

    speedValue = constrain(
      speedValue,
      0,
      100);

    // Convert percentage to PWM

    int baseSpeed = map(
      speedValue,
      0,
      100,
      0,
      255);
    // MOTOR SPEED VARIABLES

    int leftSpeed = baseSpeed;
    int rightSpeed = baseSpeed;
    // FORWARD
    // CD X+

    if (cdXValue > center + deadZone) {

      // Both motors same speed

      leftSpeed = baseSpeed;
      rightSpeed = baseSpeed;

      // LEFT MOTOR FORWARD

      analogWrite(
        motor1_IN1,
        leftSpeed);

      digitalWrite(
        motor1_IN2,
        LOW);

      // RIGHT MOTOR FORWARD

      analogWrite(
        motor2_IN3,
        rightSpeed);

      digitalWrite(
        motor2_IN4,
        LOW);
      Serial.print("FORWARD ");
    }

    // BACKWARD
    // CD X-

    else if (cdXValue < center - deadZone) {

      // Both motors same speed

      leftSpeed = baseSpeed;
      rightSpeed = baseSpeed;

      // LEFT MOTOR BACKWARD

      digitalWrite(
        motor1_IN1,
        LOW);

      analogWrite(
        motor1_IN2,
        leftSpeed);

      // RIGHT MOTOR BACKWARD
      digitalWrite(
        motor2_IN3,
        LOW);

      analogWrite(
        motor2_IN4,
        rightSpeed);
      Serial.print("BACKWARD ");
    }
    // CD X CENTER
    // CHECK ROTATION

    else {
      // RIGHT / CLOCKWISE ROTATION
      // CD Y+

      if (cdYValue > center + deadZone) {

        // Both motors same speed
        // Opposite directions

        leftSpeed = baseSpeed;
        rightSpeed = baseSpeed;

        // LEFT MOTOR FORWARD

        analogWrite(
          motor1_IN1,
          leftSpeed);

        digitalWrite(
          motor1_IN2,
          LOW);

        // RIGHT MOTOR REVERSE

        digitalWrite(
          motor2_IN3,
          LOW);

        analogWrite(
          motor2_IN4,
          rightSpeed);

        Serial.print("CLOCKWISE ");
      }
      // LEFT / ANTI-CLOCKWISE ROTATION
      // CD Y-

      else if (cdYValue < center - deadZone) {
        // Both motors same speed
        // Opposite directions

        leftSpeed = baseSpeed;
        rightSpeed = baseSpeed;

        // LEFT MOTOR REVERSE

        digitalWrite(
          motor1_IN1,
          LOW);

        analogWrite(
          motor1_IN2,
          leftSpeed);

        // RIGHT MOTOR FORWARD
        analogWrite(
          motor2_IN3,
          rightSpeed);

        digitalWrite(
          motor2_IN4,
          LOW);
        Serial.print("ANTI-CLOCKWISE ");
      }
      // CD Y CENTER = STOP

      else {

        digitalWrite(
          motor1_IN1,
          LOW);

        digitalWrite(
          motor1_IN2,
          LOW);

        digitalWrite(
          motor2_IN3,
          LOW);

        digitalWrite(
          motor2_IN4,
          LOW);
        Serial.print("STOP ");
      }
    }
    // SERIAL MONITOR

    Serial.print("Speed: ");
    Serial.print(speedValue);
    Serial.print("%");

    Serial.print("  Base PWM: ");
    Serial.print(baseSpeed);

    Serial.print("  Left PWM: ");
    Serial.print(leftSpeed);

    Serial.print("  Right PWM: ");
    Serial.println(rightSpeed);
  }
}