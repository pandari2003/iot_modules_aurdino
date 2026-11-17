/*
7Semi RS485 Pin	   =     Connect To	      =      Important Note
VCC	               -     Arduino 5V	      -     Must be 5V.
GND (Logic Side)	 -     Arduino GND	    -      Common ground.
TX	               -     Arduino Pin 2	  -      7Semi TX is data out to Arduino RX.
RX	               -     Arduino Pin 3	  -      7Semi RX is data in from Arduino TX.
RTS	               -     Arduino Pin 4    -   	 Controls send/receive direction.
A	                 -     SensorA(Yellow)	-      Connect all sensors' A wires here.
B	                 -     Sensor B (Blue)	-      Connect all sensors' B wires here.
GND                - (Bus Side)External 12V–24V - GND	Also connect this GND to Arduino GND.
*/
#include <ModbusMaster.h>
#include <SoftwareSerial.h>

// 7Semi RS485 Module Pins
#define RX_PIN 2    // Connect to TX on 7Semi
#define TX_PIN 3    // Connect to RX on 7Semi
#define RTS_PIN 4   // Connect to RTS on 7Semi

#define SLAVE_ID 1  // Default Modbus Slave ID

SoftwareSerial rs485Serial(RX_PIN, TX_PIN);
ModbusMaster node;

long activeBaud = 9600;

void preTransmission() {
  digitalWrite(RTS_PIN, HIGH);
  delayMicroseconds(50);
}

void postTransmission() {
  delayMicroseconds(50);
  digitalWrite(RTS_PIN, LOW);
}

// Convert two 16-bit Big-Endian Modbus registers into a 32-bit float
float registersToFloat(uint16_t highWord, uint16_t lowWord) {
  uint32_t combined = ((uint32_t)highWord << 16) | lowWord;
  float result;
  memcpy(&result, &combined, sizeof(result));
  return result;
}

// Auto-detect sensor baud rate (9600 or 4800) on startup
void detectBaudRate() {
  long bauds[] = {9600, 4800};
  for (int i = 0; i < 2; i++) {
    rs485Serial.begin(bauds[i]);
    node.begin(SLAVE_ID, rs485Serial);
    node.preTransmission(preTransmission);
    node.postTransmission(postTransmission);

    uint8_t res = node.readHoldingRegisters(0x0000, 2);
    if (res == node.ku8MBSuccess || res == node.ku8MBIllegalDataAddress) {
      activeBaud = bauds[i];
      Serial.print("Sensor detected at baud rate: ");
      Serial.println(activeBaud);
      return;
    }
    delay(200);
  }
  Serial.println("Warning: Auto-detect timed out. Defaulting to 9600 baud.");
  rs485Serial.begin(9600);
}

void setup() {
  Serial.begin(9600);
  pinMode(RTS_PIN, OUTPUT);
  digitalWrite(RTS_PIN, LOW);

  Serial.println("Initializing RS485 Freshwater DO Sensor...");
  detectBaudRate();
}

void loop() {
  Serial.println("========================================");

  // 1. Read DO Saturation (0x0000-0x0001), DO Conc (0x0002-0x0003), Temp (0x0004-0x0005)
  uint8_t resMain = node.readHoldingRegisters(0x0000, 6);

  if (resMain == node.ku8MBSuccess) {
    float doSatRaw = registersToFloat(node.getResponseBuffer(0), node.getResponseBuffer(1));
    float doConc   = registersToFloat(node.getResponseBuffer(2), node.getResponseBuffer(3));
    float tempC    = registersToFloat(node.getResponseBuffer(4), node.getResponseBuffer(5));

    // Some probes output saturation as 0.0-1.0 (e.g., 0.918 = 91.8%), others as 0-100
    float doSatPercent = (doSatRaw <= 2.5) ? (doSatRaw * 100.0) : doSatRaw;

    Serial.print("Dissolved Oxygen : ");
    Serial.print(doConc, 2);
    Serial.println(" mg/L");

    Serial.print("DO Saturation    : ");
    Serial.print(doSatPercent, 1);
    Serial.println(" %");

    Serial.print("Water Temperature: ");
    Serial.print(tempC, 2);
    Serial.println(" °C");
  } else {
    Serial.print("Error reading DO/Temp! Code: 0x");
    Serial.println(resMain, HEX);
  }

  delay(250); // Required >200ms polling interval between Modbus frames

  // 2. Read Salinity (0x1020) and Atmospheric Pressure (0x1022)
  uint8_t resEnv = node.readHoldingRegisters(0x1020, 3);
  if (resEnv == node.ku8MBSuccess) {
    float salinity = node.getResponseBuffer(0) / 10.0;   // Freshwater defaults to 0.0%
    float pressure = node.getResponseBuffer(2) / 100.0;  // Default ~101.33 kPa

    Serial.print("Salinity Setting : ");
    Serial.print(salinity, 1);
    Serial.println(" %");

    Serial.print("Atm. Pressure    : ");
    Serial.print(pressure, 2);
    Serial.println(" kPa");
  }

  // Update continuously every 5 seconds
  delay(4750);
}
