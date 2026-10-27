
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int SDA_PIN = 21;
const int SCL_PIN = 22;
const int ESPNOW_CHANNEL = 1;

struct TemperaturePacket {
  float temperature;
  bool sensorOK;
  uint32_t counter;
};

TemperaturePacket latestPacket;

volatile bool newData = false;
volatile unsigned long lastReceiveMs = 0;

portMUX_TYPE packetMux = portMUX_INITIALIZER_UNLOCKED;

// ESP-NOW receive callback
void onDataReceived(const esp_now_recv_info_t *info,
                    const uint8_t *data,
                    int len) {
  if (len != sizeof(TemperaturePacket)) {
    return;
  }

  TemperaturePacket incoming;
  memcpy(&incoming, data, sizeof(incoming));

  portENTER_CRITICAL(&packetMux);
  latestPacket = incoming;
  lastReceiveMs = millis();
  newData = true;
  portEXIT_CRITICAL(&packetMux);
}

void setup() {
  Serial.begin(115200);

  // Initialize LCD
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Temperature:");
  lcd.setCursor(0, 1);
  lcd.print("Waiting...");

  // Initialize ESP-NOW
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  esp_wifi_set_channel(ESPNOW_CHANNEL,
                       WIFI_SECOND_CHAN_NONE);

  Serial.println();
  Serial.print("Receiver MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.println("ESP-NOW channel: 1");

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    lcd.clear();
    lcd.print("ESP-NOW error");
    while (true) {
      delay(1000);
    }
  }

  esp_now_register_recv_cb(onDataReceived);

  Serial.println("Receiver ready");
}

void loop() {
  static unsigned long lastDisplayMs = 0;

  if (millis() - lastDisplayMs < 100) {
    return;
  }
  lastDisplayMs = millis();

  TemperaturePacket packet;
  bool received = false;
  unsigned long lastRx;

  // Safely copy data shared with the receive callback
  portENTER_CRITICAL(&packetMux);
  if (newData) {
    packet = latestPacket;
    newData = false;
    received = true;
  }
  lastRx = lastReceiveMs;
  portEXIT_CRITICAL(&packetMux);

  // Update LCD only when a new packet arrives
  if (received) {
    if (packet.sensorOK) {
      char line[17];

      snprintf(line, sizeof(line), "Temp: %5.1f C",
               packet.temperature);

      lcd.setCursor(0, 0);
      lcd.print("Temperature:    ");

      lcd.setCursor(0, 1);
      lcd.print(line);
      lcd.print("                ");

      Serial.print("Temperature: ");
      Serial.print(packet.temperature, 2);
      Serial.print(" C | Packet: ");
      Serial.println(packet.counter);
    } else {
      lcd.setCursor(0, 0);
      lcd.print("DS18B20 error   ");
      lcd.setCursor(0, 1);
      lcd.print("Check sensor    ");

      Serial.println("Transmitter sensor error");
    }
  }

  // Display connection loss only after 5 seconds without data
  if (lastRx == 0 || millis() - lastRx > 5000) {
    lcd.setCursor(0, 0);
    lcd.print("Waiting for data");
    lcd.setCursor(0, 1);
    lcd.print("No signal       ");
  }
}
