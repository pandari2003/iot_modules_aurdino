
#include <WiFi.h>
#include <WiFiClient.h>
#include <LovyanGFX.hpp>
#include <TJpg_Decoder.h>

// =====================================================
// 1. WIFI AND CAMERA SETTINGS
// =====================================================

const char* WIFI_SSID = "Telezer_J";
const char* WIFI_PASSWORD = "Telezer12";

// ESP32-CAM IP from Serial Monitor
const char* CAMERA_IP = "192.168.29.197";
const uint16_t CAMERA_PORT = 80;

// MJPEG endpoint
const char* CAMERA_PATH = "/stream";

// =====================================================
// 2. WT32-SC01 DISPLAY CONFIGURATION
// =====================================================

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7796 _panel;
  lgfx::Bus_SPI _bus;

public:
  LGFX() {
    {
      auto cfg = _bus.config();

      cfg.spi_host = SPI2_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 40000000;
      cfg.freq_read = 16000000;

      cfg.pin_sclk = 14;
      cfg.pin_mosi = 13;
      cfg.pin_miso = -1;
      cfg.pin_dc = 21;

      cfg.dma_channel = 1;
      cfg.use_lock = true;

      _bus.config(cfg);
      _panel.setBus(&_bus);
    }

    {
      auto cfg = _panel.config();

      cfg.pin_cs = 15;
      cfg.pin_rst = 22;
      cfg.pin_busy = -1;

      cfg.memory_width = 320;
      cfg.memory_height = 480;

      cfg.panel_width = 320;
      cfg.panel_height = 480;

      cfg.offset_x = 0;
      cfg.offset_y = 0;

      cfg.offset_rotation = 0;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;

      cfg.readable = false;
      cfg.invert = false;
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = false;

      _panel.config(cfg);
    }

    setPanel(&_panel);
  }
};

LGFX tft;

#define TFT_BACKLIGHT 23

// =====================================================
// 3. CAMERA AND JPEG BUFFER
// =====================================================

WiFiClient cameraClient;

uint8_t* jpegBuffer = nullptr;

// Increase if your camera sends larger JPEG frames.
const size_t JPEG_BUFFER_SIZE = 100000;

// 480 x 320 landscape display
// Centered 320 x 240 camera image
const int IMAGE_X = 80;
const int IMAGE_Y = 40;

// =====================================================
// 4. JPEG DISPLAY CALLBACK
// =====================================================

bool jpegOutput(
  int16_t x,
  int16_t y,
  uint16_t width,
  uint16_t height,
  uint16_t* bitmap
) {
  if (x >= tft.width() || y >= tft.height()) {
    return true;
  }

  if (x + width > tft.width()) {
    width = tft.width() - x;
  }

  if (y + height > tft.height()) {
    height = tft.height() - y;
  }

  tft.pushImage(x, y, width, height, bitmap);

  return true;
}

// =====================================================
// 5. RAW STREAM BYTE READER
// No HTTP chunked decoding required.
// =====================================================

bool readStreamByte(uint8_t& value, uint32_t timeout = 10000) {

  unsigned long start = millis();

  while (cameraClient.available() == 0) {

    if (!cameraClient.connected()) {
      return false;
    }

    if (millis() - start > timeout) {
      Serial.println("Stream read timeout");
      return false;
    }

    delay(1);
  }

  int result = cameraClient.read();

  if (result < 0) {
    return false;
  }

  value = (uint8_t)result;

  return true;
}

// =====================================================
// 6. READ LINE FROM RAW MJPEG STREAM
// =====================================================

bool readStreamLine(String& line) {

  line = "";

  uint8_t ch;

  while (readStreamByte(ch)) {

    if (ch == '\n') {
      return true;
    }

    if (ch != '\r') {
      line += (char)ch;
    }

    if (line.length() > 300) {
      return false;
    }
  }

  return false;
}

// =====================================================
// 7. READ EXACT JPEG DATA
// =====================================================

bool readJPEGData(uint8_t* destination, size_t length) {

  size_t received = 0;

  unsigned long lastDataTime = millis();

  while (received < length) {

    int availableBytes = cameraClient.available();

    if (availableBytes > 0) {

      size_t remaining = length - received;

      size_t toRead = min(
        (size_t)availableBytes,
        remaining
      );

      int count = cameraClient.read(
        destination + received,
        toRead
      );

      if (count > 0) {
        received += count;
        lastDataTime = millis();
      }

    } else {

      if (!cameraClient.connected()) {
        return false;
      }

      if (millis() - lastDataTime > 15000) {
        Serial.println("JPEG receive timeout");
        return false;
      }

      delay(1);
    }
  }

  return true;
}

// =====================================================
// 8. READ HTTP RESPONSE HEADERS
// =====================================================

bool readHTTPHeaders() {

  cameraClient.setTimeout(5000);

  String line = cameraClient.readStringUntil('\n');
  line.trim();

  Serial.print("HTTP Status: ");
  Serial.println(line);

  if (!line.startsWith("HTTP/1.1 200") &&
      !line.startsWith("HTTP/1.0 200")) {

    Serial.println("Camera HTTP request failed");
    return false;
  }

  while (cameraClient.connected()) {

    line = cameraClient.readStringUntil('\n');
    line.trim();

    if (line.length() == 0) {
      break;
    }

    Serial.println(line);
  }

  Serial.println("HTTP headers received");

  return true;
}

// =====================================================
// 9. CONNECT TO ESP32-CAM
// =====================================================

bool connectCamera() {

  Serial.println();
  Serial.println("Connecting to ESP32-CAM...");

  cameraClient.stop();

  if (!cameraClient.connect(CAMERA_IP, CAMERA_PORT)) {

    Serial.println("Camera TCP connection failed");
    return false;
  }

  cameraClient.setNoDelay(true);
  cameraClient.setTimeout(5000);

  // Request MJPEG stream
  cameraClient.print("GET ");
  cameraClient.print(CAMERA_PATH);
  cameraClient.println(" HTTP/1.1");

  cameraClient.print("Host: ");
  cameraClient.println(CAMERA_IP);

  cameraClient.println("Accept: multipart/x-mixed-replace");
  cameraClient.println("Connection: keep-alive");
  cameraClient.println();

  if (!readHTTPHeaders()) {

    cameraClient.stop();
    return false;
  }

  Serial.println("ESP32-CAM stream connected!");

  tft.fillScreen(TFT_BLACK);

  return true;
}

// =====================================================
// 10. RECEIVE AND DISPLAY ONE MJPEG FRAME
// =====================================================

bool receiveMJPEGFrame() {

  String line;
  int contentLength = 0;

  // Find multipart boundary
  while (readStreamLine(line)) {

    if (line.startsWith("--frame")) {
      break;
    }
  }

  if (!line.startsWith("--frame")) {

    Serial.println("MJPEG boundary missing");
    return false;
  }

  // Read JPEG frame headers
  while (readStreamLine(line)) {

    if (line.length() == 0) {
      break;
    }

    if (line.startsWith("Content-Length:") ||
        line.startsWith("content-length:")) {

      int colon = line.indexOf(':');

      contentLength = line.substring(colon + 1).toInt();
    }
  }

  if (contentLength <= 0) {

    Serial.println("Invalid JPEG length");
    return false;
  }

  if ((size_t)contentLength > JPEG_BUFFER_SIZE) {

    Serial.printf(
      "JPEG too large: %d bytes\n",
      contentLength
    );

    return false;
  }

  // Receive JPEG image directly
  if (!readJPEGData(
        jpegBuffer,
        (size_t)contentLength
      )) {

    Serial.println("JPEG reception failed");
    return false;
  }

  // Decode and display
  JRESULT result = TJpgDec.drawJpg(
    IMAGE_X,
    IMAGE_Y,
    jpegBuffer,
    (uint32_t)contentLength
  );

  if (result != JDR_OK) {

    Serial.print("JPEG decoder error: ");
    Serial.println((int)result);

    return false;
  }

  return true;
}

// =====================================================
// 11. SETUP
// =====================================================

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("WT32-SC01 LIVE CAMERA RECEIVER");

  // Backlight
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH);

  // Display initialization
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 20);
  tft.println("Camera Receiver");

  // JPEG decoder
  TJpgDec.setCallback(jpegOutput);
  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);

  // Allocate JPEG buffer
  Serial.println("Checking memory...");

  Serial.printf(
    "Free heap: %u\n",
    ESP.getFreeHeap()
  );

  Serial.printf(
    "PSRAM: %s\n",
    psramFound() ? "YES" : "NO"
  );

  if (psramFound()) {
    jpegBuffer = (uint8_t*)ps_malloc(JPEG_BUFFER_SIZE);
  }

  if (jpegBuffer == nullptr) {
    jpegBuffer = (uint8_t*)malloc(JPEG_BUFFER_SIZE);
  }

  if (jpegBuffer == nullptr) {

    Serial.println("JPEG memory allocation failed");

    tft.fillScreen(TFT_RED);
    tft.setCursor(20, 100);
    tft.println("Memory Error");

    while (true) {
      delay(1000);
    }
  }

  Serial.println("JPEG buffer allocated");

  // Wi-Fi connection
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  tft.setCursor(20, 70);
  tft.println("Connecting WiFi");

  Serial.print("Connecting to WiFi");

  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - startTime < 20000) {

    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("\nWiFi connection failed");

    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 100);
    tft.println("WiFi Failed");

    return;
  }

  Serial.println("\nWiFi connected");

  Serial.print("Receiver IP: ");
  Serial.println(WiFi.localIP());

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(20, 20);
  tft.println("WiFi Connected");

  delay(1000);

  // Connect to camera
  connectCamera();
}

// =====================================================
// 12. MAIN LOOP
// =====================================================

void loop() {

  // Check Wi-Fi
  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("WiFi disconnected");

    cameraClient.stop();

    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    delay(3000);
    return;
  }

  // Check camera connection
  if (!cameraClient.connected()) {

    Serial.println("Camera disconnected");

    cameraClient.stop();

    delay(1000);

    connectCamera();
    return;
  }

  // Receive and display frames continuously
  if (!receiveMJPEGFrame()) {

    Serial.println("Frame error. Reconnecting...");

    cameraClient.stop();

    delay(500);

    connectCamera();
  }
}
