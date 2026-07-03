#include <LedControl.h>

LedControl lc = LedControl(12, 11, 10, 1); // DIN, CLK, CS, 1 module

void setup() {
  lc.shutdown(0, false);   // Wake up display
  lc.setIntensity(0, 8);   // Brightness (0–15)
  lc.clearDisplay(0);
}
byte smiley[8] = {
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10100101,
  B10011001,
  B01000010,
  B00111100
};
byte heart[8] = {
  B00000000,
  B01100110,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000
};
byte arrow[8] = {
  B00011000,
  B00011000,
  B00011000,
  B11111111,
  B01111110,
  B00111100,
  B00011000,
  B00000000
};

void loop() {
  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, heart[i]);
  }
  delay(1000);

  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, smiley[i]);
  }
  delay(1000);

  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, arrow[i]);
  }
  delay(1000);
}