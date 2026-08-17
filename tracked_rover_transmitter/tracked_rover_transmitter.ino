/*
Arduino UNO	Device
D2	Servo AY1
D4	Servo BX1
D6	Servo P1
D7	Servo CY2
D8	Servo DX2
A2	Servo P2
D9	E01 CE
D10	E01 CSN
D11	E01 MOSI
D12	E01 MISO
D13	E01 SCK
D3	Motor 1 PWM
A0	Motor 1 DIR
D5	Motor 2 PWM
A1	Motor 2 DIR
A3	Indicator btn
*/

#include <SPI.h>
#include <RF24.h>

// NRF24 / E01-ML01DP5

RF24 radio(4, 5);       // CE, CSN

const byte address[6] = "REM01";

// INPUT PINS - ESP32

// Joystick AB
const int joystickABX = 34;
const int joystickABY = 35;
const int potPin1     = 32;

// Joystick CD
const int joystickCDX = 33;
const int joystickCDY = 25;
const int potPin2     = 26;

// Control button
const int controlButton = 27;

// SERVO ANGLES

int ABxAngle = 90;
int AByAngle = 90;

int CDxAngle = 90;
int CDyAngle = 90;

// DRIVING SPEED

// Starts at 50%
int speedValue = 50;

// JOYSTICK SETTINGS

const int center = 2048;
const int deadZone = 200;

// DATA PACKET

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

// JOYSTICK SERVO FUNCTION

void controlJoystick(int pin, int &angle)
{
  int value = analogRead(pin);

  if (value > center + deadZone)
  {
    angle++;
  }
  else if (value < center - deadZone)
  {
    angle--;
  }

  angle = constrain(angle, 0, 180);
}

// SETUP

void setup()
{
  Serial.begin(115200);

  analogReadResolution(12);

  pinMode(controlButton, INPUT_PULLUP);

  // NRF24

  if (!radio.begin())
  {
    Serial.println("NRF24 NOT FOUND!");

    while (1);
  }

  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(108);

  radio.openWritingPipe(address);

  radio.stopListening();


  Serial.println("TRANSMITTER READY");
}

// LOOP

void loop()
{
  int buttonState = digitalRead(controlButton);

  
  // SERVO MODE
  // BUTTON ON  =

  if (buttonState == LOW)
  { 
    // AB JOYSTICK 

    controlJoystick(joystickABX, ABxAngle);
    controlJoystick(joystickABY, AByAngle);
 
    // CD JOYSTICK 

    controlJoystick(joystickCDX, CDxAngle);
    controlJoystick(joystickCDY, CDyAngle);

    // POTENTIOMETER 1 

    int pot1Value = analogRead(potPin1);

    int pot1Angle =
      map(pot1Value, 0, 4095, 0, 180);
 
    // POTENTIOMETER 2 

    int pot2Value = analogRead(potPin2);

    int pot2Angle =
      map(pot2Value, 0, 4095, 0, 180);
 
    // PACKET 

    data.ABxAngle = ABxAngle;
    data.AByAngle = AByAngle;

    data.pot1Angle = pot1Angle;

    data.CDxAngle = CDxAngle;
    data.CDyAngle = CDyAngle;

    data.pot2Angle = pot2Angle;

    data.button = 1;
 
    // SEND 

    radio.write(&data, sizeof(data));
 
    // SERIAL 

    Serial.print("SERVO MODE | ");

    Serial.print("AB X: ");
    Serial.print(ABxAngle);

    Serial.print("  AB Y: ");
    Serial.print(AByAngle);

    Serial.print("  P1: ");
    Serial.print(pot1Angle);

    Serial.print("  | CD X: ");
    Serial.print(CDxAngle);

    Serial.print("  CD Y: ");
    Serial.print(CDyAngle);

    Serial.print("  P2: ");
    Serial.println(pot2Angle);
  }

  // DRIVING MODE
  // BUTTON OFF  =

  else
  { 
    // READ JOYSTICKS 

    int abXValue = analogRead(joystickABX);

    int cdXValue = analogRead(joystickCDX);

    int cdYValue = analogRead(joystickCDY);

  
    // SPEED CONTROL
    // AB X  

    if (abXValue > center + deadZone)
    {
      speedValue++;
    }
    else if (abXValue < center - deadZone)
    {
      speedValue--;
    }

    speedValue = constrain(speedValue, 0, 100);

    // DRIVING DIRECTION  

    // X direction
    // 1 = forward
    // 2 = backward
    // 0 = center

    if (cdXValue > center + deadZone)
    {
      data.driveX = 1;
    }
    else if (cdXValue < center - deadZone)
    {
      data.driveX = 2;
    }
    else
    {
      data.driveX = 0;
    }

    // Y direction
    // 1 = right
    // 2 = left
    // 0 = center

    if (cdYValue > center + deadZone)
    {
      data.driveY = 1;
    }
    else if (cdYValue < center - deadZone)
    {
      data.driveY = 2;
    }
    else
    {
      data.driveY = 0;
    }
 
    // SEND DRIVING DATA 

    data.button = 0;

    data.driveSpeed = speedValue;

    radio.write(&data, sizeof(data));

    // SERIAL 

    Serial.print("DRIVING | Speed: ");
    Serial.print(speedValue);

    Serial.print("%  X: ");
    Serial.print(data.driveX);

    Serial.print("  Y: ");
    Serial.println(data.driveY);
  }

  delay(20);
}