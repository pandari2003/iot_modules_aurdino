
#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

// WiFi credentials
const char* ssid = "Telezer_J";
const char* password = "Telezer12";

// AI Thinker ESP32-CAM pin configuration
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5

#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

WebServer server(80);

// Home page
void handleRoot() {
  String page =
    "<html><head><title>ESP32 CAM</title></head>"
    "<body style='text-align:center;background:#222;color:white;'>"
    "<h2>ESP32-CAM LIVE STREAM</h2>"
    "<img src='/stream' style='width:90%;max-width:640px;'>"
    "</body></html>";

  server.send(200, "text/html", page);
}

// Continuous MJPEG stream
void handleStream() {

  WiFiClient client = server.client();

  client.setNoDelay(true);
  client.setTimeout(1000);

  // HTTP MJPEG response header
  client.print(
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n"
    "Access-Control-Allow-Origin: *\r\n"
    "Cache-Control: no-cache, no-store, must-revalidate\r\n"
    "Pragma: no-cache\r\n"
    "Connection: close\r\n\r\n"
  );

  while (client.connected()) {

    camera_fb_t *fb = esp_camera_fb_get();

    if (!fb) {
      Serial.println("Camera capture failed");
      delay(10);
      continue;
    }

    // Send JPEG frame boundary and headers
    client.printf(
      "--frame\r\n"
      "Content-Type: image/jpeg\r\n"
      "Content-Length: %u\r\n\r\n",
      (unsigned int)fb->len
    );

    // Send image data
    size_t sent = client.write(fb->buf, fb->len);

    client.print("\r\n");

    // Return framebuffer immediately
    esp_camera_fb_return(fb);

    if (sent == 0) {
      break;
    }

    // Allow WiFi and watchdog tasks to run
    delay(1);
  }

  client.stop();
  Serial.println("Stream client disconnected");
}

// Single image capture
void handleCapture() {

  camera_fb_t *fb = esp_camera_fb_get();

  if (!fb) {
    server.send(500, "text/plain", "Camera capture failed");
    return;
  }

  server.sendHeader("Cache-Control", "no-store");
  server.setContentLength(fb->len);
  server.send(200, "image/jpeg", "");

  WiFiClient client = server.client();
  client.write(fb->buf, fb->len);

  esp_camera_fb_return(fb);
}

void setup() {

  Serial.begin(115200);
  delay(1000);

  // Camera configuration
  camera_config_t config = {};

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;

  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;

  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;

  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;

  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;

  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {

    Serial.println("PSRAM detected");

    config.frame_size = FRAMESIZE_HVGA;
    config.jpeg_quality = 5;
    config.fb_count = 15;
    config.grab_mode = CAMERA_GRAB_LATEST;
    config.fb_location = CAMERA_FB_IN_PSRAM;

  } else {

    Serial.println("PSRAM not detected");

    config.frame_size = FRAMESIZE_HVGA;
    config.jpeg_quality = 5;
    config.fb_count = 15;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  // Initialize camera
  esp_err_t err = esp_camera_init(&config);

  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x\n", err);
    return;
  }

  Serial.println("Camera initialized");

  // WiFi connection
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  unsigned long start = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - start < 20000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiFi connection failed");
    return;
  }

  Serial.println("\nWiFi connected!");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/stream", HTTP_GET, handleStream);
  server.on("/capture", HTTP_GET, handleCapture);

  server.begin();

  Serial.println("Streaming server started");
  Serial.print("Live stream: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/stream");

  Serial.print("Web page: http://");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();
}
