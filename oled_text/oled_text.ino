#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
void setup() {

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
while(1){
  for(int i = 0; i <= 10; i++) {
    display.clearDisplay();
    display.setCursor(10, i);
    display.println("WELCOME");
    display.println(" PANDARI");
    display.println("JAI BHEEM");
    display.display();
    delay(100);
  }
  display.clearDisplay();
  delay(100);
}
  
}

void loop() {}