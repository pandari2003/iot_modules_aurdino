#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>

const char* AP_SSID     = "Telezer_J1";
const char* AP_PASSWORD = "Telezer12";   // must be >= 8 characters

WebServer server(80);

Servo rightHip;
Servo rightFoot;
Servo leftHip;
Servo leftFoot;

#define RIGHT_HIP_PIN   13
#define RIGHT_FOOT_PIN  27
#define LEFT_HIP_PIN    14
#define LEFT_FOOT_PIN   26

// Fixed geometry (no longer web-adjustable). If the robot needs
// re-calibrating, edit these constants and re-upload.
const int RH_CENTER = 90;
const int LH_CENTER = 90;
const int RF_CENTER = 90;
const int LF_CENTER = 90;

const int RH_INVERT = -1;
const int LH_INVERT = 1;
const int RF_INVERT = -1;
const int LF_INVERT = 1;

const int HIP_SWING = 45;
const int FOOT_TILT = 30;

// ===== SPEED =====
// 1 = Slow & Stable, 2 = Medium, 3 = Fast
int SPEED_LEVEL = 1;

int servoDelay, servoStep, liftPauseMs, leanPauseMs;

//======================================================
// Mode â€” what the robot is currently doing, driven by web requests.
// Declared here near the top (before any function) so that Arduino's
// auto-generated function prototypes, which get inserted above this
// point otherwise, can see the type. Keep this above all functions.
//======================================================
enum Mode { MODE_STOP, MODE_FORWARD, MODE_BACKWARD, MODE_TURN_LEFT, MODE_TURN_RIGHT, MODE_DANCE };
volatile Mode mode = MODE_FORWARD;   // starts walking immediately on boot

const char* modeName(Mode m)
{
  switch (m) {
    case MODE_FORWARD:    return "Forward";
    case MODE_BACKWARD:   return "Backward";
    case MODE_TURN_LEFT:  return "Turning Left";
    case MODE_TURN_RIGHT: return "Turning Right";
    case MODE_DANCE:      return "Dancing";
    default:               return "Stopped";
  }
}

void applySpeed(int level)
{
  SPEED_LEVEL = level;
  switch (level) {
    case 1:
      servoDelay  = 3; servoStep = 4; liftPauseMs = 150; leanPauseMs = 120;
      break;
    case 2:
      servoDelay  = 2; servoStep = 6; liftPauseMs = 90;  leanPauseMs = 70;
      break;
    case 3:
    default:
      servoDelay  = 1; servoStep = 9; liftPauseMs = 55;  leanPauseMs = 40;
      break;
  }
}

// Derived angles (fixed now that calibration trims are gone)
int RH_FWD, RH_BACK;
int LH_FWD, LH_BACK;
int RF_UP, RF_DOWN;
int LF_UP, LF_DOWN;

void recalcAngles() {
  RH_FWD  = RH_CENTER + HIP_SWING * RH_INVERT;
  RH_BACK = RH_CENTER - HIP_SWING * RH_INVERT;

  LH_FWD  = LH_CENTER + HIP_SWING * LH_INVERT;
  LH_BACK = LH_CENTER - HIP_SWING * LH_INVERT;

  RF_DOWN = RF_CENTER;
  RF_UP   = RF_CENTER + FOOT_TILT * RF_INVERT;

  LF_DOWN = LF_CENTER;
  LF_UP   = LF_CENTER + FOOT_TILT * LF_INVERT;
}

// Tracked current angles
int curRH, curLH, curRF, curLF;

void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do {
    server.handleClient();
  } while (millis() - start < ms);
}

// Smooth servo movement that updates tracked state
void moveServo(Servo &servo, int &currentAngle, int endAngle)
{
  if (currentAngle == endAngle) {
    servo.write(endAngle);
    return;
  }
  if (currentAngle < endAngle) {
    for (int i = currentAngle; i <= endAngle; i += servoStep) {
      servo.write(i);
      smartDelay(servoDelay);
    }
  } else {
    for (int i = currentAngle; i >= endAngle; i -= servoStep) {
      servo.write(i);
      smartDelay(servoDelay);
    }
  }
  servo.write(endAngle);
  currentAngle = endAngle;
}

// Stand Position
void stand()
{
  curRH = RH_CENTER;
  curLH = LH_CENTER;
  curRF = RF_DOWN;
  curLF = LF_DOWN;

  rightHip.write(curRH);
  leftHip.write(curLH);
  rightFoot.write(curRF);
  leftFoot.write(curLF);

  smartDelay(300);
}

void leanLeft()
{
  moveServo(rightFoot, curRF, RF_UP);
  moveServo(leftFoot,  curLF, LF_UP);
  smartDelay(leanPauseMs);
}

void leanRight()
{
  moveServo(rightFoot, curRF, RF_DOWN - FOOT_TILT * RF_INVERT);
  moveServo(leftFoot,  curLF, LF_DOWN - FOOT_TILT * LF_INVERT);
  smartDelay(leanPauseMs);
}

void feetLevel()
{
  moveServo(rightFoot, curRF, RF_DOWN);
  moveServo(leftFoot,  curLF, LF_DOWN);
}

//======================================================
// FORWARD stepping
//======================================================
void rightStep()
{
  leanLeft();
  moveServo(rightHip, curRH, RH_FWD);
  smartDelay(liftPauseMs);

  feetLevel();
  smartDelay(20);

  leanRight();
  moveServo(rightHip, curRH, RH_CENTER);
  smartDelay(leanPauseMs);
}

void leftStep()
{
  leanRight();
  moveServo(leftHip, curLH, LH_FWD);
  smartDelay(liftPauseMs);

  feetLevel();
  smartDelay(20);

  leanLeft();
  moveServo(leftHip, curLH, LH_CENTER);
  smartDelay(leanPauseMs);
}

//======================================================
// BACKWARD stepping (mirror of forward: hip swings the other way)
//======================================================
void rightStepBack()
{
  leanLeft();
  moveServo(rightHip, curRH, RH_BACK);
  smartDelay(liftPauseMs);

  feetLevel();
  smartDelay(20);

  leanRight();
  moveServo(rightHip, curRH, RH_CENTER);
  smartDelay(leanPauseMs);
}

void leftStepBack()
{
  leanRight();
  moveServo(leftHip, curLH, LH_BACK);
  smartDelay(liftPauseMs);

  feetLevel();
  smartDelay(20);

  leanLeft();
  moveServo(leftHip, curLH, LH_CENTER);
  smartDelay(leanPauseMs);
}

//======================================================
// TURNING ("side moves") â€” only one leg steps forward at a time,
// which pivots the body toward the side that stays planted.
//======================================================
void turnLeftStep()   // only the RIGHT leg steps -> body pivots left
{
  leanLeft();
  moveServo(rightHip, curRH, RH_FWD);
  smartDelay(liftPauseMs);

  feetLevel();
  smartDelay(20);

  leanRight();
  moveServo(rightHip, curRH, RH_CENTER);
  smartDelay(leanPauseMs);
}

void turnRightStep()  // only the LEFT leg steps -> body pivots right
{
  leanRight();
  moveServo(leftHip, curLH, LH_FWD);
  smartDelay(liftPauseMs);

  feetLevel();
  smartDelay(20);

  leanLeft();
  moveServo(leftHip, curLH, LH_CENTER);
  smartDelay(leanPauseMs);
}

//======================================================
// DANCE â€” cycles through a few canned moves each time it's called
//======================================================
int danceMoveIndex = 0;

void danceShimmy()
{
  for (int i = 0; i < 3; i++) { leanLeft(); leanRight(); }
  feetLevel();
}

void danceKick()
{
  moveServo(rightHip, curRH, RH_FWD);
  smartDelay(150);
  moveServo(rightHip, curRH, RH_CENTER);
  moveServo(leftHip, curLH, LH_FWD);
  smartDelay(150);
  moveServo(leftHip, curLH, LH_CENTER);
}

void danceSpin()
{
  turnLeftStep();
  turnLeftStep();
}

void danceBounce()
{
  leanLeft(); leanRight(); leanLeft(); leanRight();
}

void danceStep()
{
  switch (danceMoveIndex % 4) {
    case 0: danceShimmy(); break;
    case 1: danceKick();   break;
    case 2: danceSpin();   break;
    case 3: danceBounce(); break;
  }
  danceMoveIndex++;
}

//======================================================
// Web page (no calibration section)
//======================================================
const char PAGE_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Robot Control</title>
<style>
  body { font-family: sans-serif; background:#111; color:#eee; text-align:center; margin:0; padding:16px; }
  h2 { margin-top: 0; }
  .row { display:flex; justify-content:center; gap:10px; flex-wrap:wrap; margin:12px 0; }
  button { font-size:18px; padding:14px 22px; border:none; border-radius:10px; background:#2a2a2a; color:#eee; }
  button:active { background:#444; }
  .big { background:#2e7d32; }
  .stop { background:#b71c1c; }
  .dance { background:#6a1b9a; }
  .turn { background:#ef6c00; }
  .modeBtn.active { background:#1565c0; }
  .speedBtn.active { background:#1565c0; }
  #status { margin-top:14px; font-size:14px; color:#9aa; }
</style>
</head>
<body>
<h2>Spider Robot</h2>

<div class="row">
  <button class="big modeBtn" id="m_forward" onclick="setMode('/forward')">Forward</button>
  <button class="turn modeBtn" id="m_back" onclick="setMode('/backward')">Backward</button>
</div>

<div class="row">
  <button class="turn modeBtn" id="m_turnleft" onclick="setMode('/turnleft')">Turn Left</button>
  <button class="turn modeBtn" id="m_turnright" onclick="setMode('/turnright')">Turn Right</button>
</div>

<div class="row">
  <button class="dance modeBtn" id="m_dance" onclick="setMode('/dance')">Dance</button>
  <button class="stop modeBtn" id="m_stop" onclick="setMode('/stop')">Stop</button>
</div>

<div class="row">
  <button class="speedBtn" id="s1" onclick="setSpeed(1)">Slow</button>
  <button class="speedBtn" id="s2" onclick="setSpeed(2)">Medium</button>
  <button class="speedBtn" id="s3" onclick="setSpeed(3)">Fast</button>
</div>

<div id="status">loading...</div>

<script>
function setMode(path) {
  fetch(path).then(refreshStatus);
}

function setSpeed(level) {
  fetch(`/speed?level=${level}`).then(refreshStatus);
}

const modeButtonMap = {
  'Forward': 'm_forward',
  'Backward': 'm_back',
  'Turning Left': 'm_turnleft',
  'Turning Right': 'm_turnright',
  'Dancing': 'm_dance',
  'Stopped': 'm_stop'
};

function refreshStatus() {
  fetch('/status').then(r => r.json()).then(d => {
    document.getElementById('status').innerText =
      d.mode + ' | Speed ' + d.SPEED_LEVEL;

    Object.values(modeButtonMap).forEach(id => {
      document.getElementById(id).classList.remove('active');
    });
    const activeId = modeButtonMap[d.mode];
    if (activeId) document.getElementById(activeId).classList.add('active');

    [1,2,3].forEach(n => {
      document.getElementById('s'+n).classList.toggle('active', d.SPEED_LEVEL == n);
    });
  });
}

refreshStatus();
setInterval(refreshStatus, 2000);
</script>
</body>
</html>
)HTML";

//======================================================
// Route handlers
//======================================================
void handleRoot()
{
  // Copy the PROGMEM page into a regular String and send it with a normal
  // send() call. send_P() on a large PROGMEM buffer can truncate the
  // response on some versions of the ESP32 WebServer library, which shows
  // up in the browser as a blank or broken-looking page.
  String page = FPSTR(PAGE_HTML);
  server.setContentLength(page.length());
  server.send(200, "text/html", page);
}

void handleNotFound()
{
  server.send(404, "text/plain", "Not found");
}

void handleForward()
{
  mode = MODE_FORWARD;
  server.send(200, "text/plain", "OK");
}

void handleBackward()
{
  mode = MODE_BACKWARD;
  server.send(200, "text/plain", "OK");
}

void handleTurnLeft()
{
  mode = MODE_TURN_LEFT;
  server.send(200, "text/plain", "OK");
}

void handleTurnRight()
{
  mode = MODE_TURN_RIGHT;
  server.send(200, "text/plain", "OK");
}

void handleDance()
{
  mode = MODE_DANCE;
  server.send(200, "text/plain", "OK");
}

void handleStop()
{
  mode = MODE_STOP;
  feetLevel();
  server.send(200, "text/plain", "OK");
}

void handleStand()
{
  mode = MODE_STOP;
  stand();
  server.send(200, "text/plain", "OK");
}

void handleSpeed()
{
  if (server.hasArg("level")) {
    int lvl = server.arg("level").toInt();
    if (lvl >= 1 && lvl <= 3) applySpeed(lvl);
  }
  server.send(200, "text/plain", "OK");
}

void handleStatus()
{
  String json = "{";
  json += "\"mode\":\"" + String(modeName(mode)) + "\",";
  json += "\"SPEED_LEVEL\":" + String(SPEED_LEVEL);
  json += "}";
  server.send(200, "application/json", json);
}

//--------------------------------------------------
// Setup
//--------------------------------------------------
void setup()
{
  Serial.begin(115200);

  applySpeed(SPEED_LEVEL);
  recalcAngles();

  rightHip.setPeriodHertz(50);
  rightFoot.setPeriodHertz(50);
  leftHip.setPeriodHertz(50);
  leftFoot.setPeriodHertz(50);

  rightHip.attach(RIGHT_HIP_PIN, 500, 2400);
  rightFoot.attach(RIGHT_FOOT_PIN, 500, 2400);
  leftHip.attach(LEFT_HIP_PIN, 500, 2400);
  leftFoot.attach(LEFT_FOOT_PIN, 500, 2400);

  stand();

  WiFi.softAP(AP_SSID, AP_PASSWORD);
  Serial.print("AP started. Connect to WiFi: ");
  Serial.println(AP_SSID);
  Serial.print("Then open http://");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/forward", handleForward);
  server.on("/backward", handleBackward);
  server.on("/turnleft", handleTurnLeft);
  server.on("/turnright", handleTurnRight);
  server.on("/dance", handleDance);
  server.on("/stop", handleStop);
  server.on("/stand", handleStand);
  server.on("/speed", handleSpeed);
  server.on("/status", handleStatus);
  server.onNotFound(handleNotFound);
  server.begin();

  Serial.println("Robot web server ready. Walking forward continuously.");
}

void loop()
{
  server.handleClient();

  switch (mode) {
    case MODE_FORWARD:
      rightStep();
      leftStep();
      break;
    case MODE_BACKWARD:
      rightStepBack();
      leftStepBack();
      break;
    case MODE_TURN_LEFT:
      turnLeftStep();
      break;
    case MODE_TURN_RIGHT:
      turnRightStep();
      break;
    case MODE_DANCE:
      danceStep();
      break;
    default:
      break;
  }
}