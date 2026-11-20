
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// Pin configuration
#define DHT_PIN 4
#define DHT_TYPE DHT11

#define OLED_SDA 8
#define OLED_SCL 9

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// Objects
Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {

  Serial.begin(115200);

  // Initialize I2C
  Wire.begin(OLED_SDA, OLED_SCL);

  // Initialize OLED
  if (!display.begin(
        SSD1306_SWITCHCAPVCC, 0x3C)) {

    Serial.println("OLED initialization failed!");
    while (1);
  }

  // Initialize DHT11
  dht.begin();

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(15, 20);
  display.println("DHT11 MONITOR");
  display.setCursor(20, 40);
  display.println("Initializing...");
  display.display();

  delay(2000);
}

void loop() {

  // Read sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(10, 0);
  display.println("ENVIRONMENT MONITOR");

  display.drawLine(0, 13, 128, 13,
                   SSD1306_WHITE);

  if (isnan(temperature) || isnan(humidity)) {

    display.setCursor(15, 30);
    display.println("Sensor Error!");

    Serial.println("DHT11 read failed");

  } else {

    // Temperature
    display.setTextSize(1);
    display.setCursor(5, 20);
    display.println("TEMPERATURE");

    display.setTextSize(2);
    display.setCursor(5, 32);
    display.print(temperature, 1);
    display.write(247);
    display.print("C");

    // Humidity
    display.setTextSize(1);
    display.setCursor(5, 49);
    display.print("HUMIDITY: ");

    display.print(humidity, 0);
    display.print(" %");

    // Serial monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" C | Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }

  display.display();

  // DHT11 reading interval
  delay(2000);
}
