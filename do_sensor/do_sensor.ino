#include <ModbusMaster.h>

// Define ESP32 Hardware Serial 2 Controller Pins
#define RXD2 16  // Connects directly to TXD on TTL-485 v2.0 module
#define TXD2 17  // Connects directly to RXD on TTL-485 v2.0 module

ModbusMaster node;

// Configuration defaults (Adjust these if your manual specifies exact targets)
const long TARGET_BAUD = 9600;     // Factory standard for most freshwater DO probes
const int  SLAVE_ID    = 1;        // Common default unit address
const uint16_t DO_REG  = 0x0000;   // Register address for Dissolved Oxygen / Saturation

void setup() {
  // Initialize standard hardware USB lane to your computer's Serial Monitor
  Serial.begin(115200); 
  delay(1000);
  Serial.println("--- ESP32 HARDWARE SERIAL DO SENSOR READING ---");
  // Initialize ESP32 native Hardware Serial 2 port
  Serial2.begin(TARGET_BAUD, SERIAL_8N1, RXD2, TXD2);
  
  // Assign the Modbus node to operate over Hardware Serial 2
  node.begin(SLAVE_ID, Serial2);
  
  Serial.println("[SYSTEM INIT]: Listening for RS485 Sensor data stream...");
}

void loop() {
  // Poll 2 sequential registers starting at 0x0000 using Input Registers (Function Code 0x04)
  uint8_t result = node.readInputRegisters(DO_REG, 2);

  // Fallback: If Input Registers timeout, automatically try Holding Registers (Function Code 0x03)
  if (result == 0xE2) {
    result = node.readHoldingRegisters(DO_REG, 2);
  }

  // Evaluate the Modbus transmission frame verification status
  if (result == node.ku8MBSuccess) {
    // Extract raw base-10 decimal data frame values directly from the Modbus internal cache
    uint16_t raw_value1 = node.getResponseBuffer(0); // Hexadecimal value converted to Decimal
    uint16_t raw_value2 = node.getResponseBuffer(1); // Hexadecimal value converted to Decimal

    // Apply industrial integer scaling factors (converts raw integers back into floats)
    float dissolvedOxygen = raw_value1 * 0.01; // Scale factor 0.01 transforms 824 to 8.24 mg/L
    float temperature     = raw_value2 * 0.1;  // Scale factor 0.1 transforms 245 to 24.5 °C

    // Output formatted metrics directly to the interface layout
    Serial.print(" [SUCCESS] | ");
    Serial.print("DO Concentration: ");
    Serial.print(dissolvedOxygen, 2);
    Serial.print(" mg/L (ppm) | ");
    
    // Note: If your manual indicates register 0 is % Saturation, read raw_value1 as percentage
    Serial.print("Oxygen Saturation: ");
    Serial.print((raw_value1 * 0.01), 2); 
    Serial.print(" % | ");
    
    Serial.print("Water Temp: ");
    Serial.print(temperature, 1);
    Serial.println(" °C");

  } else {
    // Fault condition routing handler logic
    Serial.print("[COMMUNICATION ERROR] | Modbus Error Code: 0x");
    Serial.print(result, HEX);
    
    if (result == 0xE2) {
      Serial.println(" (Timeout: Sensor did not reply. Verify common ground link or swap A+/B- wires)");
    } else if (result == 0xE0) {
      Serial.println(" (Invalid Response frame: Check device ID configuration parameters)");
    } else {
      Serial.println(" (Check wire insulation contacts)");
    }
  }

  delay(2000); // Poll sensor data points every 2 seconds
}
