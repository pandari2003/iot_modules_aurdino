/* ARDUINO MEGA ROBOT module code
Arduino Mega    —    Robot Controller
E01-ML01DP5      →    Arduino Mega
E01-ML01DP5	        Arduino Mega
VCC	                3.3V
GND	                GND
CE	                D48
CSN               	D49
SCK               	D52
MOSI	              D51
MISO	              D50
IRQ               	Not connected
Mega logic-level warning
Arduino Mega uses 5 V logic, while the nRF24/E01 module uses 3.3 V logic.
For reliable operation, use a 5 V → 3.3 V level shifter on:

D48 → CE
D49 → CSN
D52 → SCK
D51 → MOSI
MISO from the module → Mega D50 is 3.3 V and is normally read by the Mega.
Also use a stable 3.3 V supply for the PA/LNA module.

 Arduino Mega → PCA9685
PCA9685       	Mega
VCC	            5V
GND            	GND
SDA           	D20
SCL	            D21
V+	            External 5–6 V servo supply

Do not power multiple servos from the Mega 5 V pin.

PCA9685 servo channels
Servo	PCA9685        Channel
servoAY11	            CH0
servoAY1	            CH1
servoBX1	            CH2
servoP1	              CH3
servoCY2            	CH4
servoDX2	            CH5
servoP2	              CH6
 Arduino Mega → DRV8833
DRV8833	              Mega
AIN1	                D6
AIN2	                D7
BIN1	                D8
BIN2	                D9
GND	Mega              GND
VM	Motor battery +
AOUT1/AOUT2         	Motor 1
BOUT1/BOUT2	          Motor 2
  =========================================================
  ARDUINO MEGA ROBOT / RECEIVER
  PCA9685 + DRV8833 + nRF24L01+
  =========================================================
  Receives:
    AB X  -> Speed
    AB Y  -> Servo control
    CD X  -> Forward / Backward
    CD Y  -> Rotation
    Pot 1 -> P1
    Pot 2 -> P2
    Button -> Servo / Driving mode
  =========================================================
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// =========================================================
// nRF24
// =========================================================

const int CE_PIN  = 48;
const int CSN_PIN = 49;

RF24 radio(CE_PIN, CSN_PIN);

const byte address[6] = "ROVER";

// =========================================================
// PCA9685
// =========================================================

Adafruit_PWMServoDriver pwm =
  Adafruit_PWMServoDriver(0x40);

// =========================================================
// PCA9685 SERVO SETTINGS
// =========================================================

const int SERVOMIN = 150;
const int SERVOMAX = 600;

// =========================================================
// SERVO VARIABLES
// =========================================================

int servoAY1;
int servoAY11;
int servoBX1;
int servoP1;

int servoCY2;
int servoDX2;
int servoP2;

// =========================================================
// PCA9685 CHANNELS
// =========================================================

const int servoAY11Channel = 0;
const int servoAY1Channel  = 1;
const int servoBX1Channel  = 2;
const int servoP1Channel   = 3;

const int servoCY2Channel  = 4;
const int servoDX2Channel  = 5;
const int servoP2Channel   = 6;

// =========================================================
// MOTOR PINS
// =========================================================

const int motor1_IN1 = 6;
const int motor1_IN2 = 7;

const int motor2_IN3 = 8;
const int motor2_IN4 = 9;

// =========================================================
// ORIGINAL SERVO CONTROL ANGLES
// =========================================================

int ABxAngle = 90;
int AByAngle = 90;

int CDxAngle = 90;
int CDyAngle = 90;

// =========================================================
// DRIVING SPEED
// =========================================================

int speedValue = 50;

// =========================================================
// JOYSTICK SETTINGS
// =========================================================

const int center = 512;
const int deadZone = 50;

const int servoMoveDelay = 30;

// =========================================================
// RECEIVED DATA
// =========================================================

struct ControlData {

  int16_t abX;
  int16_t abY;

  int16_t cdX;
  int16_t cdY;

  int16_t pot1;
  int16_t pot2;

  uint8_t button;
};

ControlData data;

// =========================================================
// COMMUNICATION STATUS
// =========================================================

unsigned long lastPacketTime = 0;

const unsigned long radioTimeout = 1000;

// =========================================================
// SERVO FUNCTION
// =========================================================

void writeServo(int channel, int angle) {

  angle = constrain(angle, 0, 180);

  int pulse =
    map(angle, 0, 180, SERVOMIN, SERVOMAX);

  pwm.setPWM(channel, 0, pulse);
}

// =========================================================
// JOYSTICK FUNCTION
// =========================================================

void controlJoystickValue(int value, int &angle) {

  if (value > center + deadZone) {

    angle++;

    angle = constrain(angle, 0, 180);

    delay(servoMoveDelay);

  }

  else if (value < center - deadZone) {

    angle--;

    angle = constrain(angle, 0, 180);

    delay(servoMoveDelay);
  }
}

// =========================================================
// MOVE ALL SERVOS SLOWLY TO 90 DEGREES
// =========================================================
//
// Every servo moves 1 degree per step.
// Delay = 30 ms.
//
// Example:
// 150 -> 149 -> 148 -> ... -> 90
//
// All seven servos are updated during every step.
// =========================================================

void moveAllServosTo90Slowly() {

  bool moving = true;

  while (moving) {

    moving = false;

    // -----------------------------------------------------
    // AY1
    // -----------------------------------------------------

    if (servoAY1 != 90) {

      servoAY1 +=
        (servoAY1 < 90) ? 1 : -1;

      writeServo(
        servoAY1Channel,
        servoAY1
      );

      moving = true;
    }

    // -----------------------------------------------------
    // AY11
    // -----------------------------------------------------

    if (servoAY11 != 90) {

      servoAY11 +=
        (servoAY11 < 90) ? 1 : -1;

      writeServo(
        servoAY11Channel,
        servoAY11
      );

      moving = true;
    }

    // -----------------------------------------------------
    // BX1
    // -----------------------------------------------------

    if (servoBX1 != 90) {

      servoBX1 +=
        (servoBX1 < 90) ? 1 : -1;

      writeServo(
        servoBX1Channel,
        servoBX1
      );

      moving = true;
    }

    // -----------------------------------------------------
    // P1
    // -----------------------------------------------------

    if (servoP1 != 90) {

      servoP1 +=
        (servoP1 < 90) ? 1 : -1;

      writeServo(
        servoP1Channel,
        servoP1
      );

      moving = true;
    }

    // -----------------------------------------------------
    // CY2
    // -----------------------------------------------------

    if (servoCY2 != 90) {

      servoCY2 +=
        (servoCY2 < 90) ? 1 : -1;

      writeServo(
        servoCY2Channel,
        servoCY2
      );

      moving = true;
    }

    // -----------------------------------------------------
    // DX2
    // -----------------------------------------------------

    if (servoDX2 != 90) {

      servoDX2 +=
        (servoDX2 < 90) ? 1 : -1;

      writeServo(
        servoDX2Channel,
        servoDX2
      );

      moving = true;
    }

    // -----------------------------------------------------
    // P2
    // -----------------------------------------------------

    if (servoP2 != 90) {

      servoP2 +=
        (servoP2 < 90) ? 1 : -1;

      writeServo(
        servoP2Channel,
        servoP2
      );

      moving = true;
    }

    // -----------------------------------------------------
    // DELAY
    // -----------------------------------------------------

    if (moving) {

      delay(servoMoveDelay);
    }
  }
}

// =========================================================
// STOP MOTORS
// =========================================================

void stopMotors() {

  digitalWrite(motor1_IN1, LOW);
  digitalWrite(motor1_IN2, LOW);

  digitalWrite(motor2_IN3, LOW);
  digitalWrite(motor2_IN4, LOW);
}

// =========================================================
// SETUP
// =========================================================

void setup() {

  Serial.begin(115200);

  // =======================================================
  // PCA9685 START
  // =======================================================

  pwm.begin();

  pwm.setOscillatorFrequency(27000000);

  pwm.setPWMFreq(50);

  delay(10);

  // =======================================================
  // START ALL SERVOS AT 90 DEGREES
  // =======================================================

  servoAY1  = 90;
  servoAY11 = 90;
  servoBX1  = 90;
  servoP1   = 90;

  servoCY2  = 90;
  servoDX2  = 90;
  servoP2   = 90;

  writeServo(
    servoAY1Channel,
    servoAY1
  );

  writeServo(
    servoAY11Channel,
    servoAY11
  );

  writeServo(
    servoBX1Channel,
    servoBX1
  );

  writeServo(
    servoP1Channel,
    servoP1
  );

  writeServo(
    servoCY2Channel,
    servoCY2
  );

  writeServo(
    servoDX2Channel,
    servoDX2
  );

  writeServo(
    servoP2Channel,
    servoP2
  );

  // =======================================================
  // MOTOR SETUP
  // =======================================================

  pinMode(motor1_IN1, OUTPUT);
  pinMode(motor1_IN2, OUTPUT);

  pinMode(motor2_IN3, OUTPUT);
  pinMode(motor2_IN4, OUTPUT);

  stopMotors();

  // =======================================================
  // nRF24 START
  // =======================================================

  if (!radio.begin()) {

    Serial.println("nRF24 receiver not detected!");

    while (1) {
      delay(1000);
    }
  }

  radio.setPALevel(RF24_PA_HIGH);

  radio.setDataRate(RF24_250KBPS);

  radio.setChannel(108);

  radio.openReadingPipe(1, address);

  radio.startListening();

  Serial.println("MEGA ROBOT RECEIVER READY");
}

// =========================================================
// MAIN LOOP
// =========================================================

void loop() {

  // =======================================================
  // RECEIVE WIRELESS DATA
  // =======================================================

  if (radio.available()) {

    radio.read(
      &data,
      sizeof(data)
    );

    lastPacketTime = millis();
  }

  // =======================================================
  // COMMUNICATION FAIL-SAFE
  // =======================================================

  if (millis() - lastPacketTime > radioTimeout) {

    stopMotors();

    // Keep robot servos at safe position
    // after communication is lost.

    servoAY1  = 90;
    servoAY11 = 90;
    servoBX1  = 90;
    servoP1   = 90;
    servoCY2  = 90;
    servoDX2  = 90;
    servoP2   = 90;

    writeServo(servoAY1Channel, 90);
    writeServo(servoAY11Channel, 90);
    writeServo(servoBX1Channel, 90);
    writeServo(servoP1Channel, 90);
    writeServo(servoCY2Channel, 90);
    writeServo(servoDX2Channel, 90);
    writeServo(servoP2Channel, 90);

    return;
  }

  // =======================================================
  // BUTTON ON = SERVO / WORKING MODE
  // =======================================================

  if (data.button == LOW) {

    // -----------------------------------------------------
    // STOP DC MOTORS IMMEDIATELY
    // -----------------------------------------------------

    stopMotors();

    // -----------------------------------------------------
    // JOYSTICK CONTROL
    // -----------------------------------------------------

    controlJoystickValue(
      data.abX,
      ABxAngle
    );

    controlJoystickValue(
      data.abY,
      AByAngle
    );

    controlJoystickValue(
      data.cdX,
      CDxAngle
    );

    controlJoystickValue(
      data.cdY,
      CDyAngle
    );

    // -----------------------------------------------------
    // AB X -> AY1 / AY11
    // -----------------------------------------------------

    int ayAngle =
      map(
        ABxAngle,
        0,
        180,
        0,
        140
      );

    ayAngle =
      constrain(
        ayAngle,
        0,
        140
      );

    servoAY1 = ayAngle;

    servoAY11 =
      140 - ayAngle;

    // -----------------------------------------------------
    // AB Y -> BX1
    // -----------------------------------------------------

    servoBX1 = AByAngle;

    // -----------------------------------------------------
    // CD X -> CY2
    // -----------------------------------------------------

    servoCY2 = CDxAngle;

    // -----------------------------------------------------
    // CD Y -> DX2
    // -----------------------------------------------------

    servoDX2 = CDyAngle;

    // -----------------------------------------------------
    // WRITE JOYSTICK SERVOS
    // -----------------------------------------------------

    writeServo(
      servoAY1Channel,
      servoAY1
    );

    writeServo(
      servoAY11Channel,
      servoAY11
    );

    writeServo(
      servoBX1Channel,
      servoBX1
    );

    writeServo(
      servoCY2Channel,
      servoCY2
    );

    writeServo(
      servoDX2Channel,
      servoDX2
    );

    // =====================================================
    // POTENTIOMETER 1
    // =====================================================

    int pot1Angle =
      map(
        data.pot1,
        0,
        1023,
        0,
        180
      );

    if (servoP1 < pot1Angle) {

      servoP1++;

      writeServo(
        servoP1Channel,
        servoP1
      );

      delay(servoMoveDelay);
    }

    else if (servoP1 > pot1Angle) {

      servoP1--;

      writeServo(
        servoP1Channel,
        servoP1
      );

      delay(servoMoveDelay);
    }

    // =====================================================
    // POTENTIOMETER 2
    // =====================================================

    int pot2Angle =
      map(
        data.pot2,
        0,
        1023,
        0,
        180
      );

    if (servoP2 < pot2Angle) {

      servoP2++;

      writeServo(
        servoP2Channel,
        servoP2
      );

      delay(servoMoveDelay);
    }

    else if (servoP2 > pot2Angle) {

      servoP2--;

      writeServo(
        servoP2Channel,
        servoP2
      );

      delay(servoMoveDelay);
    }

    // =====================================================
    // SERIAL MONITOR
    // =====================================================

    Serial.print("SERVO MODE | ");

    Serial.print("AB X: ");
    Serial.print(ABxAngle);

    Serial.print("  AB Y: ");
    Serial.print(AByAngle);

    Serial.print("  Pot1: ");
    Serial.print(pot1Angle);

    Serial.print("  |  CD X: ");
    Serial.print(CDxAngle);

    Serial.print("  CD Y: ");
    Serial.print(CDyAngle);

    Serial.print("  Pot2: ");
    Serial.println(pot2Angle);
  }

  // =======================================================
  // BUTTON OFF = DRIVING MODE
  // =======================================================

  else {

    Serial.print("DRIVING MODE | ");

    // -----------------------------------------------------
    // IMPORTANT:
    // Do NOT instantly set servos to 90.
    //
    // Move them slowly from their current positions.
    // -----------------------------------------------------

    moveAllServosTo90Slowly();

    // =====================================================
    // READ JOYSTICKS
    // =====================================================

    // AB joystick X = SPEED

    int abXValue = data.abX;

    // CD joystick X = FORWARD / BACKWARD

    int cdXValue = data.cdX;

    // CD joystick Y = LEFT / RIGHT ROTATION

    int cdYValue = data.cdY;

    // =====================================================
    // SPEED CONTROL
    // =====================================================

    if (abXValue > center + deadZone) {

      speedValue++;
    }

    else if (abXValue < center - deadZone) {

      speedValue--;
    }

    // -----------------------------------------------------
    // Limit speed 0 to 100%
    // -----------------------------------------------------

    speedValue =
      constrain(
        speedValue,
        0,
        100
      );

    // =====================================================
    // CONVERT PERCENTAGE TO PWM
    // =====================================================

    int baseSpeed =
      map(
        speedValue,
        0,
        100,
        0,
        255
      );

    // =====================================================
    // MOTOR SPEED VARIABLES
    // =====================================================

    int leftSpeed  = baseSpeed;
    int rightSpeed = baseSpeed;

    // =====================================================
    // FORWARD
    // CD X+
    // =====================================================

    if (cdXValue > center + deadZone) {

      leftSpeed  = baseSpeed;
      rightSpeed = baseSpeed;

      // ---------------------------------------------------
      // LEFT MOTOR FORWARD
      // ---------------------------------------------------

      analogWrite(
        motor1_IN1,
        leftSpeed
      );

      digitalWrite(
        motor1_IN2,
        LOW
      );

      // ---------------------------------------------------
      // RIGHT MOTOR FORWARD
      // ---------------------------------------------------

      analogWrite(
        motor2_IN3,
        rightSpeed
      );

      digitalWrite(
        motor2_IN4,
        LOW
      );

      Serial.print("FORWARD ");
    }

    // =====================================================
    // BACKWARD
    // CD X-
    // =====================================================

    else if (cdXValue < center - deadZone) {

      leftSpeed  = baseSpeed;
      rightSpeed = baseSpeed;

      // ---------------------------------------------------
      // LEFT MOTOR BACKWARD
      // ---------------------------------------------------

      digitalWrite(
        motor1_IN1,
        LOW
      );

      analogWrite(
        motor1_IN2,
        leftSpeed
      );

      // ---------------------------------------------------
      // RIGHT MOTOR BACKWARD
      // ---------------------------------------------------

      digitalWrite(
        motor2_IN3,
        LOW
      );

      analogWrite(
        motor2_IN4,
        rightSpeed
      );

      Serial.print("BACKWARD ");
    }

    // =====================================================
    // CD X CENTER
    // CHECK ROTATION
    // =====================================================

    else {

      // ===================================================
      // RIGHT / CLOCKWISE
      // CD Y+
      // ===================================================

      if (cdYValue > center + deadZone) {

        leftSpeed  = baseSpeed;
        rightSpeed = baseSpeed;

        // -------------------------------------------------
        // LEFT MOTOR FORWARD
        // -------------------------------------------------

        analogWrite(
          motor1_IN1,
          leftSpeed
        );

        digitalWrite(
          motor1_IN2,
          LOW
        );

        // -------------------------------------------------
        // RIGHT MOTOR REVERSE
        // -------------------------------------------------

        digitalWrite(
          motor2_IN3,
          LOW
        );

        analogWrite(
          motor2_IN4,
          rightSpeed
        );

        Serial.print("CLOCKWISE ");
      }

      // ===================================================
      // LEFT / ANTI-CLOCKWISE
      // CD Y-
      // ===================================================

      else if (cdYValue < center - deadZone) {

        leftSpeed  = baseSpeed;
        rightSpeed = baseSpeed;

        // -------------------------------------------------
        // LEFT MOTOR REVERSE
        // -------------------------------------------------

        digitalWrite(
          motor1_IN1,
          LOW
        );

        analogWrite(
          motor1_IN2,
          leftSpeed
        );

        // -------------------------------------------------
        // RIGHT MOTOR FORWARD
        // -------------------------------------------------

        analogWrite(
          motor2_IN3,
          rightSpeed
        );

        digitalWrite(
          motor2_IN4,
          LOW
        );

        Serial.print("ANTI-CLOCKWISE ");
      }

      // ===================================================
      // CD Y CENTER = STOP
      // ===================================================

      else {

        stopMotors();

        Serial.print("STOP ");
      }
    }

    // =====================================================
    // SERIAL MONITOR
    // =====================================================

    Serial.print("Speed: ");
    Serial.print(speedValue);
    Serial.print("%");

    Serial.print("  Base PWM: ");
    Serial.print(baseSpeed);

    Serial.print("  Left PWM: ");
    Serial.print(leftSpeed);

    Serial.print("  Right PWM: ");
    Serial.println(rightSpeed);
  }
}