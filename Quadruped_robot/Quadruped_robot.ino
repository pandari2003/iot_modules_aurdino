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

    delay(200);
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

    delay(200);
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
    for (int i = 90; i <= 110; i++)
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
    for (int i = 110; i >= 90; i--)
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
<style>
body{
font-family:Arial;
text-align:center;
margin-top:50px;
}
button{
width:180px;
height:70px;
font-size:24px;
margin:15px;
}
</style>
</head>

<body>

<h2>Quadruped robot controle</h2>

<form action="/forward">
<button>FORWARD</button>
</form>

<form action="/backward">
<button>BACKWARD</button>
</form>

<form action="/sit">
<button>SIT</button>
</form>

<form action="/stand">
<button>STAND</button>
</form>

<form action="/leftmove">
<button>LEFT TURN</button>
</form>

<form action="/rightmove">
<button>RIGHT TURN</button>
</form>
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

servoH1.attach(4);
servoH2.attach(5);
servoH3.attach(18);
servoH4.attach(19);

servoKN1.attach(46); 
servoKN2.attach(15); 
servoKN3.attach(16); 
servoKN4.attach(17); 
 
servoB1.attach(42);
servoB2.attach(41);
servoB3.attach(40);
servoB4.attach(39);

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
