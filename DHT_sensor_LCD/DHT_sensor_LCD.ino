
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ================= PIN CONFIGURATION =================

#define DHT_PIN 4
#define DHT_TYPE DHT11

#define LCD_SDA 8
#define LCD_SCL 9

// ================= LCD CONFIGURATION =================

#define LCD_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

DHT dht(DHT_PIN, DHT_TYPE);

// ================= DEGREE SYMBOL =================

byte degreeSymbol[8] = {
  B00110,
  B01001,
  B01001,
  B00110,
  B00000,
  B00000,
  B00000,
  B00000
};

// ================= SETUP =================

void setup() {

  Serial.begin(115200);

  // Initialize I2C
  Wire.begin(LCD_SDA, LCD_SCL);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, degreeSymbol);

  // Initialize DHT11
  dht.begin();

  // Startup screen
  lcd.clear();

  lcd.setCursor(1, 0);
  lcd.print("ENVIRONMENT");

  lcd.setCursor(3, 1);
  lcd.print("MONITOR");

  delay(2000);

  lcd.clear();
}

// ================= MAIN LOOP =================

void loop() {

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {

    lcd.setCursor(0, 0);
    lcd.print("SENSOR ERROR    ");

    lcd.setCursor(0, 1);
    lcd.print("CHECK DHT11     ");

    Serial.println("DHT11 Sensor Error");

  } 
  else {

    // Display temperature
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");

    lcd.print(temperature, 1);
    lcd.write(byte(0));
    lcd.print("C   ");

    // Display humidity
    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");

    lcd.print(humidity, 0);
    lcd.print("%   ");

    // Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temperature, 1);
    Serial.print(" C | Humidity: ");
    Serial.print(humidity, 0);
    Serial.println(" %");
  }

  delay(2000);
}
