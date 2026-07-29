#include <Servo.h>

// Ultrasonic Sensor 1
const int TRIG1_PIN = 2;
const int ECHO1_PIN = 3;

// Ultrasonic Sensor 2
const int TRIG2_PIN = 12;
const int ECHO2_PIN = 13;

//--------------- Servos ---------------//
Servo hipFL, hipFR, hipBL, hipBR;
Servo kneeFL, kneeFR, kneeBL, kneeBR;

//--------------- Pins -----------------//
const int HIP_FL_PIN   = 4;
const int HIP_FR_PIN   = 5;
const int HIP_BL_PIN   = 6;
const int HIP_BR_PIN   = 7;

const int KNEE_FL_PIN  = 8;
const int KNEE_FR_PIN  = 9;
const int KNEE_BL_PIN  = 10;
const int KNEE_BR_PIN  = 11;

//------------ Current Position --------//
int hipFLPos = 90;
int hipFRPos = 90;
int hipBLPos = 90;
int hipBRPos = 90;

int kneeFLPos = 90;
int kneeFRPos = 90;
int kneeBLPos = 90;
int kneeBRPos = 90;

// Walking parameters
// Left legs
const int HIP_LEFT_FORWARD = 70;
const int HIP_LEFT_BACK    = 110;

// Right legs (mirror)
const int HIP_RIGHT_FORWARD = 110;
const int HIP_RIGHT_BACK    = 70;

const int KNEE_UP   = 65;
const int KNEE_DOWN = 90;

const int STEP_DELAY  = 20;


void moveLeg(
Servo &hip,
Servo &knee,
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

void moveTwoLegs(
    Servo &hip1, Servo &knee1,
    int &hipPos1, int &kneePos1,
    int hipTarget1, int kneeTarget1,

    Servo &hip2, Servo &knee2,
    int &hipPos2, int &kneePos2,
    int hipTarget2, int kneeTarget2)
{
    while (hipPos1 != hipTarget1 ||
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
void walkCycle()
{
    //===========================
    // Phase 1
    // Front Left + Back Right
    //===========================

    // Lift both legs
    moveTwoLegs(
        hipFL, kneeFL, hipFLPos, kneeFLPos, hipFLPos, KNEE_UP,
        hipBR, kneeBR, hipBRPos, kneeBRPos, hipBRPos, KNEE_UP);

    // Swing forward
    moveTwoLegs(
        hipFL, kneeFL, hipFLPos, kneeFLPos, HIP_LEFT_FORWARD, KNEE_UP,
        hipBR, kneeBR, hipBRPos, kneeBRPos, HIP_RIGHT_FORWARD, KNEE_UP);

    // Place on ground
    moveTwoLegs(
        hipFL, kneeFL, hipFLPos, kneeFLPos, HIP_LEFT_FORWARD, KNEE_DOWN,
        hipBR, kneeBR, hipBRPos, kneeBRPos, HIP_RIGHT_FORWARD, KNEE_DOWN);

    //===========================
    // Phase 2
    // Front Right + Back Left
    //===========================

    moveTwoLegs(
        hipFR, kneeFR, hipFRPos, kneeFRPos, hipFRPos, KNEE_UP,
        hipBL, kneeBL, hipBLPos, kneeBLPos, hipBLPos, KNEE_UP);

    moveTwoLegs(
        hipFR, kneeFR, hipFRPos, kneeFRPos, HIP_RIGHT_FORWARD, KNEE_UP,
        hipBL, kneeBL, hipBLPos, kneeBLPos, HIP_LEFT_FORWARD, KNEE_UP);

    moveTwoLegs(
        hipFR, kneeFR, hipFRPos, kneeFRPos, HIP_RIGHT_FORWARD, KNEE_DOWN,
        hipBL, kneeBL, hipBLPos, kneeBLPos, HIP_LEFT_FORWARD, KNEE_DOWN);

    //===========================
    // Return to neutral
    //===========================

    moveTwoLegs(
        hipFL, kneeFL, hipFLPos, kneeFLPos, 90, KNEE_DOWN,
        hipBR, kneeBR, hipBRPos, kneeBRPos, 90, KNEE_DOWN);

    moveTwoLegs(
        hipFR, kneeFR, hipFRPos, kneeFRPos, 90, KNEE_DOWN,
        hipBL, kneeBL, hipBLPos, kneeBLPos, 90, KNEE_DOWN);
}
void walkCycleBackward()
{
    // Front Left + Back Right
    moveTwoLegs(
        hipFL, kneeFL, hipFLPos, kneeFLPos, hipFLPos, KNEE_UP,
        hipBR, kneeBR, hipBRPos, kneeBRPos, hipBRPos, KNEE_UP);

    moveTwoLegs(
        hipFL, kneeFL, hipFLPos, kneeFLPos, HIP_LEFT_BACK, KNEE_UP,
        hipBR, kneeBR, hipBRPos, kneeBRPos, HIP_RIGHT_BACK, KNEE_UP);

    moveTwoLegs(
        hipFL, kneeFL, hipFLPos, kneeFLPos, HIP_LEFT_BACK, KNEE_DOWN,
        hipBR, kneeBR, hipBRPos, kneeBRPos, HIP_RIGHT_BACK, KNEE_DOWN);

    // Front Right + Back Left
    moveTwoLegs(
        hipFR, kneeFR, hipFRPos, kneeFRPos, hipFRPos, KNEE_UP,
        hipBL, kneeBL, hipBLPos, kneeBLPos, hipBLPos, KNEE_UP);

    moveTwoLegs(
        hipFR, kneeFR, hipFRPos, kneeFRPos, HIP_RIGHT_BACK, KNEE_UP,
        hipBL, kneeBL, hipBLPos, kneeBLPos, HIP_LEFT_BACK, KNEE_UP);

    moveTwoLegs(
        hipFR, kneeFR, hipFRPos, kneeFRPos, HIP_RIGHT_BACK, KNEE_DOWN,
        hipBL, kneeBL, hipBLPos, kneeBLPos, HIP_LEFT_BACK, KNEE_DOWN);

    moveTwoLegs(
        hipFL, kneeFL, hipFLPos, kneeFLPos, 90, KNEE_DOWN,
        hipBR, kneeBR, hipBRPos, kneeBRPos, 90, KNEE_DOWN);

    moveTwoLegs(
        hipFR, kneeFR, hipFRPos, kneeFRPos, 90, KNEE_DOWN,
        hipBL, kneeBL, hipBLPos, kneeBLPos, 90, KNEE_DOWN);
}

void walkBackward()
{
    standPose();

    for(int i = 0; i < 2; i++)
    {
        walkCycleBackward();
    }

    standPose();
}

//------------- Stand Pose -------------//
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

if (hipFLPos < 90) hipFLPos++;
if (hipFLPos > 90) hipFLPos--;

if (hipFRPos < 90) hipFRPos++;
if (hipFRPos > 90) hipFRPos--;

if (hipBLPos < 90) hipBLPos++;
if (hipBLPos > 90) hipBLPos--;

if (hipBRPos < 90) hipBRPos++;
if (hipBRPos > 90) hipBRPos--;

if (kneeFLPos < 90) kneeFLPos++;
if (kneeFLPos > 90) kneeFLPos--;

if (kneeFRPos < 90) kneeFRPos++;
if (kneeFRPos > 90) kneeFRPos--;

if (kneeBLPos < 90) kneeBLPos++;
if (kneeBLPos > 90) kneeBLPos--;

if (kneeBRPos < 90) kneeBRPos++;
if (kneeBRPos > 90) kneeBRPos--;

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

//-------------- Sit Pose --------------//
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

void walkForward()
{
    // If the robot is sitting, stand up smoothly.
    standPose();

    // Walk forward.
    for (int i = 0; i < 2; i++)
    {
        walkCycle();
    }

    // Finish in standing position.
    standPose();
}


long getDistance(int trigPin, int echoPin)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);

  if(duration == 0)
    return 400;

  return duration * 0.017;
}
//--------------- Setup ----------------//
void setup()
{
  Serial.begin(115200);

  hipFL.attach(HIP_FL_PIN);
  hipFR.attach(HIP_FR_PIN);
  hipBL.attach(HIP_BL_PIN);
  hipBR.attach(HIP_BR_PIN);

  kneeFL.attach(KNEE_FL_PIN);
  kneeFR.attach(KNEE_FR_PIN);
  kneeBL.attach(KNEE_BL_PIN);
  kneeBR.attach(KNEE_BR_PIN);

  pinMode(TRIG1_PIN, OUTPUT);
pinMode(ECHO1_PIN, INPUT);

pinMode(TRIG2_PIN, OUTPUT);
pinMode(ECHO2_PIN, INPUT);

  hipFL.write(90);
  hipFR.write(90);
  hipBL.write(90);
  hipBR.write(90);

  kneeFL.write(90);
  kneeFR.write(90);
  kneeBL.write(90);
  kneeBR.write(90);

  Serial.println("Robot Dog Started");
  Serial.println("Initial State: STAND");
}

//---------------- Loop ----------------//
void loop()
{
  long distance1 = getDistance(TRIG1_PIN, ECHO1_PIN);
  long distance2 = getDistance(TRIG2_PIN, ECHO2_PIN);

  Serial.print("S1: ");
  Serial.print(distance1);
  Serial.print(" cm   ");

  Serial.print("S2: ");
  Serial.print(distance2);
  Serial.println(" cm");

  // SIT
  if(distance1 < 5 || distance2 < 5)
  {
    Serial.println("Action : SIT");
    sitPose();
  }

  // BACKWARD
  else if(
      distance1 >= 5 && distance1 < 15 &&
      distance2 >= 5 && distance2 < 15)
  {
    Serial.println("Action : BACKWARD");
    walkBackward();
  }

  // STAND
  else if(
      distance1 >= 15 && distance1 <= 30 &&
      distance2 >= 15 && distance2 <= 30)
  {
    Serial.println("Action : STAND");
    standPose();
  }

  // FORWARD
else if(distance1 > 30 && distance2 > 30)
{
    Serial.println("Action : FORWARD");
    walkForward();
}

  else
  {
    Serial.println("Action : STAND");
    standPose();
  }
}