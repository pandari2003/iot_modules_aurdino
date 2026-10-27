#include <WiFiS3.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

//====================================================
//                 WIFI
//====================================================

char ssid[] = "DogRobot";
char pass[] = "12345678";

WiFiServer server(80);

//====================================================
//              PCA9685 SERVO DRIVER
//====================================================

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// PCA9685 servo settings
const int SERVOMIN = 150;
const int SERVOMAX = 600;
const int SERVO_FREQ = 50;

//====================================================
//        PCA9685 SERVO COMPATIBILITY CLASS
//====================================================
// This keeps your original .write() logic unchanged.
// Instead of Arduino Servo pins, PCA9685 channels
// are used.

class PCAServo {
  private:
    int channel;

  public:

    void attach(int ch) {
      channel = ch;
    }

    void write(int angle) {

      angle = constrain(angle, 0, 180);

      int pulse = map(
        angle,
        0,
        180,
        SERVOMIN,
        SERVOMAX
      );

      pwm.setPWM(channel, 0, pulse);
    }
};

//--------------- Servos ---------------//

PCAServo hipFL;
PCAServo hipFR;
PCAServo hipBL;
PCAServo hipBR;

PCAServo kneeFL;
PCAServo kneeFR;
PCAServo kneeBL;
PCAServo kneeBR;

//====================================================
//             PCA9685 CHANNELS
//====================================================

const int HIP_FL_CH  = 0;
const int KNEE_FL_CH = 1;

const int HIP_FR_CH  = 2;
const int KNEE_FR_CH = 3;

const int HIP_BL_CH  = 4;
const int KNEE_BL_CH = 5;

const int HIP_BR_CH  = 6;
const int KNEE_BR_CH = 7;

//====================================================
//          CURRENT SERVO POSITIONS
//====================================================

int hipFLPos = 90;
int hipFRPos = 90;
int hipBLPos = 90;
int hipBRPos = 90;

int kneeFLPos = 90;
int kneeFRPos = 90;
int kneeBLPos = 90;
int kneeBRPos = 90;

//====================================================
//             WALKING PARAMETERS
//====================================================

// Left legs
const int HIP_LEFT_FORWARD = 70;
const int HIP_LEFT_BACK    = 110;

// Right legs (mirror)
const int HIP_RIGHT_FORWARD = 110;
const int HIP_RIGHT_BACK    = 70;

const int KNEE_UP   = 60;
const int KNEE_DOWN = 90;

const int STEP_DELAY = 15;

//====================================================
//              SMOOTH MOVE
//====================================================

void moveServo(PCAServo &servo, int &current, int target)
{
  if (current < target)
  {
    while (current < target)
    {
      current++;
      servo.write(current);
      delay(15);
    }
  }
  else
  {
    while (current > target)
    {
      current--;
      servo.write(current);
      delay(15);
    }
  }
}

//====================================================
//                 MOVE LEG
//====================================================

void moveLeg(
  PCAServo &hip,
  PCAServo &knee,
  int &hipPos,
  int &kneePos,
  int hipTarget,
  int kneeTarget)
{
  while (hipPos != hipTarget || kneePos != kneeTarget)
  {
    if (hipPos < hipTarget) hipPos++;
    if (hipPos > hipTarget) hipPos--;

    if (kneePos < kneeTarget) kneePos++;
    if (kneePos > kneeTarget) kneePos--;

    hip.write(hipPos);
    knee.write(kneePos);

    delay(STEP_DELAY);
  }
}

//====================================================
//              MOVE TWO LEGS
//====================================================

void moveTwoLegs(
  PCAServo &hip1,
  PCAServo &knee1,
  int &hipPos1,
  int &kneePos1,
  int hipTarget1,
  int kneeTarget1,

  PCAServo &hip2,
  PCAServo &knee2,
  int &hipPos2,
  int &kneePos2,
  int hipTarget2,
  int kneeTarget2)
{
  while (
    hipPos1 != hipTarget1 ||
    kneePos1 != kneeTarget1 ||
    hipPos2 != hipTarget2 ||
    kneePos2 != kneeTarget2)
  {
    if (hipPos1 < hipTarget1) hipPos1++;
    if (hipPos1 > hipTarget1) hipPos1--;

    if (kneePos1 < kneeTarget1) kneePos1++;
    if (kneePos1 > kneeTarget1) kneePos1--;

    if (hipPos2 < hipTarget2) hipPos2++;
    if (hipPos2 > hipTarget2) hipPos2--;

    if (kneePos2 < kneeTarget2) kneePos2++;
    if (kneePos2 > kneeTarget2) kneePos2--;

    hip1.write(hipPos1);
    knee1.write(kneePos1);

    hip2.write(hipPos2);
    knee2.write(kneePos2);

    delay(STEP_DELAY);
  }
}

//====================================================
//                 FORWARD WALK
//====================================================

void walkCycle()
{
  //===========================
  // Phase 1
  // Front Left + Back Right
  //===========================

  // Lift both legs
  moveTwoLegs(
    hipFL, kneeFL,
    hipFLPos, kneeFLPos,
    hipFLPos, KNEE_UP,

    hipBR, kneeBR,
    hipBRPos, kneeBRPos,
    hipBRPos, KNEE_UP
  );

  // Swing forward
  moveTwoLegs(
    hipFL, kneeFL,
    hipFLPos, kneeFLPos,
    HIP_LEFT_FORWARD, KNEE_UP,

    hipBR, kneeBR,
    hipBRPos, kneeBRPos,
    HIP_RIGHT_FORWARD, KNEE_UP
  );

  // Place on ground
  moveTwoLegs(
    hipFL, kneeFL,
    hipFLPos, kneeFLPos,
    HIP_LEFT_FORWARD, KNEE_DOWN,

    hipBR, kneeBR,
    hipBRPos, kneeBRPos,
    HIP_RIGHT_FORWARD, KNEE_DOWN
  );

  //===========================
  // Phase 2
  // Front Right + Back Left
  //===========================

  moveTwoLegs(
    hipFR, kneeFR,
    hipFRPos, kneeFRPos,
    hipFRPos, KNEE_UP,

    hipBL, kneeBL,
    hipBLPos, kneeBLPos,
    hipBLPos, KNEE_UP
  );

  moveTwoLegs(
    hipFR, kneeFR,
    hipFRPos, kneeFRPos,
    HIP_RIGHT_FORWARD, KNEE_UP,

    hipBL, kneeBL,
    hipBLPos, kneeBLPos,
    HIP_LEFT_FORWARD, KNEE_UP
  );

  moveTwoLegs(
    hipFR, kneeFR,
    hipFRPos, kneeFRPos,
    HIP_RIGHT_FORWARD, KNEE_DOWN,

    hipBL, kneeBL,
    hipBLPos, kneeBLPos,
    HIP_LEFT_FORWARD, KNEE_DOWN
  );

  //===========================
  // Return to neutral
  //===========================

  moveTwoLegs(
    hipFL, kneeFL,
    hipFLPos, kneeFLPos,
    90, KNEE_DOWN,

    hipBR, kneeBR,
    hipBRPos, kneeBRPos,
    90, KNEE_DOWN
  );

  moveTwoLegs(
    hipFR, kneeFR,
    hipFRPos, kneeFRPos,
    90, KNEE_DOWN,

    hipBL, kneeBL,
    hipBLPos, kneeBLPos,
    90, KNEE_DOWN
  );
}

//====================================================
//                 BACKWARD WALK
//====================================================

void walkCycleBackward()
{
  //===========================
  // Phase 1
  // Front Left + Back Right
  //===========================

  moveTwoLegs(
    hipFL, kneeFL,
    hipFLPos, kneeFLPos,
    hipFLPos, KNEE_UP,

    hipBR, kneeBR,
    hipBRPos, kneeBRPos,
    hipBRPos, KNEE_UP
  );

  // Swing backward
  moveTwoLegs(
    hipFL, kneeFL,
    hipFLPos, kneeFLPos,
    HIP_LEFT_BACK, KNEE_UP,

    hipBR, kneeBR,
    hipBRPos, kneeBRPos,
    HIP_RIGHT_BACK, KNEE_UP
  );

  // Place on ground
  moveTwoLegs(
    hipFL, kneeFL,
    hipFLPos, kneeFLPos,
    HIP_LEFT_BACK, KNEE_DOWN,

    hipBR, kneeBR,
    hipBRPos, kneeBRPos,
    HIP_RIGHT_BACK, KNEE_DOWN
  );

  //===========================
  // Phase 2
  // Front Right + Back Left
  //===========================

  moveTwoLegs(
    hipFR, kneeFR,
    hipFRPos, kneeFRPos,
    hipFRPos, KNEE_UP,

    hipBL, kneeBL,
    hipBLPos, kneeBLPos,
    hipBLPos, KNEE_UP
  );

  moveTwoLegs(
    hipFR, kneeFR,
    hipFRPos, kneeFRPos,
    HIP_RIGHT_BACK, KNEE_UP,

    hipBL, kneeBL,
    hipBLPos, kneeBLPos,
    HIP_LEFT_BACK, KNEE_UP
  );

  moveTwoLegs(
    hipFR, kneeFR,
    hipFRPos, kneeFRPos,
    HIP_RIGHT_BACK, KNEE_DOWN,

    hipBL, kneeBL,
    hipBLPos, kneeBLPos,
    HIP_LEFT_BACK, KNEE_DOWN
  );

  // Return to stand
  moveTwoLegs(
    hipFL, kneeFL,
    hipFLPos, kneeFLPos,
    90, KNEE_DOWN,

    hipBR, kneeBR,
    hipBRPos, kneeBRPos,
    90, KNEE_DOWN
  );

  moveTwoLegs(
    hipFR, kneeFR,
    hipFRPos, kneeFRPos,
    90, KNEE_DOWN,

    hipBL, kneeBL,
    hipBLPos, kneeBLPos,
    90, KNEE_DOWN
  );
}

//====================================================
//                  STAND POSE
//====================================================

void standPose()
{
  while (
    hipFLPos != 90 ||
    hipFRPos != 90 ||
    hipBLPos != 90 ||
    hipBRPos != 90 ||
    kneeFLPos != 90 ||
    kneeFRPos != 90 ||
    kneeBLPos != 90 ||
    kneeBRPos != 90)
  {
    if (hipFLPos > 90) hipFLPos--;
    if (hipFRPos < 90) hipFRPos++;
    if (hipBLPos > 90) hipBLPos--;
    if (hipBRPos < 90) hipBRPos++;

    if (kneeFLPos > 90) kneeFLPos--;
    if (kneeFRPos < 90) kneeFRPos++;
    if (kneeBLPos > 90) kneeBLPos--;
    if (kneeBRPos < 90) kneeBRPos++;

    hipFL.write(hipFLPos);
    hipFR.write(hipFRPos);
    hipBL.write(hipBLPos);
    hipBR.write(hipBRPos);

    kneeFL.write(kneeFLPos);
    kneeFR.write(kneeFRPos);
    kneeBL.write(kneeBLPos);
    kneeBR.write(kneeBRPos);

    delay(20);
  }
}

//====================================================
//                   SIT POSE
//====================================================

void sitPose()
{
  while (
    hipFLPos != 140 ||
    hipFRPos != 50 ||
    hipBLPos != 140 ||
    hipBRPos != 50 ||
    kneeFLPos != 140 ||
    kneeFRPos != 50 ||
    kneeBLPos != 140 ||
    kneeBRPos != 50)
  {
    if (hipFLPos < 140) hipFLPos++;
    if (hipFRPos > 50) hipFRPos--;
    if (hipBLPos < 140) hipBLPos++;
    if (hipBRPos > 50) hipBRPos--;

    if (kneeFLPos < 140) kneeFLPos++;
    if (kneeFRPos > 50) kneeFRPos--;
    if (kneeBLPos < 140) kneeBLPos++;
    if (kneeBRPos > 50) kneeBRPos--;

    hipFL.write(hipFLPos);
    hipFR.write(hipFRPos);
    hipBL.write(hipBLPos);
    hipBR.write(hipBRPos);

    kneeFL.write(kneeFLPos);
    kneeFR.write(kneeFRPos);
    kneeBL.write(kneeBLPos);
    kneeBR.write(kneeBRPos);

    delay(20);
  }
}

//====================================================
//                 WALK FORWARD
//====================================================

void walkForward()
{
  // If the robot is sitting, stand up smoothly.
  standPose();

  // Walk forward.
  for (int i = 0; i < 5; i++)
  {
    walkCycle();
  }

  // Finish in standing position.
  standPose();
}

//====================================================
//                 WALK BACKWARD
//====================================================

void walkBackward()
{
  standPose();

  for (int i = 0; i < 5; i++)
  {
    walkCycleBackward();
  }

  standPose();
}

//====================================================
//                  WEB PAGE
//====================================================

String page()
{
  String html =
  "<!DOCTYPE html>"
  "<html>"
  "<head>"
  "<meta name='viewport' content='width=device-width, initial-scale=1'>"
  "<style>"
  "body{text-align:center;font-family:Arial;margin-top:40px;}"
  "button{width:180px;height:70px;font-size:24px;margin:20px;}"
  "</style>"
  "</head>"
  "<body>"
  "<h2>Dog Robot</h2>"
  "<a href='/stand'><button>STAND</button></a><br>"
  "<a href='/sit'><button>SIT</button></a>"
  "<a href='/forward'><button>FORWARD</button></a><br>"
  "<a href='/backward'><button>BACKWARD</button></a><br>"
  "</body>"
  "</html>";

  return html;
}

//====================================================
//                     SETUP
//====================================================

void setup()
{
  Serial.begin(115200);

  //==================================================
  // START PCA9685
  //==================================================

  Wire.begin();

  pwm.begin();

  pwm.setOscillatorFrequency(27000000);

  pwm.setPWMFreq(SERVO_FREQ);

  delay(10);

  //==================================================
  // ASSIGN PCA9685 CHANNELS
  //==================================================

  hipFL.attach(HIP_FL_CH);
  kneeFL.attach(KNEE_FL_CH);

  hipFR.attach(HIP_FR_CH);
  kneeFR.attach(KNEE_FR_CH);

  hipBL.attach(HIP_BL_CH);
  kneeBL.attach(KNEE_BL_CH);

  hipBR.attach(HIP_BR_CH);
  kneeBR.attach(KNEE_BR_CH);

  //==================================================
  // START IN STAND POSITION
  //==================================================

  hipFL.write(90);
  hipFR.write(90);
  hipBL.write(90);
  hipBR.write(90);

  kneeFL.write(90);
  kneeFR.write(90);
  kneeBL.write(90);
  kneeBR.write(90);

  //==================================================
  // WIFI ACCESS POINT
  //==================================================

  WiFi.beginAP(ssid, pass);

  Serial.print("AP IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

//====================================================
//                      LOOP
//====================================================

void loop()
{
  WiFiClient client = server.available();

  if (client)
  {
    String req = client.readStringUntil('\r');

    client.flush();

    if (req.indexOf("GET /stand") >= 0)
      standPose();

    if (req.indexOf("GET /sit") >= 0)
      sitPose();

    if (req.indexOf("GET /forward") >= 0)
      walkForward();

    if (req.indexOf("GET /backward") >= 0)
      walkBackward();

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();

    client.println(page());

    client.stop();
  }
}