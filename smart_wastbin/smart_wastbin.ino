#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
 
// WiFi Credentials 
const char* ssid = "Telezer_J";
const char* password = "Telezer12";
 
// DHT Settings 
#define DHTPIN 4
#define DHTTYPE DHT11      // Change to DHT22 if needed

DHT dht(DHTPIN, DHTTYPE);
 
// Ultrasonic Pins 
#define TRIG_PIN 5
#define ECHO_PIN 18
 
// Metal Sensor 
#define METAL_PIN 27
//Buzzer indicator
#define buzzer 1

WebServer server(80);
 
// Read Distance 
float getDistance()
{  digitalWrite(TRIG_PIN, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);  delayMicroseconds(10);  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.0343 / 2;
  return distance;
}
 
// Bin Status 
String binStatus(float distance)
{  if(distance > 20){
  return "EMPTY";
}
  
  else if(distance > 10){
    return "HALF FULL";
  }   
  else{
    return "FULL";
     tone(buzzer, 1000);   // 1 kHz sound
  }
  
}
 
// Web Page 
void handleRoot()
{
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float distance = getDistance();

  int metal = digitalRead(METAL_PIN);

  String metalStatus = (metal == LOW) ? "YES" : "NO";
  String humidityStatus = (humidity > 80) ? "HIGH" : "NORMAL";

  String page = "";

  page += "<!DOCTYPE html>";
  page += "<html>";
  page += "<head>";
  page += "<meta http-equiv='refresh' content='3'>";
  page += "<title>Smart Waste Bin</title>";

  page += "<style>";
  page += "body{background:#f4f4f4;font-family:Arial;text-align:center;}";
  page += ".card{width:420px;margin:auto;background:white;padding:20px;";
  page += "margin-top:40px;border-radius:15px;box-shadow:0px 0px 15px gray;}";
  page += "h1{color:#008000;}";
  page += ".data{font-size:24px;color:#0066ff;}";
  page += "</style>";

  page += "</head>";
  page += "<body>";

  page += "<div class='card'>";

  page += "<h1>SMART WASTE BIN</h1>";
  page += "<hr>";

  page += "<h3>Bin Status</h3>";
  page += "<div class='data'>";
  page += binStatus(distance);
  page += "</div>";

  page += "<h3>Distance</h3>";
  page += "<div class='data'>";
  page += String(distance, 1);
  page += " cm</div>";

  page += "<h3>Temperature</h3>";
  page += "<div class='data'>";
  page += String(temperature, 1);
  page += " &deg;C</div>";

  page += "<h3>Humidity</h3>";
  page += "<div class='data'>";
  page += String(humidity, 1);
  page += " %</div>";

  page += "<h3>Metal Detected</h3>";
  page += "<div class='data'>";
  page += metalStatus;
  page += "</div>";

  page += "<h3>Humidity Status</h3>";
  page += "<div class='data'>";
  page += humidityStatus;
  page += "</div>";

  page += "</div>";

  page += "</body>";
  page += "</html>";

  server.send(200, "text/html", page);
}
 
// Setup 
void setup()
{  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);  pinMode(ECHO_PIN, INPUT);
  pinMode(METAL_PIN, INPUT);
  dht.begin();
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while(WiFi.status()!=WL_CONNECTED)  {    delay(500);    Serial.print(".");  }
  Serial.println();  Serial.println("Connected");
  Serial.print("IP Address: ");  Serial.println(WiFi.localIP());
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Web Server Started");
}
 
// Loop 
void loop()
{  server.handleClient();
}

/*

Add these libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
3. Create the OLED object
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
4. Initialize the OLED in setup()
Wire.begin();

if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
{
  Serial.println("OLED not found");
  while(true);
}

display.clearDisplay();
display.setTextColor(SSD1306_WHITE);
display.setTextSize(1);
display.display();
5. Create a function to update the OLED
void updateOLED(float temperature, float humidity, float distance, String binStatus, String metalStatus)
{
  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(0,0);
  display.println("SMART WASTE BIN");

  display.setCursor(0,15);
  display.print("Bin : ");
  display.println(binStatus);

  display.setCursor(0,25);
  display.print("Dist: ");
  display.print(distance,1);
  display.println(" cm");

  display.setCursor(0,35);
  display.print("Temp: ");
  display.print(temperature,1);
  display.println(" C");

  display.setCursor(0,45);
  display.print("Hum : ");
  display.print(humidity,0);
  display.println("%");

  display.setCursor(0,55);
  display.print("Metal: ");
  display.println(metalStatus);

  display.display();
}
6. Call the function in handleRoot()

After reading the sensors:

String bin = "FULL";

if(distance > 20)
  bin = "EMPTY";
else if(distance > 10)
  bin = "HALF FULL";

String metalStatus = (metal == LOW) ? "YES" : "NO";

Call:

updateOLED(temperature, humidity, distance, bin, metalStatus);

*/