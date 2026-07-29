#include <WiFiS3.h>
#include <Servo.h>

const char* wifiSSID = "Telezer_J";
const char* wifiPASS = "Telezer12";

volatile bool stopMotion = false;

WiFiServer server(80);
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

const int servoB1pin = 2;   //front right
const int servoB3pin = 5;   //front left
const int servoB2pin = 8;   //back right
const int servoB4pin = 11;  //back left

const int servoH2Pin = 3;   // Front Right
const int servoH4Pin = 6;   // front left
const int servoH1Pin = 9;   // back right
const int servoH3Pin = 12;  // back Left

const int servoKN2Pin = 4;   // Front Right
const int servoKN4Pin = 7;   // front left
const int servoKN1Pin = 10;  // back right
const int servoKN3Pin = 13;  // back Left

/**************** SET ALL SERVOS TO 90° ****************/
void Set() {

  // Body servos
  servoB1.write(90);
  servoB2.write(90);
  servoB3.write(90);
  servoB4.write(90);

  // Hip servos
  servoH1.write(90);
  servoH2.write(90);
  servoH3.write(90);
  servoH4.write(90);

  // Knee servos
  servoKN1.write(90);
  servoKN2.write(90);
  servoKN3.write(90);
  servoKN4.write(90);

  delay(500);
}

/****************sit*****************/
void Sit() {
  for (int i = 90; i >= 0; i--) {
    // Left pair
    servoH2.write(i);
    servoH3.write(i);
    servoKN2.write(i - 45);
    servoKN3.write(i - 45);

    // Right pair
    servoH1.write(180 - i);
    servoH4.write(180 - i);
    servoKN1.write(230 - i);
    servoKN4.write(230 - i);

    delay(80);  // Same speed as Stand
  }

  delay(60);
}

/******************stand robot********************/  //stand
void Stand() {
  for (int i = 0; i <= 90; i++) {
    // Left pair
    servoH2.write(i);
    servoH3.write(i);
    servoKN2.write(i - 45);
    servoKN3.write(i - 45);

    // Right pair
    servoH1.write(180 - i);
    servoH4.write(180 - i);
    servoKN1.write(230 - i);
    servoKN4.write(230 - i);

    delay(50);  // Adjust speed here
  }

  delay(50);
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
  // Repeat walking cycle
  for (int step = 0; step < 10; step++) {
    checkStop();

    if (stopMotion) {
      Set();
      stopMotion = false;
      return;
    }
    //---------------- STEP 1 ----------------//
    for (int i = 90; i >= 70; i--) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoH1.write(i);
      servoH3.write(i);
      servoH2.write(180 - i);
      servoH4.write(180 - i);

      delay(50);
    }

    // Lift Legs 1 & 4
    for (int k = 90; k >= 70; k--) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoKN1.write(k);
      servoKN3.write(k);
      delay(50);
    }

    // Lower Legs 1 & 4
    for (int k = 70; k <= 90; k++) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoKN1.write(k);
      servoKN3.write(k);
      delay(50);
    }

    //---------------- STEP 2 ----------------//
    for (int i = 70; i <= 110; i++) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoH1.write(i);
      servoH3.write(i);
      servoH2.write(180 - i);
      servoH4.write(180 - i);

      delay(50);
    }

    // Lift Legs 2 & 3
    for (int k = 90; k >= 70; k--) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoKN2.write(k);
      servoKN4.write(k);
      delay(50);
    }

    // Lower Legs 2 & 3
    for (int k = 70; k <= 90; k++) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoKN2.write(k);
      servoKN4.write(k);
      delay(50);
    }

    //---------------- Return Center ----------------//
    for (int i = 110; i >= 90; i--) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoH1.write(i);
      servoH3.write(i);

      servoH2.write(180 - i);
      servoH4.write(180 - i);

      delay(50);
    }
  }
  delay(30);
}
/**************** BACKWARD ****************/
void walkBackward() {

  for (int step = 0; step < 10; step++) {
    checkStop();

    if (stopMotion) {
      Set();
      stopMotion = false;
      return;
    }

    //---------------- STEP 1 : Shift Hips ----------------//
    for (int i = 90; i <= 110; i++) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoH1.write(i);
      servoH3.write(i);

      servoH2.write(180 - i);
      servoH4.write(180 - i);

      delay(50);
    }

    //---------------- Lift Legs 1 & 3 ----------------//
    for (int k = 90; k <= 110; k++) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoKN1.write(k);
      servoKN3.write(k);
      delay(50);
    }

    //---------------- Lower Legs 1 & 3 ----------------//
    for (int k = 110; k >= 90; k--) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoKN1.write(k);
      servoKN3.write(k);
      delay(50);
    }

    //---------------- STEP 2 : Shift Hips ----------------//
    for (int i = 110; i >= 70; i--) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoH1.write(i);
      servoH3.write(i);

      servoH2.write(180 - i);
      servoH4.write(180 - i);

      delay(50);
    }

    //---------------- Lift Legs 2 & 4 ----------------//
    for (int k = 90; k <= 110; k++) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoKN2.write(k);
      servoKN4.write(k);
      delay(50);
    }

    //---------------- Lower Legs 2 & 4 ----------------//
    for (int k = 110; k >= 90; k--) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoKN2.write(k);
      servoKN4.write(k);
      delay(50);
    }

    //---------------- Return Center ----------------//
    for (int i = 70; i <= 90; i++) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoH1.write(i);
      servoH3.write(i);

      servoH2.write(180 - i);
      servoH4.write(180 - i);

      delay(50);
    }
  }
  delay(30);
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
      servoB1.write(90);
      servoB3.write(i);
      servoKN2.write(i);
      servoB2.write(180 - i);
      servoB4.write(90);

      delay(50);
    }
    for (int i = 110; i >= 70; i--) {
      checkStop();

      if (stopMotion) {
        Set();
        stopMotion = false;
        return;
      }
      servoB1.write(90);
      servoB3.write(i);
      servoKN2.write(i);
      servoB2.write(180 - i);
      servoB4.write(90);

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
      servoB3.write(90);
      servoB1.write(i);
      servoKN4.write(i);
      servoB4.write(180 - i);
      servoB2.write(90);

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
      servoB3.write(90);
      servoB1.write(i);
      servoKN1.write(i);
      servoB4.write(180 - i);
      servoB2.write(90);

      delay(80);
    }
    delay(40);
  }
}
/*******************hand shake*****************/
void Handshake() {

  servoB2.write(90);
  servoB3.write(90);
  servoB4.write(90);

  servoH1.write(90);
  servoH3.write(90);
  servoH4.write(90);

  servoKN1.write(90);
  servoKN3.write(90);
  servoKN4.write(90);

  for (int i = 0; i < 2; i++) {
    checkStop();

    if (stopMotion) {
      Set();
      stopMotion = false;
      return;
    }
    servoH2.write(150);
    servoB1.write(85);
    delay(800);

    servoB1.write(95);
    delay(800);

    servoKN2.write(40);
    delay(200);

    servoKN2.write(70);
    delay(200);
    servoH2.write(90);
  }
  delay(500);
  Set();
}
/*******************setup()****************************/
void setup() {
  Serial.begin(115200);
  const char* wifiSSID = "Telezer_J";
  const char* wifiPASS = "Telezer12";

  Serial.print("Connecting to WiFi");

  WiFi.begin(wifiSSID, wifiPASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();

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

  Set();
}

void loop() {
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