
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// DS18B20 DATA pin
const int ONE_WIRE_BUS = 4;
const int ESPNOW_CHANNEL = 1;

// REPLACE with ESP32 #2 receiver MAC address.
uint8_t receiverAddress[] = {
  0x04, 0xB2, 0x47, 0x83, 0x71, 0x90
};

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

struct TemperaturePacket {
  float temperature;
  bool sensorOK;
  uint32_t counter;
};

TemperaturePacket packet;
uint32_t packetCounter = 0;

void setup() {
  Serial.begin(115200);

  sensors.begin();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Must match the receiver's channel.
  esp_wifi_set_channel(ESPNOW_CHANNEL,
                       WIFI_SECOND_CHAN_NONE);

  Serial.println();
  Serial.print("Transmitter MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.println("ESP-NOW channel: 1");

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    while (true) {
      delay(1000);
    }
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = ESPNOW_CHANNEL;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add receiver peer");
    while (true) {
      delay(1000);
    }
  }

  Serial.println("ESP-NOW transmitter ready");
}

void loop() {
  sensors.requestTemperatures();

  float temperature = sensors.getTempCByIndex(0);

  packet.counter = ++packetCounter;

  if (temperature != DEVICE_DISCONNECTED_C &&
      temperature >= -55.0 &&
      temperature <= 125.0) {
    packet.temperature = temperature;
    packet.sensorOK = true;

    Serial.print("Temperature: ");
    Serial.print(temperature, 2);
    Serial.println(" C");
  } else {
    packet.temperature = 0.0;
    packet.sensorOK = false;

    Serial.println("DS18B20 not detected!");
  }

  esp_err_t result = esp_now_send(
    receiverAddress,
    (uint8_t *)&packet,
    sizeof(packet)
  );

  if (result == ESP_OK) {
    Serial.println("Packet queued");
  } else {
    Serial.print("Send error: ");
    Serial.println(result);
  }

  delay(2000);
}
