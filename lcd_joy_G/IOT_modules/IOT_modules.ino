#include "led.c"

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  ledinit();
}

// the loop function runs over and over again forever
void loop() {
  ledlogic();                      // wait for a second
}
