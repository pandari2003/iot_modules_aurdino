#include <Servo.h>

Servo servoB1;
Servo servoB2;
Servo servoB3;
Servo servoB4;

Servo servoH1;
Servo servoH2;
Servo servoH3;
Servo servoH4;

Servo servoKN1;
Servo servoKN2;
Servo servoKN3;
Servo servoKN4;

const int servoB1pin = 2;  //front right (servo 1)
const int servoB2pin = 5;  //front left (servo 2)
const int servoB3pin = 8;  //back right (servo 4)
const int servoB4pin = 11;  //back left (servo 3)

const int servoH1Pin = 3;  // Front Right (servo 5)
const int servoH2Pin = 6;  // front left(servo 8)
const int servoH3Pin = 9;  // back right (servo 6)
const int servoH4Pin = 12;  // back Left(servo 7)


const int servoKN1Pin = 4;  // Front Right (servo 9)
const int servoKN2Pin = 7;  // front left (servo 12)
const int servoKN3Pin = 10;  // back right (servo 10)
const int servoKN4Pin = 13;  // back Left (servo 11)



/******************************/
void forword() {
  for (int i = 0; i <= 10; i++) {

    // Leg1 front right
      for(int x=110; x > 80; x-=5){
        servoH4.write(x);
    }
    for (int j = 90; j < 150; j += 5) {
      servoKN1.write(j);
      delay(40);
    }
    for (int k = 90; k < 130; k += 5) {
      servoH1.write(k);
      delay(40);
    }
    for (int j = 150; j > 90; j -= 5) {
      servoKN1.write(j);
      delay(40);
    }
    for (int k = 130; k > 70; k -= 5) {
      
      delay(40);
    }
  
    delay(100);

    //leg2 back right
      for(int x=110; x > 80; x-=5){
        servoH2.write(x);
    }
      for (int j = 90; j < 150; j += 5) {
      servoKN3.write(j);
      delay(40);
    }
    for (int k = 90; k < 130; k += 5) {
      servoH3.write(k);
      delay(40);
    }
    for (int j = 150; j > 90; j -= 5) {
      servoKN3.write(j);
      delay(40);
    }
    for (int k = 130; k > 70; k -= 5) {
      servoH3.write(k);
      delay(40);
    }
    delay(100);

    //leg3 front left
      for(int x=70; x < 110; x+=5){
        servoH3.write(x);
    }
    for (int j = 90; j > 30; j -= 5) {
      servoKN2.write(j);
      delay(40);
    }
    for (int k = 90; k > 50; k -= 5) {
      servoH2.write(k);
      delay(40);
    }
    for (int j = 30; j < 90; j += 5) {
      servoKN2.write(j);
      delay(40);
    }
    for (int k = 50; k < 110; k += 5) {
      servoH2.write(k);
      delay(40);
    }
    delay(100);

    //leg4 back left
    for(int x=70; x < 110; x+=5){
        servoH1.write(x);
    }
    for (int j = 90; j > 30; j -= 5) {
      servoKN4.write(j);
      delay(40);
    }
    for (int k = 90; k > 50; k -= 5) {
      servoH4.write(k);
      delay(40);
    }
    for (int j = 30; j < 90; j += 5) {
      servoKN4.write(j);
      delay(40);
    }
    for (int k = 50; k < 110; k += 5) {
      servoH4.write(k);
      delay(40);
    }
    delay(100);
  }
}
//servoB1.write(jk);
//servoH1.write(i);
//servoKN1.write(k);
/******************************/

void setup() {
  Serial.begin(115200);

  servoH1.attach(servoH1Pin);
  servoH2.attach(servoH2Pin);
  servoH3.attach(servoH3Pin);
  servoH4.attach(servoH4Pin);

  servoKN1.attach(servoKN1Pin);
  servoKN2.attach(servoKN2Pin);
  servoKN3.attach(servoKN3Pin);
  servoKN4.attach(servoKN4Pin);

  servoB1.attach(servoB1pin);
  servoB2.attach(servoB2pin);
  servoB3.attach(servoB3pin);
  servoB4.attach(servoB4pin);

  Serial.println("Quadruped Robot Started");
}


void loop(){
  forword();
}


