#include <WiFiS3.h>
#include <Servo.h>

//---------------- WiFi ----------------//
char ssid[] = "DogRobot";
char pass[] = "12345678";

WiFiServer server(80);

//--------------- Servos ---------------//
Servo hipFL, hipFR, hipBL, hipBR;
Servo kneeFL, kneeFR, kneeBL, kneeBR;

//--------------- Pins -----------------//
// Change these if your wiring is different
const int HIP_FL_PIN   = 4;
const int HIP_FR_PIN   = 5;
const int HIP_BL_PIN   = 6;
const int HIP_BR_PIN   = 7;

const int KNEE_FL_PIN  = 8;
const int KNEE_FR_PIN  = 9;
const int KNEE_BL_PIN  = 10;
const int KNEE_BR_PIN  = 11;

//------------ Current Position --------//
int hipFLPos = 140;
int hipFRPos = 50;
int hipBLPos = 140;
int hipBRPos = 50;

int kneeFLPos = 140;
int kneeFRPos = 50;
int kneeBLPos = 140;
int kneeBRPos = 50;

// Walking parameters
const int HIP_FORWARD = 70;
const int HIP_BACK    = 110;

const int KNEE_UP     = 60;
const int KNEE_DOWN   = 90;

const int STEP_DELAY  = 15;

//------------- Smooth Move ------------//
void moveServo(Servo &servo, int &current, int target)
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
void walkCycle()
{
  //-----------------------------------
  // Front Left
  //-----------------------------------
  moveLeg(hipFL,kneeFL,
          hipFLPos,kneeFLPos,
          hipFLPos,KNEE_UP);

  moveLeg(hipFL,kneeFL,
          hipFLPos,kneeFLPos,
          HIP_FORWARD,KNEE_UP);

  moveLeg(hipFL,kneeFL,
          hipFLPos,kneeFLPos,
          HIP_FORWARD,KNEE_DOWN);

  moveLeg(hipFL,kneeFL,
          hipFLPos,kneeFLPos,
          90,KNEE_DOWN);

  //-----------------------------------
  // Back Right
  //-----------------------------------
  moveLeg(hipBR,kneeBR,
          hipBRPos,kneeBRPos,
          hipBRPos,KNEE_UP);

  moveLeg(hipBR,kneeBR,
          hipBRPos,kneeBRPos,
          HIP_BACK,KNEE_UP);

  moveLeg(hipBR,kneeBR,
          hipBRPos,kneeBRPos,
          HIP_BACK,KNEE_DOWN);

  moveLeg(hipBR,kneeBR,
          hipBRPos,kneeBRPos,
          90,KNEE_DOWN);

  //-----------------------------------
  // Front Right
  //-----------------------------------
  moveLeg(hipFR,kneeFR,
          hipFRPos,kneeFRPos,
          hipFRPos,KNEE_UP);

  moveLeg(hipFR,kneeFR,
          hipFRPos,kneeFRPos,
          HIP_BACK,KNEE_UP);

  moveLeg(hipFR,kneeFR,
          hipFRPos,kneeFRPos,
          HIP_BACK,KNEE_DOWN);

  moveLeg(hipFR,kneeFR,
          hipFRPos,kneeFRPos,
          90,KNEE_DOWN);

  //-----------------------------------
  // Back Left
  //-----------------------------------
  moveLeg(hipBL,kneeBL,
          hipBLPos,kneeBLPos,
          hipBLPos,KNEE_UP);

  moveLeg(hipBL,kneeBL,
          hipBLPos,kneeBLPos,
          HIP_FORWARD,KNEE_UP);

  moveLeg(hipBL,kneeBL,
          hipBLPos,kneeBLPos,
          HIP_FORWARD,KNEE_DOWN);

  moveLeg(hipBL,kneeBL,
          hipBLPos,kneeBLPos,
          90,KNEE_DOWN);
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
  standPose();

  for(int i=0;i<5;i++)
  {
    walkCycle();
  }

  standPose();
}

//------------- Web Page ---------------//
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
  "</body>"
  "</html>";

  return html;
}

//--------------- Setup ----------------//
void setup()
{
  hipFL.attach(HIP_FL_PIN);
  hipFR.attach(HIP_FR_PIN);
  hipBL.attach(HIP_BL_PIN);
  hipBR.attach(HIP_BR_PIN);

  kneeFL.attach(KNEE_FL_PIN);
  kneeFR.attach(KNEE_FR_PIN);
  kneeBL.attach(KNEE_BL_PIN);
  kneeBR.attach(KNEE_BR_PIN);

  // Start in sit position
  hipFL.write(140);
  hipFR.write(50);
  hipBL.write(140);
  hipBR.write(50);

  kneeFL.write(140);
  kneeFR.write(50);
  kneeBL.write(140);
  kneeBR.write(50);

  WiFi.beginAP(ssid, pass);

  Serial.begin(115200);
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

//---------------- Loop ----------------//
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

    if(req.indexOf("GET /forward") >= 0)
    walkForward();

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println(page());

    client.stop();
  }
}