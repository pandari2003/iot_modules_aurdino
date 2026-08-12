/*
Joystick AB
Joystick AB	ESP32
VCC------------	3.3V
GND------------	GND
VRx------------	GPIO 34
VRy	------------GPIO 35

Potentiometer 1
Pot 1	ESP32
Outer pin	------------3.3V
Other outer------------ pin	GND
Middle pin	v------------GPIO 32

Joystick CD
Joystick CD	ESP32
VCC	------------3.3V
GND	------------GND
VRx	------------GPIO 33
VRy	------------GPIO 25

Potentiometer 2
Pot 2	ESP32
Outer pin------------	3.3V
Other outer pin	GND
Middle pin	------------GPIO 26

Control button
ESP32 GPIO 27 ───── BUTTON ───── GND

*****ESP32 → E01-ML01DP5***
E01-ML01DP5	ESP32
VCC	------------3.3V
GND	------------GND
CE	------------GPIO 4
CSN	------------GPIO 5
SCK	------------GPIO 18
MOSI------------	GPIO 23
MISO	------------GPIO 19
*/
#include <SPI.h>
#include <RF24.h>
 
// NRF24 / E01 

RF24 radio(4, 5);   // CE, CSN

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
 
// JOYSTICK SETTINGS 

const int center = 2048;
const int deadZone = 200;
 
// DATA PACKET 

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
 
// JOYSTICK FUNCTION 

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
  // BUTTON ON 

  if (buttonState == LOW)
  {
  
    // JOYSTICK AB
  
    controlJoystick(joystickABX, ABxAngle);
    controlJoystick(joystickABY, AByAngle);
  
    // JOYSTICK CD
  
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
  
    // CREATE PACKET
  
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
  // BUTTON OFF 

  else
  {
  
    // CREATE DRIVING PACKET
  
    data.button = 0;

    // AB X joystick

    int abXValue = analogRead(joystickABX);

    if (abXValue > center + deadZone)
    {
      data.ABxAngle = 1;
    }
    else if (abXValue < center - deadZone)
    {
      data.ABxAngle = 2;
    }
    else
    {
      data.ABxAngle = 0;
    }

    // CD X joystick

    int cdXValue = analogRead(joystickCDX);

    if (cdXValue > center + deadZone)
    {
      data.CDxAngle = 1;
    }
    else if (cdXValue < center - deadZone)
    {
      data.CDxAngle = 2;
    }
    else
    {
      data.CDxAngle = 0;
    }

    // Send driving command

    radio.write(&data, sizeof(data));

    Serial.print("DRIVING MODE | Motor 1: ");

    Serial.print(data.ABxAngle);

    Serial.print("  Motor 2: ");

    Serial.println(data.CDxAngle);
  }

  delay(30);
}
