#include "esp_camera.h"
#include <WiFi.h>

// =====================================
// DFR1154 CAMERA PIN CONFIGURATION
// =====================================
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     5

#define Y9_GPIO_NUM       4
#define Y8_GPIO_NUM       6
#define Y7_GPIO_NUM       7
#define Y6_GPIO_NUM       14
#define Y5_GPIO_NUM       17
#define Y4_GPIO_NUM       21
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM       16

#define VSYNC_GPIO_NUM    1
#define HREF_GPIO_NUM     2
#define PCLK_GPIO_NUM     15

#define SIOD_GPIO_NUM     8
#define SIOC_GPIO_NUM     9

#define LED_GPIO_NUM      47

// Wi-Fi credentials
const char* WIFI_SSID = "Telezer_J";
const char* WIFI_PASSWORD = "Telezer12";

// These functions must be provided by the
// DFRobot CameraWebServer example files.
void startCameraServer();
void setupLedFlash(int pin);

// =====================================
// CONNECT TO WIFI AND PRINT IP
// =====================================
bool initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println();
  Serial.println("Connecting to WiFi...");
  Serial.print("SSID: ");
  Serial.println(WIFI_SSID);

  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("==============================");
    Serial.println("WIFI CONNECTED SUCCESSFULLY");
    Serial.print("IP ADDRESS: http://");
    Serial.println(WiFi.localIP());
    Serial.print("Signal strength (RSSI): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    Serial.println("==============================");

    return true;
  }

  Serial.println("ERROR: WiFi connection failed.");
  Serial.println("Check the SSID, password and 2.4 GHz WiFi.");
  return false;
}

// =====================================
// INITIALIZE CAMERA
// =====================================
bool initCamera() {
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
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;

  if (psramFound()) {
    Serial.println("PSRAM detected.");
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    Serial.println("PSRAM not detected; using internal RAM.");
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  Serial.println("Initializing camera...");

  esp_err_t err = esp_camera_init(&config);

  if (err != ESP_OK) {
    Serial.printf(
      "Camera initialization failed: 0x%x\n",
      err
    );
    return false;
  }

  sensor_t* sensor = esp_camera_sensor_get();

  if (sensor == nullptr) {
    Serial.println("Camera sensor unavailable.");
    return false;
  }

  if (sensor->id.PID == OV3660_PID) {
    sensor->set_vflip(sensor, 1);
    sensor->set_brightness(sensor, 1);
    sensor->set_saturation(sensor, -2);
    Serial.println("OV3660 sensor detected.");
  } else {
    Serial.printf(
      "Detected sensor PID: 0x%04X\n",
      sensor->id.PID
    );
  }

  sensor->set_framesize(sensor, FRAMESIZE_QVGA);

  Serial.println("Camera initialized successfully.");
  return true;
}

// =====================================
// SETUP
// =====================================
void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(1500);

  Serial.println();
  Serial.println("==================================");
  Serial.println("DFR1154 ESP32-S3 CAMERA");
  Serial.println("==================================");

  // Connect to WiFi FIRST so the IP can be
  // printed even if camera initialization fails.
  if (!initWiFi()) {
    Serial.println("IP unavailable: WiFi did not connect.");
    return;
  }

  // IP has already been printed above.
  // Now initialize the camera.
  if (!initCamera()) {
    Serial.println("WiFi is connected, but camera failed.");
    Serial.print("Your device IP is still: http://");
    Serial.println(WiFi.localIP());
    return;
  }

  setupLedFlash(LED_GPIO_NUM);

  Serial.println("Starting camera web server...");
  startCameraServer();

  Serial.println("Camera web server started.");
  Serial.print("Open this address: http://");
  Serial.println(WiFi.localIP());
}

// =====================================
// LOOP
// =====================================
void loop() {
  delay(1000);

  // Print IP again if WiFi is connected.
  // This helps confirm the address in Serial Monitor.
  static unsigned long lastPrint = 0;

  if (millis() - lastPrint >= 15000) {
    lastPrint = millis();

    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("Camera IP: http://");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("WiFi disconnected.");
    }
  }
}
