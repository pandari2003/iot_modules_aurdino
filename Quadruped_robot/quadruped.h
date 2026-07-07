#ifndef QUADRUPED_H
#define QUADRUPED_H

#include <ESP32Servo.h>

int walkSpeed = 10;

//---------------- Servo Objects ----------------//

Servo L1Hip1;
Servo L1Hip2;
Servo L1Knee;

Servo L2Hip1;
Servo L2Hip2;
Servo L2Knee;

Servo L3Hip1;
Servo L3Hip2;
Servo L3Knee;

Servo L4Hip1;
Servo L4Hip2;
Servo L4Knee;

//--------------- Pin Mapping ----------------//

#define L1_HIP1 4
#define L1_HIP2 2
#define L1_KNEE 5

#define L2_HIP1 18
#define L2_HIP2 19
#define L2_KNEE 21

#define L3_HIP1 34
#define L3_HIP2 35
#define L3_KNEE 32

#define L4_HIP1 33
#define L4_HIP2 25
#define L4_KNEE 26

//------------------------------------------------
enum Motion
{
    STOP,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

Motion currentMotion = STOP;

void setWalkSpeed(int s)
{
    walkSpeed = constrain(s,5,30);
}

void initServos()
{
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  L1Hip1.setPeriodHertz(50);
L1Hip1.attach(L1_HIP1, 500, 2400);
  L1Hip2.setPeriodHertz(50);
L1Hip2.attach(L1_HIP2, 500, 2400);
  L1Knee.setPeriodHertz(50);
L1Knee.attach(L1_KNEE, 500, 2400);

  L2Hip1.setPeriodHertz(50);
L2Hip1.attach(L2_HIP1, 500, 2400);
  L2Hip2.setPeriodHertz(50);
L2Hip2.attach(L2_HIP2, 500, 2400);
  L2Knee.setPeriodHertz(50);
L2Knee.attach(L2_KNEE, 500, 2400);

  L3Hip1.setPeriodHertz(50);
L3Hip1.attach(L3_HIP1, 500, 2400);
  L2Hip2.setPeriodHertz(50);
L3Hip2.attach(L3_HIP2, 500, 2400);
  L3Knee.setPeriodHertz(50);
L3Knee.attach(L3_KNEE, 500, 2400);

  L4Hip1.setPeriodHertz(50);
L4Hip1.attach(L4_HIP1, 500, 2400);
  L4Hip2.setPeriodHertz(50);
L4Hip2.attach(L4_HIP2, 500, 2400);
  L4Knee.setPeriodHertz(50);
L4Knee.attach(L4_KNEE, 500, 2400);
}

void centerServos()
{
  L1Hip1.write(90);
  L1Hip2.write(90);
  L1Knee.write(90);

  L2Hip1.write(90);
  L2Hip2.write(90);
  L2Knee.write(90);

  L3Hip1.write(90);
  L3Hip2.write(90);
  L3Knee.write(90);

  L4Hip1.write(90);
  L4Hip2.write(90);
  L4Knee.write(90);
}

void standPose()
{
    L1Hip2.write(80);
    L2Hip2.write(80);
    L3Hip2.write(80);
    L4Hip2.write(80);

    L1Knee.write(100);
    L2Knee.write(100);
    L3Knee.write(100);
    L4Knee.write(100);
}

void sitPose()
{
  L1Hip2.write(80);
  L2Hip2.write(80);
  L3Hip2.write(80);
  L4Hip2.write(80);

  L1Knee.write(120);
  L2Knee.write(120);
  L3Knee.write(120);
  L4Knee.write(120);
}

void forward()
{
    currentMotion = FORWARD;
}

void backward()
{
    currentMotion = BACKWARD;
}

void turnLeft()
{
    currentMotion = LEFT;
}

void turnRight()
{
    currentMotion = RIGHT;
}

//*********************************
void moveSmooth(Servo &servo, int from, int to)
{
    if(from < to)
    {
        for(int i=from;i<=to;i++)
        {
            servo.write(i);
            delay(walkSpeed);
        }
    }
    else
    {
        for(int i=from;i>=to;i--)
        {
            servo.write(i);
            delay(walkSpeed);
        }
    }
}

void forwardStep()
{
    // STEP 1 - Lift Front Left
    moveSmooth(L1Hip2,90,60);
    moveSmooth(L1Knee,90,60);

    // Swing Forward
    moveSmooth(L1Hip1,90,60);

    // Put Down
    moveSmooth(L1Hip2,60,90);
    moveSmooth(L1Knee,60,90);
     // STEP 2 - Lift Rear Right
    moveSmooth(L4Hip2,90,60);
    moveSmooth(L4Knee,90,60);

    moveSmooth(L4Hip1,90,120);

    moveSmooth(L4Hip2,60,90);
    moveSmooth(L4Knee,60,90);
     // Move body forward
    moveSmooth(L2Hip1,90,110);
    moveSmooth(L3Hip1,90,70);
     // STEP 3 - Lift Front Right
    moveSmooth(L2Hip2,90,60);
    moveSmooth(L2Knee,90,60);

    moveSmooth(L2Hip1,110,60);

    moveSmooth(L2Hip2,60,90);
    moveSmooth(L2Knee,60,90);
     // STEP 4 - Lift Rear Left
    moveSmooth(L3Hip2,90,60);
    moveSmooth(L3Knee,90,60);

    moveSmooth(L3Hip1,70,120);

    moveSmooth(L3Hip2,60,90);
    moveSmooth(L3Knee,60,90);
     // Return body
    moveSmooth(L1Hip1,60,90);
    moveSmooth(L2Hip1,60,90);
    moveSmooth(L3Hip1,120,90);
    moveSmooth(L4Hip1,120,90);
}

void backwardStep()
{
    // STEP 1 - Lift Front Left
    moveSmooth(L1Hip2,90,60);
    moveSmooth(L1Knee,90,60);

    // Swing Backward
    moveSmooth(L1Hip1,90,120);

    // Put Down
    moveSmooth(L1Hip2,60,90);
    moveSmooth(L1Knee,60,90);
     // STEP 2 - Lift Rear Right
    moveSmooth(L4Hip2,90,60);
    moveSmooth(L4Knee,90,60);

    moveSmooth(L4Hip1,90,60);

    moveSmooth(L4Hip2,60,90);
    moveSmooth(L4Knee,60,90);
     // Move body backward
    moveSmooth(L2Hip1,90,70);
    moveSmooth(L3Hip1,90,110);
     // STEP 3 - Lift Front Right
    moveSmooth(L2Hip2,90,60);
    moveSmooth(L2Knee,90,60);

    moveSmooth(L2Hip1,70,120);

    moveSmooth(L2Hip2,60,90);
    moveSmooth(L2Knee,60,90);
     // STEP 4 - Lift Rear Left
    moveSmooth(L3Hip2,90,60);
    moveSmooth(L3Knee,90,60);

    moveSmooth(L3Hip1,110,60);

    moveSmooth(L3Hip2,60,90);
    moveSmooth(L3Knee,60,90);
     // Return body to neutral
    moveSmooth(L1Hip1,120,90);
    moveSmooth(L2Hip1,120,90);
    moveSmooth(L3Hip1,60,90);
    moveSmooth(L4Hip1,60,90);
}

void leftStep()
{
    // STEP 1 - Front Left (short backward step)
    moveSmooth(L1Hip2,90,60);
    moveSmooth(L1Knee,90,60);

    moveSmooth(L1Hip1,90,110);

    moveSmooth(L1Hip2,60,90);
    moveSmooth(L1Knee,60,90);
     // STEP 2 - Rear Right (long forward step)
    moveSmooth(L4Hip2,90,60);
    moveSmooth(L4Knee,90,60);

    moveSmooth(L4Hip1,90,120);

    moveSmooth(L4Hip2,60,90);
    moveSmooth(L4Knee,60,90);
     // STEP 3 - Front Right (long forward step)
    moveSmooth(L2Hip2,90,60);
    moveSmooth(L2Knee,90,60);

    moveSmooth(L2Hip1,90,60);

    moveSmooth(L2Hip2,60,90);
    moveSmooth(L2Knee,60,90);
     // STEP 4 - Rear Left (short backward step)
    moveSmooth(L3Hip2,90,60);
    moveSmooth(L3Knee,90,60);

    moveSmooth(L3Hip1,90,70);

    moveSmooth(L3Hip2,60,90);
    moveSmooth(L3Knee,60,90);
     // Return to neutral
    moveSmooth(L1Hip1,110,90);
    moveSmooth(L2Hip1,60,90);
    moveSmooth(L3Hip1,70,90);
    moveSmooth(L4Hip1,120,90);
}

void rightStep()
{
    // STEP 1 - Front Left (long forward step)
    moveSmooth(L1Hip2,90,60);
    moveSmooth(L1Knee,90,60);

    moveSmooth(L1Hip1,90,60);

    moveSmooth(L1Hip2,60,90);
    moveSmooth(L1Knee,60,90);
     // STEP 2 - Rear Right (short backward step)
    moveSmooth(L4Hip2,90,60);
    moveSmooth(L4Knee,90,60);

    moveSmooth(L4Hip1,90,70);

    moveSmooth(L4Hip2,60,90);
    moveSmooth(L4Knee,60,90);
     // STEP 3 - Front Right (short backward step)
    moveSmooth(L2Hip2,90,60);
    moveSmooth(L2Knee,90,60);

    moveSmooth(L2Hip1,90,120);

    moveSmooth(L2Hip2,60,90);
    moveSmooth(L2Knee,60,90);
     // STEP 4 - Rear Left (long forward step)
    moveSmooth(L3Hip2,90,60);
    moveSmooth(L3Knee,90,60);

    moveSmooth(L3Hip1,90,120);

    moveSmooth(L3Hip2,60,90);
    moveSmooth(L3Knee,60,90);
     // Return to neutral
    moveSmooth(L1Hip1,60,90);
    moveSmooth(L2Hip1,120,90);
    moveSmooth(L3Hip1,120,90);
    moveSmooth(L4Hip1,70,90);
}

void stopWalk()
{
    // Return all Hip1 servos to center
    currentMotion = STOP;
    
    L1Hip1.write(90);
    L2Hip1.write(90);
    L3Hip1.write(90);
    L4Hip1.write(90);

    // Return all Hip2 servos to center
    L1Hip2.write(90);
    L2Hip2.write(90);
    L3Hip2.write(90);
    L4Hip2.write(90);

    // Return all Knee servos to center
    L1Knee.write(90);
    L2Knee.write(90);
    L3Knee.write(90);
    L4Knee.write(90);

    delay(100);
}

void moveServo(int servo,int angle)
{
  angle = constrain(angle,0,180);

  switch(servo)
  {
    case 1:L1Hip1.write(angle);break;
    case 2:L1Hip2.write(angle);break;
    case 3:L1Knee.write(angle);break;

    case 4:L2Hip1.write(angle);break;
    case 5:L2Hip2.write(angle);break;
    case 6:L2Knee.write(angle);break;

    case 7:L3Hip1.write(angle);break;
    case 8:L3Hip2.write(angle);break;
    case 9:L3Knee.write(angle);break;

    case 10:L4Hip1.write(angle);break;
    case 11:L4Hip2.write(angle);break;
    case 12:L4Knee.write(angle);break;
  }
}

void updateMotion()
{
    switch(currentMotion)
    {
        case FORWARD:
            forwardStep();
            break;

        case BACKWARD:
            backwardStep();
            break;

        case LEFT:
            leftStep();
            break;

        case RIGHT:
            rightStep();
            break;

        case STOP:
            break;
    }
}

#endif