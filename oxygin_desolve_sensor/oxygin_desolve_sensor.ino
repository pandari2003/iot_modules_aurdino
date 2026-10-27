
#include <SoftwareSerial.h>

SoftwareSerial sensor(2, 3); // RX, TX

const byte request[] = {
  0x01, 0x03, 0x00, 0x00,
  0x00, 0x06, 0xC5, 0xC8
};

uint16_t crc16(byte *data, byte len) {
  uint16_t crc = 0xFFFF;

  for (byte i = 0; i < len; i++) {
    crc ^= data[i];

    for (byte j = 0; j < 8; j++) {
      if (crc & 1)
        crc = (crc >> 1) ^ 0xA001;
      else
        crc >>= 1;
    }
  }

  return crc;
}

float readFloatBE(byte *p) {
  union {
    uint32_t u;
    float f;
  } value;

  value.u = ((uint32_t)p[0] << 24) |
            ((uint32_t)p[1] << 16) |
            ((uint32_t)p[2] << 8) |
            (uint32_t)p[3];

  return value.f;
}

void setup() {
  Serial.begin(9600);
  sensor.begin(4800);

  Serial.println("DISSOLVED OXYGEN SENSOR");
  Serial.println("-----------------------");
}

void loop() {
  while (sensor.available()) sensor.read();

  sensor.write(request, sizeof(request));
  sensor.flush();

  byte data[32];
  byte n = 0;
  unsigned long start = millis();

  while (millis() - start < 1200) {
    while (sensor.available() && n < sizeof(data)) {
      data[n++] = sensor.read();
    }
  }

  if (n == 0) {
    Serial.println("No sensor response");
    Serial.println("Check RS485 wiring and Modbus settings");
    delay(2000);
    return;
  }

  // Expected response: 01 03 0C + 12 data bytes + CRC
  if (n != 17 || data[0] != 0x01 ||
      data[1] != 0x03 || data[2] != 0x0C) {
    Serial.println("Unexpected response:");
    for (byte i = 0; i < n; i++) {
      if (data[i] < 16) Serial.print('0');
      Serial.print(data[i], HEX);
      Serial.print(' ');
    }
    Serial.println();
    delay(2000);
    return;
  }

  uint16_t crc = crc16(data, n - 2);
  uint16_t received = data[n - 2] |
                      ((uint16_t)data[n - 1] << 8);

  if (crc != received) {
    Serial.println("Invalid CRC");
    delay(2000);
    return;
  }

  float oxygenSaturation = readFloatBE(&data[3]);
  float oxygenMgL = readFloatBE(&data[7]);
  float temperature = readFloatBE(&data[11]);

  Serial.println("\n--- SENSOR DATA ---");

  Serial.print("Dissolved Oxygen: ");
  Serial.print(oxygenMgL, 2);
  Serial.println(" mg/L");

  Serial.print("Oxygen Saturation: ");
  Serial.print(oxygenSaturation, 2);
  Serial.println(" %");

  Serial.print("Temperature: ");
  Serial.print(temperature, 2);
  Serial.println(" C");

  delay(2000);
}
