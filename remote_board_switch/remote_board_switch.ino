#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Telezer_J";
const char* password = "Telezer12";

String serverIP = "192.168.29.44";

const int switchPin = 12;

bool lastButtonState = HIGH;   // Previous button state
bool ledState = false;         // false = OFF, true = ON

void setup() {
  Serial.begin(115200);

  pinMode(switchPin, INPUT_PULLUP);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected");
}

void loop() {
  bool buttonState = digitalRead(switchPin);

  // Detect button press (HIGH -> LOW)
  if (lastButtonState == HIGH && buttonState == LOW) {

    // Toggle LED state
    ledState = !ledState;

    HTTPClient http;

    if (ledState) {
      Serial.println("LED ON");
      http.begin("http://" + serverIP + "/on");
    } else {
      Serial.println("LED OFF");
      http.begin("http://" + serverIP + "/off");
    }

    int httpCode = http.GET();
    Serial.print("HTTP Response: ");
    Serial.println(httpCode);

    http.end();

    delay(50);   // Small debounce
  }

  lastButtonState = buttonState;
}