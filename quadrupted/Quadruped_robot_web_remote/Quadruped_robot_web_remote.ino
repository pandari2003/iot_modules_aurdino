/* this board is esp32 and quadruptrobo board is aurdino r4 wifi
quadruptrobo ip address place in this place exp:
String robotIP = "192.168.1.25";
192.168.1.25 is quadruptrobo ip addresss
*/

#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Telezer_J";
const char* password = "Telezer12";

// Change to Arduino R4 IP
String robotIP = "192.168.29.244";

#define BTN_FORWARD 13
#define BTN_BACKWARD 12
#define BTN_LEFT 14
#define BTN_RIGHT 27  
#define BTN_STAND 26  
#define BTN_SIT 25  
#define BTN_HANDSHAKE 33 
#define BTN_SET 32  

void sendCommand(String cmd)
{
  HTTPClient http;

  String url = "http://" + robotIP + "/" + cmd;

  http.begin(url);
  http.GET();
  http.end();

  Serial.println(cmd);
}

void setup()
{
  Serial.begin(115200);

  pinMode(BTN_FORWARD, INPUT_PULLUP);
  pinMode(BTN_BACKWARD, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_STAND, INPUT_PULLUP);
  pinMode(BTN_SIT, INPUT_PULLUP);
  pinMode(BTN_HANDSHAKE, INPUT_PULLUP);
  pinMode(BTN_SET, INPUT_PULLUP);

  WiFi.begin(ssid,password);

  while(WiFi.status()!=WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connected");
}

void loop()
{
  if(!digitalRead(BTN_FORWARD))
  {
    sendCommand("forward");
    delay(300);
  }

  if(!digitalRead(BTN_BACKWARD))
  {
    sendCommand("backward");
    delay(300);
  }

  if(!digitalRead(BTN_LEFT))
  {
    sendCommand("left");
    delay(300);
  }

  if(!digitalRead(BTN_RIGHT))
  {
    sendCommand("right");
    delay(300);
  }

  if(!digitalRead(BTN_STAND))
  {    
    sendCommand("stand");
    delay(300);
  }
  

  if(!digitalRead(BTN_SIT)) 
  {
    sendCommand("sit");
    delay(300); 
  }

  if(!digitalRead(BTN_HANDSHAKE))
  {
    sendCommand("handshake");
    delay(300);
  }

  if(!digitalRead(BTN_SET))
  {
    sendCommand("set");
    delay(300);
  }
}