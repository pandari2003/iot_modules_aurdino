/*
1. ESP32 — Remote Controller
Device	    Pin	ESP32
AB Joystick	VRx	    GPIO 34
	          VRy	    GPIO 35
          	VCC	    3.3V
          	GND	    GND
CD Joystick	VRx	    GPIO 33
	          VRy	    GPIO 36
	          VCC	    3.3V
	          GND	    GND
Potentiometer 1	OUT	  GPIO 32
	              VCC	  3.3V
	              GND	  GND
Potentiometer 2	OUT	  GPIO 39
	              VCC	  3.3V
	              GND 	GND
Push Button	One side	GPIO 25
	Other side	        GND
    LED	+	            GPIO 2 through resistor
	  −	                GND
ESP32        →     E01-ML01DP5
E01-ML01DP5	ESP32
VCC	                3.3V external/stable supply
GND	                GND
CE	                GPIO 4
CSN	                GPIO 5
SCK	                GPIO 18
MOSI	              GPIO 23
MISO	              GPIO 19
IRQ	                Not connected

Important: E01-ML01DP5 PA/LNA needs a stable 3.3 V supply. 
Put a 10–47 µF capacitor between VCC and GND close to the module.
  =========================================================
  ESP32 REMOTE / TRANSMITTER
  =========================================================

  Controls:
    AB Joystick X -> Speed
    AB Joystick Y -> Servo AY/BX control
    CD Joystick X -> Forward / Backward / Servo CY2
    CD Joystick Y -> Rotation / Servo DX2
    Pot 1         -> Servo P1
    Pot 2         -> Servo P2
    Button        -> Servo Mode / Driving Mode
    LED           -> Mode indicator

  Wireless:
    nRF24L01+ PA/LNA

  =========================================================
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// =========================================================
// ESP32 INPUT PINS
// =========================================================

const int joystickABX = 34;
const int joystickABY = 35;
const int potPin1     = 32;

const int joystickCDX = 33;
const int joystickCDY = 36;
const int potPin2     = 39;

// =========================================================
// BUTTON
// =========================================================

const int controlButton = 25;

// =========================================================
// INDICATOR LED
// =========================================================

const int indicator = 2;

// =========================================================
// nRF24L01
// =========================================================

const int CE_PIN  = 4;
const int CSN_PIN = 5;

RF24 radio(CE_PIN, CSN_PIN);

const byte address[6] = "ROVER";

// =========================================================
// DATA PACKET
// =========================================================

struct ControlData {

  int16_t abX;
  int16_t abY;

  int16_t cdX;
  int16_t cdY;

  int16_t pot1;
  int16_t pot2;

  uint8_t button;
};

ControlData data;

// =========================================================
// SETUP
// =========================================================

void setup() {

  Serial.begin(115200);

  // ESP32 ADC resolution = 10 bit
  // Keeps original Arduino 0-1023 logic
  analogReadResolution(10);

  // -------------------------------------------------------
  // BUTTON
  // -------------------------------------------------------

  pinMode(controlButton, INPUT_PULLUP);

  // -------------------------------------------------------
  // LED
  // -------------------------------------------------------

  pinMode(indicator, OUTPUT);

  digitalWrite(indicator, LOW);

  // -------------------------------------------------------
  // nRF24
  // -------------------------------------------------------

  if (!radio.begin()) {

    Serial.println("nRF24 transmitter not detected!");

    while (1) {
      digitalWrite(indicator, !digitalRead(indicator));
      delay(200);
    }
  }

  radio.setPALevel(RF24_PA_HIGH);

  radio.setDataRate(RF24_250KBPS);

  radio.setChannel(108);

  radio.openWritingPipe(address);

  radio.stopListening();

  Serial.println("ESP32 REMOTE READY");
}

// =========================================================
// LOOP
// =========================================================

void loop() {

  // =======================================================
  // READ JOYSTICKS
  // =======================================================

  data.abX = analogRead(joystickABX);

  data.abY = analogRead(joystickABY);

  data.cdX = analogRead(joystickCDX);

  data.cdY = analogRead(joystickCDY);

  // =======================================================
  // READ POTENTIOMETERS
  // =======================================================

  data.pot1 = analogRead(potPin1);

  data.pot2 = analogRead(potPin2);

  // =======================================================
  // READ BUTTON
  // =======================================================

  data.button = digitalRead(controlButton);

  // =======================================================
  // LED
  // =======================================================
  //
  // Original:
  // LOW  = SERVO / WORKING MODE
  // HIGH = DRIVING MODE
  //
  // Keep same indication.
  // =======================================================

  if (data.button == LOW) {

    digitalWrite(indicator, HIGH);

  } else {

    digitalWrite(indicator, LOW);
  }

  // =======================================================
  // SEND DATA
  // =======================================================

  bool success = radio.write(&data, sizeof(data));

  if (!success) {

    Serial.println("Wireless transmission failed");

  } else {

    Serial.print("ABX: ");
    Serial.print(data.abX);

    Serial.print(" ABY: ");
    Serial.print(data.abY);

    Serial.print(" CDX: ");
    Serial.print(data.cdX);

    Serial.print(" CDY: ");
    Serial.print(data.cdY);

    Serial.print(" P1: ");
    Serial.print(data.pot1);

    Serial.print(" P2: ");
    Serial.print(data.pot2);

    Serial.print(" Button: ");
    Serial.println(data.button);
  }

  // Send approximately every 20 ms
  delay(5);
}