#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Telezer_J";
const char* password = "Telezer12";

WebServer server(80);

const int ledPin = 13;

void ledOn() {
  digitalWrite(ledPin, HIGH);
  server.send(200, "text/plain", "LED ON");
}

void ledOff() {
  digitalWrite(ledPin, LOW);
  server.send(200, "text/plain", "LED OFF");
}

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Board1 IP: ");
  Serial.println(WiFi.localIP());

  server.on("/on", ledOn);
  server.on("/off", ledOff);

  server.begin();
}

void loop() {
  server.handleClient();
}