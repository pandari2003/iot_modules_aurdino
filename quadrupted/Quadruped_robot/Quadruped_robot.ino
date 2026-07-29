#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const char* ssid = "Quadruped_Robot";
const char* password = "12345678";

WebServer server(80);

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

const int servoB1pin = 42;
const int servoB2pin = 41;
const int servoB3pin = 40;
const int servoB4pin = 39;

const int servoH1Pin = 4;   // Front Left Forward (servo 8)
const int servoH2Pin = 5;   // Front Right Back 5 (servo 7)
const int servoH3Pin = 18;  // back Left Back 6 ((servo 6)
const int servoH4Pin = 19;  // back Right Forward 8 (servo 5)

const int servoKN1Pin = 46;   // Front Left (servo 12)
const int servoKN2Pin = 15;  // Front Right (servo 11)
const int servoKN3Pin = 16;  // back Left (servo 9)
const int servoKN4Pin = 17;  // back Right (servo 10)

/**********************sit robot******************/
void Sit()
{
    for (int i = 90; i >= 0; i--)
    {
        // Left pair
        servoH2.write(i);
        servoH3.write(i);
        servoKN2.write(i);
        servoKN3.write(i);

        // Right pair
        servoH1.write(180 - i);
        servoH4.write(180 - i);
        servoKN1.write(180 - i);
        servoKN4.write(180 - i);

        delay(50);      // Same speed as Stand
    }

    delay(50);
}

/******************stand robot********************/
void Stand()
{
    for (int i = 0; i <= 90; i++)
    {
        // Left pair
        servoH2.write(i);
        servoH3.write(i);
        servoKN2.write(i);
        servoKN3.write(i);

        // Right pair
        servoH1.write(180 - i);
        servoH4.write(180 - i);
        servoKN1.write(180 - i);
        servoKN4.write(180 - i);

        delay(50);      // Adjust speed here
    }

    delay(50);
}

/****************FORWARD****************/
void walkForward()
{
  // Repeat walking cycle
  for (int step = 0; step < 20; step++)
  {
    //---------------- STEP 1 ----------------//
    for (int i = 90; i >= 70; i--)
    {
      servoH1.write(i);
      servoH3.write(i);

      servoH2.write(180 - i);
      servoH4.write(180 - i);

      delay(30);
    }

    // Lift Legs 1 & 4
    for (int k = 90; k >= 70; k--)
    {
      servoKN1.write(k);
      servoKN3.write(k);
      delay(30);
    }

    // Lower Legs 1 & 4
    for (int k = 70; k <= 90; k++)
    {
      servoKN1.write(k);
      servoKN3.write(k);
      delay(30);
    }

    //---------------- STEP 2 ----------------//
    for (int i = 70; i <= 110; i++)
    {
      servoH1.write(i);
      servoH3.write(i);

      servoH2.write(180 - i);
      servoH4.write(180 - i);

      delay(30);
    }

    // Lift Legs 2 & 3
    for (int k = 90; k >= 70; k--)
    {
      servoKN2.write(k);
      servoKN4.write(k);
      delay(30);
    }

    // Lower Legs 2 & 3
    for (int k = 70; k <= 90; k++)
    {
      servoKN2.write(k);
      servoKN4.write(k);
      delay(30);
    }

    //---------------- Return Center ----------------//
    for (int i = 110; i >= 90; i--)
    {
      servoH1.write(i);
      servoH3.write(i);

      servoH2.write(180 - i);
      servoH4.write(180 - i);

      delay(30);
    }
  }

}

/****************BACKWARD****************/
void walkBackward()
{
  // Repeat walking cycle
  for (int step = 0; step < 20; step++)
  {
    //---------------- STEP 1 ----------------//
    for (int i = 90; i <= 110; i++)
    {
      servoH1.write(i);
      servoH3.write(i);

      servoH2.write(180 - i);
      servoH4.write(180 - i);

      delay(30);
    }

    // Lift Legs 1 & 4
    for (int k = 90; k >= 70; k--)
    {
      servoKN1.write(k);
      servoKN3.write(k);
      delay(30);
    }

    // Lower Legs 1 & 4
    for (int k = 70; k <= 90; k++)
    {
      servoKN1.write(k);
      servoKN3.write(k);
      delay(30);
    }

    //---------------- STEP 2 ----------------//
    for (int i = 110; i >= 70; i--)
    {
      servoH1.write(i);
      servoH3.write(i);

      servoH2.write(180 - i);
      servoH4.write(180 - i);

      delay(30);
    }

    // Lift Legs 2 & 3
    for (int k = 90; k >= 70; k--)
    {
      servoKN2.write(k);
      servoKN4.write(k);
      delay(30);
    }

    // Lower Legs 2 & 3
    for (int k = 70; k <= 90; k++)
    {
      servoKN2.write(k);
      servoKN4.write(k);
      delay(30);
    }

    //---------------- Return Center ----------------//
    for (int i = 70; i <= 90; i++)
    {
      servoH1.write(i);
      servoH3.write(i);

      servoH2.write(180 - i);
      servoH4.write(180 - i);

      delay(30);
    }
  }

 
}

/*****************right turn**********************/
void Rightmove(){
    for (int step = 0; step < 5; step++)
  {
    //---------------- STEP 1 ----------------//
    for (int i = 70; i <= 110; i++)
    {
      servoB1.write(i);
      servoB3.write(i);

      servoB2.write(180 - i);
      servoB4.write(180 - i);

      delay(80);
    }
    delay(40);
  }

}

/************************left move***************/
void Leftmove(){
    for (int step = 0; step < 5; step++)
  {
    //---------------- STEP 1 ----------------//
    for (int i = 110; i >= 70; i--)
    {
      servoB1.write(i);
      servoB3.write(i);

      servoB2.write(180 - i);
      servoB4.write(180 - i);

      delay(80);
    }
    delay(40);
  }
}
/*************rooting and web page**************/
void root() {

String page = R"====(
<!DOCTYPE html>
<html>
<head>
<title>ESP32 Walking Robot</title>

<meta name="viewport" content="width=device-width, initial-scale=1.0">

<style>

*{
    margin:0;
    padding:0;
    box-sizing:border-box;
    font-family:Arial,sans-serif;
}

body{
    background:#f2f5f9;
    display:flex;
    justify-content:center;
    align-items:center;
    min-height:100vh;
}

.container{
    width:95%;
    max-width:420px;
    background:white;
    border-radius:20px;
    padding:25px;
    box-shadow:0 8px 25px rgba(0,0,0,.15);
}

h2{
    text-align:center;
    margin-bottom:25px;
    color:#333;
}

.grid{
    display:grid;
    grid-template-columns:1fr 1fr;
    gap:15px;
}

form{
    width:100%;
}

button{
    width:100%;
    height:70px;
    border:none;
    border-radius:12px;
    font-size:20px;
    font-weight:bold;
    color:white;
    cursor:pointer;
    transition:.2s;
}

button:hover{
    transform:scale(1.03);
}

.forward{background:#27ae60;}
.backward{background:#2980b9;}
.sit{background:#f39c12;}
.stand{background:#8e44ad;}
.left{background:#16a085;}
.right{background:#c0392b;}

@media(max-width:480px){

button{
    height:65px;
    font-size:18px;
}

h2{
    font-size:24px;
}

}

</style>

</head>

<body>

<div class="container">

<h2>Quadruped Robot Control</h2>

<div class="grid">

<form action="/forward">
<button class="forward">FORWARD</button>
</form>

<form action="/backward">
<button class="backward">BACKWARD</button>
</form>

<form action="/leftmove">
<button class="left">LEFT</button>
</form>

<form action="/rightmove">
<button class="right">RIGHT</button>
</form>

<form action="/stand">
<button class="stand">STAND</button>
</form>

<form action="/sit">
<button class="sit">SIT</button>
</form>

</div>

</div>

</body>
</html>
)====";

server.send(200,"text/html",page);

}

void forward() {
  server.sendHeader("Location","/");
  server.send(303);
  walkForward();
}

void backward() {
  server.sendHeader("Location","/");
  server.send(303);
  walkBackward();
}

void sit(){
    server.sendHeader("Location","/");
    server.send(303);
    Sit();
}

void stand(){
    server.sendHeader("Location","/");
    server.send(303);
    Stand();
}

void leftmove(){
    server.sendHeader("Location","/");
    server.send(303);
    Leftmove();
}

void rightmove(){
    server.sendHeader("Location","/");
    server.send(303);
    Rightmove();
}
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

WiFi.softAP(ssid,password);

Serial.print("IP Address: ");
Serial.println(WiFi.softAPIP());

server.on("/",root);
server.on("/forward",forward);
server.on("/backward",backward);
server.on("/sit",sit);
server.on("/stand",stand);
server.on("/leftmove",leftmove);
server.on("/rightmove",rightmove);

server.begin();

}

void loop() {
server.handleClient();
}
