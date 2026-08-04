#include <WiFiS3.h>
#include <Servo.h>
#include <ArduinoGraphics.h>
#include <Arduino_LED_Matrix.h>
ArduinoLEDMatrix matrix;

const char* wifiSSID = "Telezer_J";
const char* wifiPASS = "Telezer12";

volatile bool stopMotion = false;

WiFiServer server(80);
String ipString = "";

Servo servoBa1;
Servo servoBa2;
Servo servoBa3;
Servo servoBa4;

Servo servoHa1;
Servo servoHa2;
Servo servoHa3;
Servo servoHa4;

Servo servoKNa1;
Servo servoKNa2;
Servo servoKNa3;
Servo servoKNa4;

const int servoBa1pin = 2;   //front right
const int servoBa2pin = 5;   //front left
const int servoBa3pin = 8;   //back right
const int servoBa4pin = 11;  //back left

const int servoHa1Pin = 3;   // Front Right
const int servoHa2Pin = 6;   // front left
const int servoHa3Pin = 9;   // back right
const int servoHa4Pin = 12;  // back Left

const int servoKNa1Pin = 4;   // Front Right
const int servoKNa2Pin = 7;   // front left
const int servoKNa3Pin = 10;  // back right
const int servoKNa4Pin = 13;  // back Left

/************************************display**************************************/
void matrix_setup() {
  matrix.begin();

  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textFont(Font_5x7);
  matrix.textScrollSpeed(50);
  matrix.endDraw();
}


void displayText(const char* text) {
  matrix.beginDraw();

  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(50);
  matrix.textFont(Font_5x7);

  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(text);
  matrix.endText(SCROLL_LEFT);

  matrix.endDraw();
}


/**************** SET ALL SERVOS TO 90° ****************/
void Set() {

  // Body servos
  servoBa1.write(90);
  servoBa3.write(90);
  servoBa2.write(90);
  servoBa4.write(90);

  // Hip servos
  servoHa3.write(90);
  servoHa1.write(90);
  servoHa4.write(90);
  servoHa2.write(90);

  // Knee servos
  servoKNa3.write(90);
  servoKNa1.write(90);
  servoKNa4.write(90);
  servoKNa2.write(90);

  delay(500);
}

/****************sit*****************/
void Sit() {

  for (int i = 90; i >= 45; i--) {

    // Left pair
    servoHa1.write(i);
    servoHa4.write(i);
    servoKNa1.write(i - 45);
    servoKNa4.write(i - 45);

    // Right pair
    servoHa3.write(180 - i);
    servoHa2.write(180 - i);
    servoKNa3.write(230 - i);
    servoKNa2.write(230 - i);

    delay(80);
  }
}

/******************stand robot********************/  //stand
void Stand() {

  for (int i = 45; i <= 90; i++) {

    // Left pair
    servoHa1.write(i);
    servoHa4.write(i);

    servoKNa1.write(i - 45);  // 0 -> 45
    servoKNa4.write(i - 45);

    // Right pair
    servoHa3.write(180 - i);  // 135 -> 90
    servoHa2.write(180 - i);

    servoKNa3.write(230 - i);  // 180 -> 140
    servoKNa2.write(230 - i);

    delay(80);
  }

  Set();
}

void checkStop() {
  WiFiClient client = server.available();

  if (!client)
    return;

  String request = client.readStringUntil('\r');
  client.flush();

  if (request.indexOf("GET /set") >= 0) {
    stopMotion = true;

    client.println("HTTP/1.1 200 OK");
    client.println("Connection: close");
    client.println();
  }
  client.stop();
}

/**************** walk Forward ******************/
void walkForward() {

  for (int stp = 0; stp < 20; stp++) {
    checkStop();

    if (stopMotion) {
      Set();
      stopMotion = false;
      return;
    }
    //===========================
    // Pair 1 : Leg1 & Leg4
    //===========================

    for (int i = 0; i <= 20; i++) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      // Lift Pair 1
      servoKNa1.write(90 - i);
      servoKNa4.write(90 - i);

      // Swing Pair 1 backward
      servoHa1.write(90 - i);
      servoHa2.write(90 - i);

      // Pair 2 pushes forward
      servoHa3.write(90 + i);
      servoHa4.write(90 + i);

      delay(35);
    }

    // Lower Pair 1
    for (int i = 20; i >= 0; i--) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoKNa1.write(90 - i);
      servoKNa4.write(90 - i);

      servoHa1.write(70 + (20 - i));
      servoHa2.write(70 + (20 - i));

      servoHa3.write(110 - (20 - i));
      servoHa4.write(110 - (20 - i));

      delay(35);
    }

    //===========================
    // Pair 2 : Leg2 & Leg3
    //===========================

    for (int i = 0; i <= 20; i++) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      // Lift Pair 2
      servoKNa2.write(90 - i);
      servoKNa3.write(90 - i);

      // Swing Pair 2 backward
      servoHa3.write(90 - i);
      servoHa4.write(90 - i);

      // Pair 1 pushes forward
      servoHa1.write(90 + i);
      servoHa2.write(90 + i);

      delay(35);
    }

    // Lower Pair 2
    for (int i = 20; i >= 0; i--) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoKNa2.write(90 - i);
      servoKNa3.write(90 - i);

      servoHa3.write(70 + (20 - i));
      servoHa4.write(70 + (20 - i));

      servoHa1.write(110 - (20 - i));
      servoHa2.write(110 - (20 - i));

      delay(35);
    }
  }
}
/**************** BACKWARD ****************/
void walkBackward() {

  for (int stp = 0; stp < 20; stp++) {
    checkStop();

    if (stopMotion) {
      Set();
      stopMotion = false;
      return;
    }
    //===========================
    // Pair 1 : Leg1 & Leg4
    //===========================

    for (int i = 0; i <= 20; i++) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      // Lift Pair 1
      servoKNa1.write(90 - i);
      servoKNa4.write(90 - i);

      // Reverse hip swing
      servoHa1.write(90 + i);
      servoHa2.write(90 + i);

      servoHa3.write(90 - i);
      servoHa4.write(90 - i);

      delay(35);
    }

    // Pair 1 touches ground and pushes body backward
    for (int i = 0; i <= 20; i++) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoKNa1.write(70 + i);
      servoKNa4.write(70 + i);

      servoHa1.write(110 - i);
      servoHa2.write(110 - i);

      servoHa3.write(70 + i);
      servoHa4.write(70 + i);

      delay(35);
    }

    //===========================
    // Pair 2 : Leg2 & Leg3
    //===========================

    for (int i = 0; i <= 20; i++) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      // Lift Pair 2
      servoKNa2.write(90 - i);
      servoKNa3.write(90 - i);

      // Reverse hip swing
      servoHa3.write(90 + i);
      servoHa4.write(90 + i);

      servoHa1.write(90 - i);
      servoHa2.write(90 - i);

      delay(35);
    }

    // Pair 2 touches ground and pushes body backward
    for (int i = 0; i <= 20; i++) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoKNa2.write(70 + i);
      servoKNa3.write(70 + i);

      servoHa3.write(110 - i);
      servoHa4.write(110 - i);

      servoHa1.write(70 + i);
      servoHa2.write(70 + i);

      delay(35);
    }
  }
}
/*****************right move**********************/
void Rightmove() {
  for (int step = 0; step < 5; step++) {
    checkStop();

    if (stopMotion) {
      Set();
      stopMotion = false;
      return;
    }
    //---------------- STEP 1 ----------------//
    for (int i = 70; i <= 110; i++) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoBa1.write(90);
      servoBa2.write(i);
      servoKNa1.write(i);
      servoBa3.write(180 - i);
      servoBa4.write(90);

      delay(50);
    }
    for (int i = 110; i >= 70; i--) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoBa1.write(90);
      servoBa2.write(i);
      servoKNa1.write(i);
      servoBa3.write(180 - i);
      servoBa4.write(90);

      delay(50);
    }
    delay(40);
  }
}

/************************left move***************/
void Leftmove() {
  for (int step = 0; step < 5; step++) {
    checkStop();

    if (stopMotion) {
      Set();
      stopMotion = false;
      return;
    }
    //---------------- STEP 1 ----------------//
    for (int i = 110; i >= 70; i--) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoBa2.write(90);
      servoBa1.write(i);
      servoKNa2.write(i);
      servoBa4.write(180 - i);
      servoBa3.write(90);

      delay(80);
    }
    delay(40);

    for (int i = 70; i <= 110; i++) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoBa2.write(90);
      servoBa1.write(i);
      servoKNa3.write(i);
      servoBa4.write(180 - i);
      servoBa3.write(90);

      delay(80);
    }
    delay(40);
  }
}
/*******************hand shake*****************/
void Handshake() {

  servoBa3.write(90);
  servoBa2.write(90);
  servoBa4.write(90);

  servoHa3.write(90);
  servoHa4.write(90);
  servoHa2.write(90);

  servoKNa3.write(90);
  servoKNa4.write(90);
  servoKNa2.write(90);

  for (int i = 0; i < 2; i++) {
    checkStop();

    if (stopMotion) {
      Set();
      stopMotion = false;
      return;
    }
    servoHa1.write(150);
    servoBa1.write(85);
    delay(800);

    servoBa1.write(95);
    delay(800);

    servoKNa1.write(40);
    delay(200);

    servoKNa1.write(70);
    delay(200);
    servoHa1.write(90);
  }
  delay(500);
  Set();
}
/*******************setup()****************************/

void setup() {
  Serial.begin(115200);
  matrix_setup();
  const char* wifiSSID = "Telezer_J";
  const char* wifiPASS = "Telezer12";

  Serial.print("Connecting to WiFi");

  WiFi.begin(wifiSSID, wifiPASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  IPAddress ip = WiFi.localIP();
  ipString = ip.toString();  // Convert IP to text

  Serial.println();
  Serial.println("WiFi Connected");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();

  servoHa3.attach(servoHa3Pin);
  servoHa1.attach(servoHa1Pin);
  servoHa4.attach(servoHa4Pin);
  servoHa2.attach(servoHa2Pin);

  servoKNa3.attach(servoKNa3Pin);
  servoKNa1.attach(servoKNa1Pin);
  servoKNa4.attach(servoKNa4Pin);
  servoKNa2.attach(servoKNa2Pin);

  servoBa1.attach(servoBa1pin);
  servoBa3.attach(servoBa3pin);
  servoBa2.attach(servoBa2pin);
  servoBa4.attach(servoBa4pin);

  Serial.println("Quadruped Robot Started");

  Set();
}

void loop() {
  displayText(WiFi.localIP().toString().c_str());

  WiFiClient client = server.available();

  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    if (request.indexOf("GET /forward") >= 0)
      walkForward();

    else if (request.indexOf("GET /backward") >= 0)
      walkBackward();

    else if (request.indexOf("GET /left") >= 0)
      Leftmove();

    else if (request.indexOf("GET /right") >= 0)
      Rightmove();

    else if (request.indexOf("GET /stand") >= 0)
      Stand();

    else if (request.indexOf("GET /sit") >= 0)
      Sit();

    else if (request.indexOf("GET /handshake") >= 0)
      Handshake();

    else if (request.indexOf("GET /set") >= 0) {
      stopMotion = true;
    }

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();

    client.println(R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">

<title>Quadruped Robot Control</title>

<style>

*{
    margin:0;
    padding:0;
    box-sizing:border-box;
    font-family:Arial, Helvetica, sans-serif;
}

body{
    background:linear-gradient(135deg,#4facfe,#00f2fe);
    min-height:100vh;
    display:flex;
    justify-content:center;
    align-items:center;
}

.container{

    width:95%;
    max-width:500px;

    background:white;

    border-radius:20px;

    padding:25px;

    box-shadow:0px 10px 30px rgba(0,0,0,.3);

}

h1{

    color:#333;
    margin-bottom:20px;
    text-align:center;

}

.grid{

    display:grid;

    grid-template-columns:1fr 1fr;

    gap:15px;

}

a{

    text-decoration:none;

}

button{

    width:100%;
    height:70px;

    border:none;

    border-radius:15px;

    color:white;

    font-size:20px;

    font-weight:bold;

    cursor:pointer;

    transition:.25s;

}

button:hover{

    transform:scale(1.05);

}

button:active{

    transform:scale(.96);

}

/* Button Colors */

.set{background:#607D8B;}

.forward{background:#4CAF50;}

.backward{background:#FF5722;}

.left{background:#2196F3;}

.right{background:#9C27B0;}

.stand{background:#009688;}

.handshake{background:#FFC107;color:#222;}

.sit{background:#E91E63;}

.footer{

    margin-top:20px;

    text-align:center;

    color:#777;

    font-size:15px;

}

@media(max-width:500px){

.grid{

grid-template-columns:1fr;

}

button{

height:65px;

font-size:18px;

}

}

</style>

</head>

<body>

<div class="container">

<h1>Quadruped Robot</h1>

<div class="grid">

<a href="/set">
<button class="set">SET</button>
</a>

<a href="/stand">
<button class="stand">STAND</button>
</a>

<a href="/forward">
<button class="forward">FORWARD</button>
</a>

<a href="/backward">
<button class="backward"> BACKWARD</button>
</a>

<a href="/left">
<button class="left">LEFT</button>
</a>

<a href="/right">
<button class="right">RIGHT</button>
</a>

<a href="/handshake">
<button class="handshake">HAND SHAKE</button>
</a>

<a href="/sit">
<button class="sit">SIT</button>
</a>

</div>

<div class="footer">

Quadruped Robot Controller

</div>

</div>

</body>
</html>
)rawliteral");
    delay(1);
    client.stop();
  }
}