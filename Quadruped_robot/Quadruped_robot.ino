#include "wifid.h"
#include "quadruped.h"

void setup()
{
    Serial.begin(115200);

    initServos();
    centerServos();

    startWiFi();
}

void loop()
{
    handleClient();
    updateMotion();
}