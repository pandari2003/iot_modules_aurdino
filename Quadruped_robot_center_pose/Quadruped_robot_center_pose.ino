#include <ESP32Servo.h>

Servo L1Hip1, L1Hip2, L1Knee;
Servo L2Hip1, L2Hip2, L2Knee;
Servo L3Hip1, L3Hip2, L3Knee;
Servo L4Hip1, L4Hip2, L4Knee;

void setup()
{
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    L1Hip1.attach(13);
    L1Hip2.attach(12);
    L1Knee.attach(14);

    L2Hip1.attach(11);
    L2Hip2.attach(10);
    L2Knee.attach(9);

    L3Hip1.attach(46);
    L3Hip2.attach(3);
    L3Knee.attach(8);

    L4Hip1.attach(18);
    L4Hip2.attach(17);
    L4Knee.attach(16);

    // Center position
    L1Hip1.write(120);
    L1Hip2.write(120);
    L1Knee.write(120);

    L2Hip1.write(120);
    L2Hip2.write(120);
    L2Knee.write(120);

    L3Hip1.write(120);
    L3Hip2.write(120);
    L3Knee.write(120);

    L4Hip1.write(120);
    L4Hip2.write(120);
    L4Knee.write(120);
}

void loop()
{
}