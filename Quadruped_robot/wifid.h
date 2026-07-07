#ifndef WIFID_H
#define WIFID_H
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

#include "web.h"
#include "quadruped.h"

const char* ssid="Quadruped";
const char* password="12345678";

WebServer server(80);

Preferences preferences;

void saveWiFi(String ssid,String pass)
{
}

void loadWiFi()
{
}

void connectWiFi()
{
}

void startAPMode()
{
}
void handleRoot()
{
  server.send_P(200,"text/html",webpage);
}

void handleCenter()
{
  centerServos();
  server.send(200,"text/plain","OK");
}

void handleStand()
{
  standPose();
  server.send(200,"text/plain","OK");
}

void handleSit()
{
  sitPose();
  server.send(200,"text/plain","OK");
}

void handleForward()
{
    forward();
    server.send(200,"text/plain","OK");
}

void handleBackward()
{
    backward();
    server.send(200,"text/plain","OK");
}

void handleLeft()
{
    turnLeft();
    server.send(200,"text/plain","OK");
}

void handleRight()
{
    turnRight();
    server.send(200,"text/plain","OK");
}

void handleStop()
{
    stopWalk();
    server.send(200,"text/plain","OK");
}

void handleSpeed()
{
    int s = server.arg("value").toInt();
    setWalkSpeed(s);
    server.send(200,"text/plain","OK");
}

void handleMove()
{
  int servo=server.arg("servo").toInt();
  int angle=server.arg("angle").toInt();

  moveServo(servo,angle);

  server.send(200,"text/plain","Done");
}

void startWiFi()
{
  WiFi.mode(WIFI_AP);

  WiFi.softAP(ssid,password);

  Serial.println(WiFi.softAPIP());
  server.on("/",handleRoot);

  server.on("/center",handleCenter);

  server.on("/stand",handleStand);

  server.on("/sit",handleSit);

  server.on("/forward",handleForward);

  server.on("/backward",handleBackward);

  server.on("/left",handleLeft);

  server.on("/right",handleRight);

  server.on("/stop",handleStop);

  server.on("/speed",handleSpeed);

  server.on("/move",handleMove);

  server.begin();
}

void handleClient()
{
  server.handleClient();
}


#endif