// Segment pins connected to Arduino
const int segPins[] = {2, 3, 4, 5, 6, 7, 12}; // a, b, c, d, e, f, g

// Hexadecimal encoding for Common Cathode digits 0-9
// Bit 0 = segA, Bit 1 = segB ... Bit 6 = segG
const byte hexDigits[] = {
  0x3F, // 0 (00111111)
  0x06, // 1 (00000110)
  0x5B, // 2 (01011011)
  0x4F, // 3 (01001111)
  0x66, // 4 (01100110)
  0x6D, // 5 (01101101)
  0x7D, // 6 (01111101)
  0x07, // 7 (00000111)
  0x7F, // 8 (01111111)
  0x6F  // 9 (01101111)
};

/*
// Hexadecimal encoding for Common Anode digits 0-9
// Bit 0 = segA, Bit 1 = segB ... Bit 6 = segG
// 0 = ON, 1 = OFF
const byte hexDigits[] = {
  0x40, // 0 (01000000)
  0x79, // 1 (01111001)
  0x24, // 2 (00100100)
  0x30, // 3 (00110000)
  0x19, // 4 (00011001)
  0x12, // 5 (00010010)
  0x02, // 6 (00000010)
  0x78, // 7 (01111000)
  0x00, // 8 (00000000)
  0x10  // 9 (00010000)
};

*/

void setup() {
  // Set all segment pins as OUTPUT
  for (int i = 0; i <= 7; i++) {
    pinMode(segPins[i], OUTPUT);
  }
}

void displayDigit(int digit) {
  byte mask = hexDigits[digit];
  // Extract each bit and write to the corresponding pin
  for (int i = 0; i <= 7; i++) {
    digitalWrite(segPins[i], bitRead(mask, i));
  }
}

void loop() {
  for (int i = 0; i < 10; i++) {
    displayDigit(i);
    delay(1000); // 1-second delay
  }
}
