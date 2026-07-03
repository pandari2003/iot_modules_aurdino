#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define MOISTURE 3
#define MOTOR 4
#define FAN 5
#define EFAN 6
#define BUZZER 7

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();
  pinMode(MOISTURE,OUTPUT);
  pinMode(MOTOR,OUTPUT);
  pinMode(FAN,OUTPUT);
  pinMode(EFAN,OUTPUT);
  pinMode(BUZZER,OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  float humidity = dht.readHumidity();
  float temp = dht.readTemperature();
  float moisture = analogRead(0);

  lcd.setCursor(0, 0);
  lcd.print("humidity :");
  lcd.setCursor(0, 1);
  lcd.print(humidity);

  lcd.setCursor(0, 0);
  lcd.print("temparature :");
  lcd.setCursor(0, 1);
  lcd.print(temp);
  delay(10);
  lcd.print("     "); // clears old digits

  // temprature worning in lcd and on fan
  if(temp>35){
    digitalWrite(FAN, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("FAN ON ");
    for(int i=0; i<3; i++){
      tone(BUZZER, 1000);   // 1 kHz sound
      delay(2);
    }

  }
  else{
    digitalWrite(FAN, LOW);
    lcd.setCursor(0, 0);
    lcd.print("FAN OFF ");
    noTone(BUZZER);
  }

// humidity worning in lcd and on exast air fan
  if(humidity<25){
    digitalWrite(EFAN, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("FAN ON ");
    for(int i=0; i<3; i++){
      tone(BUZZER, 1000);   // 1 kHz sound
      delay(4);
    }
  }
  else{
    digitalWrite(EFAN, LOW);
    lcd.setCursor(0, 0);
    lcd.print("FAN ON ");
    noTone(BUZZER);
  }

// moisture worning in lcd and on on motor
  if(moisture<512){
    digitalWrite(MOTOR, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("FAN ON ");
    for(int i=0; i<3; i++){
      tone(BUZZER, 1000);   // 1 kHz sound
      delay(4);
    }
  }
  else{
    digitalWrite(MOTOR, LOW);
    lcd.setCursor(0, 0);
    lcd.print("FAN ON "); 
    noTone(BUZZER);   
  }

}