#include <WiFi.h>
#include <esp_now.h>
#include <ESP32Servo.h>

Servo servo1;
Servo servo2;

typedef struct {
  int servo1Angle;
  int servo2Angle;
} ServoData;

ServoData data;

void onReceive(const esp_now_recv_info_t *recvInfo, const uint8_t *incomingData, int len) {

  memcpy(&data, incomingData, sizeof(data));

  data.servo1Angle = constrain(data.servo1Angle, 0, 90);
  data.servo2Angle = constrain(data.servo2Angle, 0, 90);

  servo1.write(data.servo1Angle);
  servo2.write(data.servo2Angle);

  Serial.print("Servo1: ");
  Serial.print(data.servo1Angle);

  Serial.print(" Servo2: ");
  Serial.println(data.servo2Angle);
}

void setup() {

  Serial.begin(115200);

  servo1.attach(18);
  servo2.attach(19);

  servo1.write(45);
  servo2.write(45);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_register_recv_cb(onReceive);

  Serial.print("Receiver MAC: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
}