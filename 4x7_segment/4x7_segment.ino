#include <Arduino.h>
#include <TM1637Display.h>

// Define the connections pins
#define CLK 5
#define DIO 4

// Create display object
TM1637Display display(CLK, DIO);

void setup() {
  // Set the brightness (0 to 7)
  display.setBrightness(0x0a); // Maximum brightness
}

void loop() {
  for (int i = 0; i < 10000; i++) {
    display.showNumberDec(i); // Display the number
    delay(1);               // Wait 100 milliseconds
  }
}
