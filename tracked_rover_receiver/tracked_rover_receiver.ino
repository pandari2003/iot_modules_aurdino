/*
Arduino UNO pin	Device
D2------------	Servo AY1
D3------------	Servo BX1
D4------------	Servo P1
D5------------	Servo CY2
D6------------	Servo DX2
D7------------	Servo P2
D8------------	E01 CE
D9------------	Indicator
D10------------	E01 CSN
D11------------	E01 MOSI
D12------------	E01 MISO
D13------------	E01 SCK
A0------------	Motor 1 IN1
A1------------	Motor 1 IN2
A2------------	Motor 2 IN3
A3------------	Motor 2 IN4
A4------------	Available
A5------------	Available
*/
#include <SPI.h>
#include <RF24.h>
#include <Servo.h>
 
// NRF24 / E01
 
RF24 radio(8, 10);   // CE, CSN

const byte address[6] = "REM01";
 
// SERVOS
 
Servo servoAY1;
Servo servoBX1;
Servo servoP1;

Servo servoCY2;
Servo servoDX2;
Servo servoP2;
 
// SERVO PINS
 
const int servoAY1Pin = 4;
const int servoBX1Pin = 5;
const int servoP1Pin  = 6;

const int servoCY2Pin = 8;
const int servoDX2Pin = 9;
const int servoP2Pin  = 7;
 
// INDICATOR 

const int indicator = 3;
 
// DC MOTOR PINS
 
// Motor 1
const int motor1_IN1 = 10;
const int motor1_IN2 = 11;

// Motor 2
const int motor2_IN3 = 12;
const int motor2_IN4 = 13;
 
// RECEIVED DATA
 
struct ControlData {

  byte ABxAngle;
  byte AByAngle;

  byte pot1Angle;

  byte CDxAngle;
  byte CDyAngle;

  byte pot2Angle;

  byte button;

};

ControlData data;
 
// SETUP
 
void setup()
{
  Serial.begin(115200);
   
  // SERVOS
   
  servoAY1.attach(servoAY1Pin);
  servoBX1.attach(servoBX1Pin);
  servoP1.attach(servoP1Pin);

  servoCY2.attach(servoCY2Pin);
  servoDX2.attach(servoDX2Pin);
  servoP2.attach(servoP2Pin);

  // Start at 90 degrees

  servoAY1.write(90);
  servoBX1.write(90);
  servoP1.write(90);

  servoCY2.write(90);
  servoDX2.write(90);
  servoP2.write(90);
   
  // INDICATOR
   
  pinMode(indicator, OUTPUT);
   
  // MOTOR PINS
   
  pinMode(motor1_IN1, OUTPUT);
  pinMode(motor1_IN2, OUTPUT);

  pinMode(motor2_IN3, OUTPUT);
  pinMode(motor2_IN4, OUTPUT);

  // Motors OFF

  digitalWrite(motor1_IN1, LOW);
  digitalWrite(motor1_IN2, LOW);

  digitalWrite(motor2_IN3, LOW);
  digitalWrite(motor2_IN4, LOW);
   
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

       // STOP MOTORS
 
      digitalWrite(motor1_IN1, LOW);
      digitalWrite(motor1_IN2, LOW);

      digitalWrite(motor2_IN3, LOW);
      digitalWrite(motor2_IN4, LOW);

       // SERVO CONTROL
 
      servoAY1.write(data.ABxAngle);

      servoBX1.write(data.AByAngle);

      servoP1.write(data.pot1Angle);

      servoCY2.write(data.CDxAngle);

      servoDX2.write(data.CDyAngle);

      servoP2.write(data.pot2Angle);
 
      // SERIAL
 
      Serial.print("SERVO MODE | ");

      Serial.print("AB X: ");
      Serial.print(data.ABxAngle);

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

      Serial.print("DRIVING MODE | ");
   
      // MOTOR 1
      // Controlled by AB X
   
      if (data.ABxAngle == 1)
      {

        // Forward

        digitalWrite(motor1_IN1, HIGH);
        digitalWrite(motor1_IN2, LOW);

        Serial.print("M1 FORWARD ");
      }

      else if (data.ABxAngle == 2)
      {

        // Reverse

        digitalWrite(motor1_IN1, LOW);
        digitalWrite(motor1_IN2, HIGH);

        Serial.print("M1 REVERSE ");
      }

      else
      {

        // Stop

        digitalWrite(motor1_IN1, LOW);
        digitalWrite(motor1_IN2, LOW);

        Serial.print("M1 STOP ");
      }
  
      // MOTOR 2
      // Controlled by CD X
   
      if (data.CDxAngle == 1)
      {

        // Forward

        digitalWrite(motor2_IN3, HIGH);
        digitalWrite(motor2_IN4, LOW);

        Serial.println("M2 FORWARD");
      }

      else if (data.CDxAngle == 2)
      {

        // Reverse

        digitalWrite(motor2_IN3, LOW);
        digitalWrite(motor2_IN4, HIGH);

        Serial.println("M2 REVERSE");
      }

      else
      {

        // Stop

        digitalWrite(motor2_IN3, LOW);
        digitalWrite(motor2_IN4, LOW);

        Serial.println("M2 STOP");
      }
    }
  }
}

