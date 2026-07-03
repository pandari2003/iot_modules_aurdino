#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int potPin = A0;
int potValue = 0;

void setup() {
  lcd.init();
  lcd.backlight();
}

void loop() {
  potValue = analogRead(potPin);   // reads 0–1023

  lcd.setCursor(0, 0);
  lcd.print("Pot Value:");

  lcd.setCursor(0, 1);
  lcd.print(potValue);
  lcd.print("     "); // clears old digits

  delay(200);
}