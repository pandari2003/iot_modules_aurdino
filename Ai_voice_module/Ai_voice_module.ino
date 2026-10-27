/*
INMP441 microphone
INMP441 pin	ESP32-C3
VDD	3.3 V
GND	GND
SCK / BCLK	GPIO 2
WS / LRCL	GPIO 3
SD	GPIO 4
L/R	GND for left channel


MAX98357A amplifier
MAX98357A pin	ESP32-C3
VIN	Suitable amplifier supply
GND	Common GND
BCLK	GPIO 2
LRC / WS	GPIO 3
DIN	GPIO 5
Speaker outputs	Speaker terminals


OLED and buttons
Component	ESP32-C3
OLED SDA	GPIO 8
OLED SCL	GPIO 9
Wake / talk button	GPIO 0 to GND
Volume up	GPIO 6 to GND
Volume down	GPIO 7 to GND
*/
#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <driver/i2s.h>

// ---------- USER SETTINGS ----------
const char *WIFI_SSID = "Telezer_J";
const char *WIFI_PASSWORD = "Telezer12";

// HTTPS backend URLs; replace with your deployed backend.
const char *ASK_URL = "http://172.28.213.135:8000/ask";
const char *TTS_URL = "http://172.28.213.135:8000/tts";

// Generate a long random token and configure the same value
// as DEVICE_TOKEN on your backend.
const char *DEVICE_TOKEN = "REPLACE_WITH_RANDOM_DEVICE_TOKEN";

// Paste the trusted root CA certificate for your backend's TLS
// certificate chain here. Do not leave this placeholder in use.
static const char ROOT_CA[] = R"PEM(
PASTE_YOUR_BACKEND_ROOT_CA_CERTIFICATE_HERE
)PEM";

// ---------- OLED ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define I2C_SDA 8
#define I2C_SCL 9

Adafruit_SSD1306 display(
  SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET
);

// ---------- I2S PINS ----------
#define I2S_SHARED_BCLK 2
#define I2S_SHARED_WS   3
#define I2S_MIC_SD      4
#define I2S_AMP_DIN     5

// ---------- BUTTONS ----------
#define BTN_WAKEUP   0
#define BTN_VOL_UP   6
#define BTN_VOL_DOWN 7

// ---------- AUDIO ----------
static constexpr uint32_t MIC_RATE = 16000;
static constexpr uint32_t TTS_RATE = 24000;
static constexpr uint32_t MAX_RECORD_SECONDS = 6;
static constexpr size_t MAX_PCM_BYTES =
    MIC_RATE * MAX_RECORD_SECONDS * sizeof(int16_t);

static uint8_t wavBuffer[44 + MAX_PCM_BYTES];
static size_t recordedBytes = 0;

int currentVolume = 70;

enum RobotState {
  IDLE, LISTENING, THINKING, SPEAKING, ERROR_STATE
};

volatile RobotState currentState = IDLE;
volatile bool workerBusy = false;
volatile bool recording = false;

portMUX_TYPE stateMux = portMUX_INITIALIZER_UNLOCKED;

char statusText[32] = "Booting";
char questionText[160] = "";
char answerText[320] = "";

bool displayReady = false;
bool micReady = false;
bool ampReady = false;

bool previousWakePressed = false;
bool previousUpPressed = false;
bool previousDownPressed = false;

uint32_t recordStartedAt = 0;
uint32_t lastDisplayAt = 0;
uint32_t lastWifiCheckAt = 0;

static const uint8_t WAV_HEADER_SIZE = 44;

// ---------- SHARED UI STATE ----------
void setStatus(RobotState state, const char *message) {
  portENTER_CRITICAL(&stateMux);
  currentState = state;
  snprintf(statusText, sizeof(statusText), "%s", message);
  portEXIT_CRITICAL(&stateMux);
}

void setQuestion(const char *text) {
  portENTER_CRITICAL(&stateMux);
  snprintf(questionText, sizeof(questionText), "%s", text);
  portEXIT_CRITICAL(&stateMux);
}

void setAnswer(const char *text) {
  portENTER_CRITICAL(&stateMux);
  snprintf(answerText, sizeof(answerText), "%s", text);
  portEXIT_CRITICAL(&stateMux);
}

// ---------- WAV HEADER ----------
void writeLE16(uint8_t *p, uint16_t v) {
  p[0] = v & 0xFF;
  p[1] = (v >> 8) & 0xFF;
}

void writeLE32(uint8_t *p, uint32_t v) {
  p[0] = v & 0xFF;
  p[1] = (v >> 8) & 0xFF;
  p[2] = (v >> 16) & 0xFF;
  p[3] = (v >> 24) & 0xFF;
}

void makeWavHeader(uint8_t *p, uint32_t dataBytes) {
  memcpy(p, "RIFF", 4);
  writeLE32(p + 4, 36 + dataBytes);
  memcpy(p + 8, "WAVE", 4);
  memcpy(p + 12, "fmt ", 4);
  writeLE32(p + 16, 16);
  writeLE16(p + 20, 1);            // PCM
  writeLE16(p + 22, 1);            // mono
  writeLE32(p + 24, MIC_RATE);
  writeLE32(p + 28, MIC_RATE * 2);
  writeLE16(p + 32, 2);
  writeLE16(p + 34, 16);
  memcpy(p + 36, "data", 4);
  writeLE32(p + 40, dataBytes);
}

// ---------- I2S CONFIGURATION ----------
bool stopI2S() {
  if (!micReady && !ampReady) return true;

  esp_err_t err = i2s_driver_uninstall(I2S_NUM_0);
  micReady = false;
  ampReady = false;

  if (err != ESP_OK) {
    Serial.printf("I2S uninstall error: %d\n", err);
    return false;
  }
  return true;
}

bool startMicrophone() {
  if (!stopI2S()) return false;

  i2s_config_t config = {};
  config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX);
  config.sample_rate = MIC_RATE;
  config.bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT;
  config.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
  config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
  config.dma_buf_count = 6;
  config.dma_buf_len = 128;
  config.use_apll = false;
  config.tx_desc_auto_clear = false;
  config.fixed_mclk = 0;

  if (i2s_driver_install(I2S_NUM_0, &config, 0, nullptr) != ESP_OK) {
    Serial.println("Microphone I2S install failed");
    return false;
  }

  i2s_pin_config_t pins = {};
  pins.bck_io_num = I2S_SHARED_BCLK;
  pins.ws_io_num = I2S_SHARED_WS;
  pins.data_out_num = I2S_PIN_NO_CHANGE;
  pins.data_in_num = I2S_MIC_SD;

  if (i2s_set_pin(I2S_NUM_0, &pins) != ESP_OK) {
    i2s_driver_uninstall(I2S_NUM_0);
    Serial.println("Microphone pin configuration failed");
    return false;
  }

  i2s_zero_dma_buffer(I2S_NUM_0);
  micReady = true;
  return true;
}

bool startSpeaker() {
  if (!stopI2S()) return false;

  i2s_config_t config = {};
  config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
  config.sample_rate = TTS_RATE;
  config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
  config.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
  config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
  config.dma_buf_count = 6;
  config.dma_buf_len = 256;
  config.use_apll = false;
  config.tx_desc_auto_clear = true;
  config.fixed_mclk = 0;

  if (i2s_driver_install(I2S_NUM_0, &config, 0, nullptr) != ESP_OK) {
    Serial.println("Speaker I2S install failed");
    return false;
  }

  i2s_pin_config_t pins = {};
  pins.bck_io_num = I2S_SHARED_BCLK;
  pins.ws_io_num = I2S_SHARED_WS;
  pins.data_out_num = I2S_AMP_DIN;
  pins.data_in_num = I2S_PIN_NO_CHANGE;

  if (i2s_set_pin(I2S_NUM_0, &pins) != ESP_OK) {
    i2s_driver_uninstall(I2S_NUM_0);
    Serial.println("Speaker pin configuration failed");
    return false;
  }

  i2s_zero_dma_buffer(I2S_NUM_0);
  ampReady = true;
  return true;
}

// ---------- OLED ----------
void initOLED() {
  Wire.begin(I2C_SDA, I2C_SCL);

  displayReady = display.begin(
    SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS
  );

  if (!displayReady) {
    Serial.println("OLED initialization failed");
    return;
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("ESP32-C3 AI ROBOT");
  display.println("Starting...");
  display.display();
}

void updateDisplay() {
  if (!displayReady) return;

  uint32_t now = millis();
  if (now - lastDisplayAt < 200) return;
  lastDisplayAt = now;

  char statusCopy[32];
  char questionCopy[160];
  char answerCopy[320];
  RobotState stateCopy;
  int volumeCopy;

  portENTER_CRITICAL(&stateMux);
  snprintf(statusCopy, sizeof(statusCopy), "%s", statusText);
  snprintf(questionCopy, sizeof(questionCopy), "%s", questionText);
  snprintf(answerCopy, sizeof(answerCopy), "%s", answerText);
  stateCopy = currentState;
  volumeCopy = currentVolume;
  portEXIT_CRITICAL(&stateMux);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextWrap(true);
  display.setCursor(0, 0);
  display.print("AI ROBOT | ");
  display.println(statusCopy);

  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
  display.setCursor(0, 13);

  if (stateCopy == LISTENING) {
    display.println("Speak now...");
    display.println("Release to send");
  } else if (strlen(answerCopy) > 0) {
    display.println("AI:");
    // A 128x64 display can only show a small part of a
    // long response at once.
    display.println(answerCopy);
  } else if (strlen(questionCopy) > 0) {
    display.println("You:");
    display.println(questionCopy);
  } else {
    display.println("Hold BOOT to talk");
    display.println("Wi-Fi voice assistant");
  }

  display.drawLine(0, 53, 127, 53, SSD1306_WHITE);
  display.setCursor(0, 56);
  display.printf("VOL %d%%  WiFi %s",
    volumeCopy, WiFi.isConnected() ? "OK" : "--");

  display.display();
}

// ---------- HTTPS HELPERS ----------
bool tlsConfigured() {
  return strlen(ROOT_CA) > 100 &&
         strstr(ROOT_CA, "BEGIN CERTIFICATE") != nullptr &&
         strstr(ROOT_CA, "PASTE_YOUR") == nullptr;
}

void setError(const char *message) {
  Serial.println(message);
  setStatus(ERROR_STATE, message);
}

bool validateBackendUrl(const char *url) {
  return url && strncmp(url, "https://", 8) == 0 &&
         strstr(url, "YOUR-BACKEND") == nullptr &&
         strlen(DEVICE_TOKEN) > 20 &&
         strstr(DEVICE_TOKEN, "REPLACE_") == nullptr;
}

// ---------- PLAY TTS AUDIO ----------
bool playTTS(const char *text) {
  if (!ampReady && !startSpeaker()) return false;

  WiFiClientSecure client;
  client.setCACert(ROOT_CA);

  HTTPClient http;
  http.setConnectTimeout(8000);
  http.setTimeout(25000);

  if (!http.begin(client, TTS_URL)) {
    Serial.println("TTS connection setup failed");
    return false;
  }

  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-Device-Token", DEVICE_TOKEN);

  JsonDocument request;
  request["text"] = text;
  String body;
  serializeJson(request, body);

int code = http.POST(
  (uint8_t *)body.c_str(),
  body.length()
);
  if (code != HTTP_CODE_OK) {
    Serial.printf("TTS HTTP error: %d\n", code);
    http.end();
    return false;
  }

  int remaining = http.getSize();
  WiFiClient *stream = http.getStreamPtr();

  // Read and skip the standard 44-byte PCM WAV header.
  uint8_t header[44];
  size_t headerRead = 0;
  uint32_t started = millis();

  while (headerRead < sizeof(header) &&
         millis() - started < 5000) {
    int available = stream->available();
    if (available > 0) {
      size_t wanted = min(
        (size_t)available, sizeof(header) - headerRead
      );
      int n = stream->readBytes(header + headerRead, wanted);
      if (n > 0) {
        headerRead += n;
        if (remaining > 0) remaining -= n;
      }
    } else {
      delay(1);
    }
  }

  if (headerRead != sizeof(header) ||
      memcmp(header, "RIFF", 4) != 0 ||
      memcmp(header + 8, "WAVE", 4) != 0) {
    Serial.println("Invalid WAV response");
    http.end();
    return false;
  }

  uint32_t sampleRate =
    (uint32_t)header[24] |
    ((uint32_t)header[25] << 8) |
    ((uint32_t)header[26] << 16) |
    ((uint32_t)header[27] << 24);

  uint16_t bitsPerSample =
    header[34] | ((uint16_t)header[35] << 8);
  uint16_t channels =
    header[22] | ((uint16_t)header[23] << 8);

  if (sampleRate != TTS_RATE ||
      bitsPerSample != 16 || channels != 1) {
    Serial.println("Unexpected TTS WAV format");
    http.end();
    return false;
  }

  uint8_t chunk[1024];
  uint32_t idleSince = millis();

  while (http.connected() &&
         (remaining > 0 || remaining == -1)) {
    size_t available = stream->available();

    if (available == 0) {
      if (remaining == 0) break;
      if (millis() - idleSince > 10000) break;
      delay(1);
      continue;
    }

    size_t count = min(available, sizeof(chunk));
    if (remaining > 0) count = min(count, (size_t)remaining);

    int n = stream->readBytes(chunk, count);
    if (n <= 0) continue;

    idleSince = millis();
    if (remaining > 0) remaining -= n;

    int16_t *samples = reinterpret_cast<int16_t *>(chunk);
    for (int i = 0; i + 1 < n; i += 2) {
      int32_t scaled = (int32_t)samples[i / 2] * currentVolume / 100;
      samples[i / 2] = (int16_t)constrain(scaled, -32768, 32767);
    }

    size_t written = 0;
    if (i2s_write(I2S_NUM_0, chunk, n, &written,
                  pdMS_TO_TICKS(100)) != ESP_OK) {
      Serial.println("I2S audio write failed");
      break;
    }
  }

  http.end();
  i2s_zero_dma_buffer(I2S_NUM_0);
  return true;
}

// ---------- SEND QUESTION TO BACKEND ----------
void aiWorker(void *parameter) {
  (void)parameter;

  setStatus(THINKING, "Transcribing");

  if (WiFi.status() != WL_CONNECTED) {
    setError("WiFi disconnected");
    workerBusy = false;
    vTaskDelete(nullptr);
    return;
  }

  if (!validateBackendUrl(ASK_URL) ||
      !validateBackendUrl(TTS_URL) || !tlsConfigured()) {
    setError("Check URL/token/CA");
    workerBusy = false;
    vTaskDelete(nullptr);
    return;
  }

  // Stop any I2S configuration before networking.
  stopI2S();

  WiFiClientSecure client;
  client.setCACert(ROOT_CA);

  HTTPClient http;
  http.setConnectTimeout(8000);
  http.setTimeout(45000);

  if (!http.begin(client, ASK_URL)) {
    setError("Backend connection failed");
    workerBusy = false;
    vTaskDelete(nullptr);
    return;
  }

  http.addHeader("Content-Type", "audio/wav");
  http.addHeader("X-Device-Token", DEVICE_TOKEN);

  size_t wavSize = WAV_HEADER_SIZE + recordedBytes;
  int code = http.POST(wavBuffer, wavSize);

  if (code != HTTP_CODE_OK) {
    Serial.printf("AI HTTP error: %d\n", code);
    http.end();
    setError("AI request failed");
    workerBusy = false;
    vTaskDelete(nullptr);
    return;
  }

  String response = http.getString();
  http.end();

  JsonDocument result;
  DeserializationError jsonError = deserializeJson(result, response);

  if (jsonError || !result["question"].is<const char *>() ||
      !result["answer"].is<const char *>()) {
    Serial.println("Invalid backend JSON response");
    setError("Invalid AI response");
    workerBusy = false;
    vTaskDelete(nullptr);
    return;
  }

  String question = result["question"].as<String>();
  String answer = result["answer"].as<String>();

  setQuestion(question.c_str());
  setAnswer(answer.c_str());

  if (answer.length() == 0) {
    setError("Empty AI answer");
    workerBusy = false;
    vTaskDelete(nullptr);
    return;
  }

  setStatus(SPEAKING, "Speaking");

  if (!playTTS(answer.c_str())) {
    setError("Speaker/TTS error");
    stopI2S();
    workerBusy = false;
    vTaskDelete(nullptr);
    return;
  }

  stopI2S();
  setStatus(IDLE, "Ready");
  workerBusy = false;
  vTaskDelete(nullptr);
}

// ---------- BUTTONS / RECORDING ----------
void beginRecording() {
  if (workerBusy || recording) return;

  if (WiFi.status() != WL_CONNECTED) {
    setError("WiFi unavailable");
    return;
  }

  setAnswer("");
  setQuestion("");
  recordedBytes = 0;

  if (!startMicrophone()) {
    setError("Microphone I2S error");
    return;
  }

  recording = true;
  recordStartedAt = millis();
  setStatus(LISTENING, "Listening");
  Serial.println("Recording started");
}

void finishRecording() {
  if (!recording) return;

  recording = false;
  stopI2S();

  if (recordedBytes < MIC_RATE / 2) {
    setStatus(IDLE, "Too short");
    return;
  }

  makeWavHeader(wavBuffer, recordedBytes);

  workerBusy = true;
  setStatus(THINKING, "Uploading");

  BaseType_t created = xTaskCreate(
    aiWorker, "AIWorker", 12288, nullptr, 1, nullptr
  );

  if (created != pdPASS) {
    workerBusy = false;
    setError("Worker task failed");
  }
}

void captureAudioChunk() {
  if (!recording || !micReady) return;

  // A small read keeps the main loop available for UI/buttons.
  int32_t raw[128];
  size_t bytesRead = 0;

  esp_err_t err = i2s_read(
    I2S_NUM_0, raw, sizeof(raw), &bytesRead, 0
  );

  if (err != ESP_OK || bytesRead == 0) return;

  size_t samples = bytesRead / sizeof(int32_t);
  int16_t *destination =
    reinterpret_cast<int16_t *>(wavBuffer + WAV_HEADER_SIZE);

  for (size_t i = 0; i < samples; i++) {
    if (recordedBytes + sizeof(int16_t) > MAX_PCM_BYTES) {
      finishRecording();
      return;
    }

    int32_t sample = raw[i] >> 16;
    destination[recordedBytes / 2] = (int16_t)sample;
    recordedBytes += sizeof(int16_t);
  }

  if (millis() - recordStartedAt >=
      MAX_RECORD_SECONDS * 1000UL) {
    finishRecording();
  }
}

void handleButtons() {
  bool wakePressed = digitalRead(BTN_WAKEUP) == LOW;
  bool upPressed = digitalRead(BTN_VOL_UP) == LOW;
  bool downPressed = digitalRead(BTN_VOL_DOWN) == LOW;

  // Push-to-talk: press to start; release to submit.
  if (wakePressed && !previousWakePressed) {
    beginRecording();
  }

  if (!wakePressed && previousWakePressed && recording) {
    finishRecording();
  }

  if (upPressed && !previousUpPressed) {
    portENTER_CRITICAL(&stateMux);
    currentVolume = min(currentVolume + 10, 100);
    portEXIT_CRITICAL(&stateMux);
  }

  if (downPressed && !previousDownPressed) {
    portENTER_CRITICAL(&stateMux);
    currentVolume = max(currentVolume - 10, 0);
    portEXIT_CRITICAL(&stateMux);
  }

  previousWakePressed = wakePressed;
  previousUpPressed = upPressed;
  previousDownPressed = downPressed;
}

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println("================================");
  Serial.println("ESP32-C3 AI ROBOT STARTING");
  Serial.println("================================");

  // Initialize buttons
  pinMode(BTN_WAKEUP, INPUT_PULLUP);
  pinMode(BTN_VOL_UP, INPUT_PULLUP);
  pinMode(BTN_VOL_DOWN, INPUT_PULLUP);

  // Initialize OLED
  initOLED();
  setStatus(IDLE, "Connecting WiFi");
  updateDisplay();

  // Configure Wi-Fi station mode
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.setAutoReconnect(true);

  Serial.print("Connecting to SSID: ");
  Serial.println(WIFI_SSID);

  // Start the connection only once
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint32_t started = millis();

  // Wait up to 20 seconds for the first connection
  while (WiFi.status() != WL_CONNECTED &&
         millis() - started < 20000UL) {
    updateDisplay();
    delay(100);
  }

  // Check connection result
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("Wi-Fi connected successfully!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");

    setStatus(IDLE, "Ready");
  } else {
    Serial.println();
    Serial.println("Initial Wi-Fi connection failed.");
    Serial.print("Wi-Fi status code: ");
    Serial.println((int)WiFi.status());

    setError("WiFi connection failed");
  }

  lastWifiCheckAt = millis();

  Serial.println("Setup complete.");
}



void loop() {
  // Handle wake and volume buttons
  handleButtons();

  // Capture microphone data while recording
  captureAudioChunk();

  // Refresh OLED display
  updateDisplay();

  // Do not call WiFi.begin() or WiFi.reconnect()
  // repeatedly here. Auto-reconnect is enabled in setup().

  delay(1);
}

