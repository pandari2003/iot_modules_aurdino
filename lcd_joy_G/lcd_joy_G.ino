#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define JOYX A0
#define JOYY A1

const int WIDTH = 16;
const int HEIGHT = 2;
const int MAXLEN = 20;

int powerX[MAXLEN];
int powerY[MAXLEN];

int length;
int foodX, foodY;
int dx, dy;
int score;

//---------------------- Spawn Food ----------------------
void spawnFood()
{
  bool valid;

  do
  {
    valid = true;

    foodX = random(WIDTH);
    foodY = random(HEIGHT);

    for (int i = 0; i < length; i++)
    {
      if (powerX[i] == foodX && powerY[i] == foodY)
      {
        valid = false;
        break;
      }
    }

  } while (!valid);
}

//---------------------- Reset start ----------------------
void resetstart()
{
  length = 3;
  score = 0;

  powerX[0] = 5;
  powerY[0] = 0;

  powerX[1] = 4;
  powerY[1] = 0;

  powerX[2] = 3;
  powerY[2] = 0;

  dx = 1;
  dy = 0;

  spawnFood();

  lcd.clear();
}

//---------------------- start Over ----------------------
void startOver()
{
  lcd.clear();

  lcd.setCursor(3, 0);
  lcd.print("start OVER");

  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);

  delay(1000);

  resetstart();
}

//---------------------- Read Joystick ----------------------
void readJoystick()
{
  int x = analogRead(JOYX);
  int y = analogRead(JOYY);

  if (x < 300 && dx != 1)
  {
    dx = -1;
    dy = 0;
  }
  else if (x > 700 && dx != -1)
  {
    dx = 1;
    dy = 0;
  }
  else if (y < 300 && dy != 1)
  {
    dx = 0;
    dy = -1;
  }
  else if (y > 700 && dy != -1)
  {
    dx = 0;
    dy = 1;
  }
}

//---------------------- Move power ----------------------
void movepower()
{
  for (int i = length - 1; i > 0; i--)
  {
    powerX[i] = powerX[i - 1];
    powerY[i] = powerY[i - 1];
  }

  powerX[0] += dx;
  powerY[0] += dy;

  // Screen wrap
  if (powerX[0] < 0)
    powerX[0] = WIDTH - 1;

  if (powerX[0] >= WIDTH)
    powerX[0] = 0;

  if (powerY[0] < 0)
    powerY[0] = HEIGHT - 1;

  if (powerY[0] >= HEIGHT)
    powerY[0] = 0;
}

//---------------------- Check Collision ----------------------
bool hitBody()
{
  for (int i = 1; i < length; i++)
  {
    if (powerX[0] == powerX[i] &&
        powerY[0] == powerY[i])
    {
      return true;
    }
  }

  return false;
}

//---------------------- Food ----------------------
void checkFood()
{
  if (powerX[0] == foodX &&
      powerY[0] == foodY)
  {
    if (length < MAXLEN)
      length++;

    score++;

    spawnFood();
  }
}

//---------------------- Draw ----------------------
void drawstart()
{
  lcd.clear();

  // Food
  lcd.setCursor(foodX, foodY);
  lcd.print("*");

  // power
  for (int i = 0; i < length; i++)
  {
    lcd.setCursor(powerX[i], powerY[i]);

    if (i == 0)
      lcd.print("@");   // Head
    else
      lcd.print("o");   // Body
  }
}

//---------------------- Setup ----------------------
void setup()
{
  lcd.init();
  lcd.backlight();

  randomSeed(analogRead(A2));

  lcd.setCursor(2, 0);
  lcd.print("power start");

  lcd.setCursor(2, 1);
  lcd.print("Loading...");

  delay(2000);

  resetstart();
}

//---------------------- Loop ----------------------
void loop()
{
  readJoystick();

  movepower();

  if (hitBody())
  {
    startOver();
    return;
  }

  checkFood();

  drawstart();

  delay(250);
}