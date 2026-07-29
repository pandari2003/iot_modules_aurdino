#include <WiFi.h>
#include <esp_now.h>

uint8_t receiverMAC[] = {0x24,0x6F,0x28,0xAB,0xCD,0xEF};

typedef struct {
  int servo1Angle;
  int servo2Angle;
} ServoData;

ServoData data;

void setup() {

  Serial.begin(115200);

  pinMode(32, INPUT);
  pinMode(34, INPUT);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_peer_info_t peerInfo = {};

  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  esp_now_add_peer(&peerInfo);

  Serial.println("Joystick Ready");
}

void loop() {

  int x = analogRead(32);
  int y = analogRead(34);

  data.servo1Angle = map(y, 0, 4095, 0, 90);
  data.servo2Angle = map(x, 0, 4095, 0, 90);

  esp_now_send(receiverMAC, (uint8_t *)&data, sizeof(data));

  Serial.print("S1=");
  Serial.print(data.servo1Angle);

  Serial.print(" S2=");
  Serial.println(data.servo2Angle);

  delay(20);   // about 50 updates per second
}