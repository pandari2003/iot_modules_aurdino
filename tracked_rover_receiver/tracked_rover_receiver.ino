/*
Servos
Servo AY1 signal → D2
Servo AY11 signal → A4
Servo BX1 signal → D4
Servo P1  signal → D6

Servo CY2 signal → D7
Servo DX2 signal → D8
Servo P2  signal → A2
E01-ML01DP5
E01 CE   → D9
E01 CSN  → D10
E01 MOSI → D11
E01 MISO → D12
E01 SCK  → D13

E01 VCC  → 3.3V
E01 GND  → GND
DRV8833
DRV8833 AIN1 → D3       ← Motor 1 PWM
DRV8833 AIN2 → A0       ← Motor 1 direction

DRV8833 BIN1 → D5       ← Motor 2 PWM
DRV8833 BIN2 → A1       ← Motor 2 direction

DRV8833 AOUT1/AOUT2 → Motor 1
DRV8833 BOUT1/BOUT2 → Motor 2

DRV8833 GND → Arduino GND
DRV8833 VM  → Motor battery +

Indicator:

Arduino A3 → LED/resistor → GND
*/

#include <SPI.h>
#include <RF24.h>
#include <Servo.h>

// NRF24 / E01-ML01DP5

RF24 radio(9, 10);       // CE, CSN

const byte address[6] = "REM01";

// SERVOS

Servo servoAY1;
Servo servoAY11;
Servo servoBX1;
Servo servoP1;

Servo servoCY2;
Servo servoDX2;
Servo servoP2;

// SERVO PINS

const int servoAY11Pin = A4;
const int servoAY1Pin = 2;
const int servoBX1Pin = 4;
const int servoP1Pin  = 6;

const int servoCY2Pin = 7;
const int servoDX2Pin = 8;
const int servoP2Pin  = A2;

// INDICATOR

const int indicator = A3;

// DRV8833 MOTOR PINS

// Motor 1 = LEFT MOTOR

// PWM pin
const int motor1_PWM = 3;

// Direction pin
const int motor1_DIR = A0;

// Motor 2 = RIGHT MOTOR

// PWM pin
const int motor2_PWM = 5;

// Direction pin
const int motor2_DIR = A1;

// RECEIVED DATA

struct ControlData
{
  byte ABxAngle;
  byte AByAngle;

  byte pot1Angle;

  byte CDxAngle;
  byte CDyAngle;

  byte pot2Angle;

  byte button;

  // Driving data
  byte driveSpeed;
  byte driveX;
  byte driveY;
};

ControlData data;

// MOTOR FUNCTION

// forward = true  -> forward
// forward = false -> backward

void motor1Move(int pwm, bool forward)
{
  pwm = constrain(pwm, 0, 255);

  if (forward)
  {
    digitalWrite(motor1_DIR, LOW);
  }
  else
  {
    digitalWrite(motor1_DIR, HIGH);
  }

  analogWrite(motor1_PWM, pwm);
}

void motor2Move(int pwm, bool forward)
{
  pwm = constrain(pwm, 0, 255);

  if (forward)
  {
    digitalWrite(motor2_DIR, LOW);
  }
  else
  {
    digitalWrite(motor2_DIR, HIGH);
  }

  analogWrite(motor2_PWM, pwm);
}

// STOP MOTORS

void stopMotors()
{
  analogWrite(motor1_PWM, 0);
  analogWrite(motor2_PWM, 0);
}

// SETUP

void setup()
{
  Serial.begin(115200);

  // SERVOS

  servoAY11.attach(servoAY11Pin);
  servoAY1.attach(servoAY1Pin);
  servoBX1.attach(servoBX1Pin);
  servoP1.attach(servoP1Pin);

  servoCY2.attach(servoCY2Pin);
  servoDX2.attach(servoDX2Pin);
  servoP2.attach(servoP2Pin);

  // Start all servos at 90 degrees

  servoAY11.write(90);
  servoAY1.write(90);
  servoBX1.write(90);
  servoP1.write(90);

  servoCY2.write(90);
  servoDX2.write(90);
  servoP2.write(90);

  // INDICATOR

  pinMode(indicator, OUTPUT);

  digitalWrite(indicator, LOW);

  // MOTOR PINS

  pinMode(motor1_PWM, OUTPUT);
  pinMode(motor1_DIR, OUTPUT);

  pinMode(motor2_PWM, OUTPUT);
  pinMode(motor2_DIR, OUTPUT);

  // Motors OFF

  stopMotors();

  // NRF24

  if (!radio.begin())
  {
    Serial.println("NRF24 NOT FOUND!");

    while (1);
  }

  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(108);

  radio.openReadingPipe(1, address);

  radio.startListening();

  Serial.println("RECEIVER READY");
}

// LOOP

void loop()
{
  if (radio.available())
  {
    radio.read(&data, sizeof(data));

    // SERVO MODE
    // BUTTON ON

    if (data.button == 1)
    {
      // Indicator ON

      digitalWrite(indicator, HIGH);

      // Stop motors

      stopMotors();

      // SERVO CONTROL

      servoAY1.write(data.ABxAngle);

      servoAY11.write(180-data.ABxAngle);

      servoBX1.write(data.AByAngle);

      servoP1.write(data.pot1Angle);

      servoCY2.write(data.CDxAngle);

      servoDX2.write(data.CDyAngle);

      servoP2.write(data.pot2Angle);

      // SERIAL

      Serial.print("SERVO MODE | ");

      Serial.print("AB X: ");
      Serial.print(data.ABxAngle);

      Serial.print("  AY11 Opp: ");
      Serial.print(180 - data.ABxAngle);
      
      Serial.print("  AB Y: ");
      Serial.print(data.AByAngle);

      Serial.print("  P1: ");
      Serial.print(data.pot1Angle);

      Serial.print("  | CD X: ");
      Serial.print(data.CDxAngle);

      Serial.print("  CD Y: ");
      Serial.print(data.CDyAngle);

      Serial.print("  P2: ");
      Serial.println(data.pot2Angle);
    }

    // DRIVING MODE
    // BUTTON OFF

    else
    {
      // Indicator OFF

      digitalWrite(indicator, LOW);

      // SPEED

      int baseSpeed =
        map(data.driveSpeed, 0, 100, 0, 255);

      // FORWARD
      // CD X+

      if (data.driveX == 1)
      {
        int leftSpeed  = baseSpeed;
        int rightSpeed = baseSpeed;

        // RIGHT STEERING
        // CD Y+

        if (data.driveY == 1)
        {
          leftSpeed  = baseSpeed + 50;
          rightSpeed = baseSpeed - 50;
        }

        // LEFT STEERING
        // CD Y-

        else if (data.driveY == 2)
        {
          leftSpeed  = baseSpeed - 50;
          rightSpeed = baseSpeed + 50;
        }

        leftSpeed =
          constrain(leftSpeed, 0, 255);

        rightSpeed =
          constrain(rightSpeed, 0, 255);

        // Both forward

        motor1Move(leftSpeed, true);

        motor2Move(rightSpeed, true);

        Serial.print("FORWARD ");
      }

      // BACKWARD
      // CD X-

      else if (data.driveX == 2)
      {
        int leftSpeed  = baseSpeed;
        int rightSpeed = baseSpeed;

        // RIGHT STEERING

        if (data.driveY == 1)
        {
          leftSpeed  = baseSpeed + 50;
          rightSpeed = baseSpeed - 50;
        }

        // LEFT STEERING

        else if (data.driveY == 2)
        {
          leftSpeed  = baseSpeed - 50;
          rightSpeed = baseSpeed + 50;
        }

        leftSpeed =
          constrain(leftSpeed, 0, 255);

        rightSpeed =
          constrain(rightSpeed, 0, 255);

        // Both backward

        motor1Move(leftSpeed, false);

        motor2Move(rightSpeed, false);

        Serial.print("BACKWARD ");
      }

      // CD X CENTER

      else
      {

        // RIGHT
        // CD Y+

        if (data.driveY == 1)
        {
          // Left motor faster
          // Right motor slower

          int leftSpeed =
            constrain(baseSpeed + 50, 0, 255);

          int rightSpeed =
            constrain(baseSpeed - 50, 0, 255);

          motor1Move(leftSpeed, true);

          motor2Move(rightSpeed, true);

          Serial.print("RIGHT ");
        }

        // LEFT
        // CD Y-

        else if (data.driveY == 2)
        {
          // Left motor slower
          // Right motor faster

          int leftSpeed =
            constrain(baseSpeed - 50, 0, 255);

          int rightSpeed =
            constrain(baseSpeed + 50, 0, 255);

          motor1Move(leftSpeed, true);

          motor2Move(rightSpeed, true);


          Serial.print("LEFT ");
        }

        // CENTER

        else
        {
          stopMotors();

          Serial.print("STOP ");
        }
      }

      // SERIAL

      Serial.print("Speed: ");
      Serial.print(data.driveSpeed);

      Serial.print("%  X: ");
      Serial.print(data.driveX);

      Serial.print("  Y: ");
      Serial.println(data.driveY);
    }
  }
}