#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <LovyanGFX.hpp>
#include <Preferences.h>

// =====================================================
// 1. USER CONFIGURATION
// =====================================================

String ssid = "";
String password = "";

String mqtt_server = "192.168.29.146";
uint16_t mqtt_port = 8883;
String mqtt_user = "bblp";
String mqtt_pass = "";
String serial_num = "";

Preferences preferences;

// =====================================================
// 2. WT32-SC01 DISPLAY CONFIGURATION
// =====================================================


class LGFX : public lgfx::LGFX_Device {

  lgfx::Panel_ST7796 _panel_instance;
  lgfx::Bus_SPI _bus_instance;
  lgfx::Light_PWM _light_instance;
  lgfx::Touch_FT5x06 _touch_instance;

public:

  LGFX(void) {

    {
      auto cfg = _bus_instance.config();

      cfg.spi_host = HSPI_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 40000000;
      cfg.freq_read = 20000000;

      cfg.pin_sclk = 14;
      cfg.pin_mosi = 13;
      cfg.pin_miso = 12;
      cfg.pin_dc = 21;

      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    {
      auto cfg = _panel_instance.config();

      cfg.pin_cs = 15;
      cfg.pin_rst = 22;
      cfg.pin_busy = -1;

      cfg.panel_width = 320;
      cfg.panel_height = 480;

      _panel_instance.config(cfg);
    }

    {
      auto cfg = _light_instance.config();

      cfg.pin_bl = 23;
      cfg.invert = false;
      cfg.freq = 44100;
      cfg.pwm_channel = 7;

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    {
      auto cfg = _touch_instance.config();

      cfg.x_min = 0;
      cfg.x_max = 319;
      cfg.y_min = 0;
      cfg.y_max = 479;

      cfg.pin_int = -1;
      cfg.bus_shared = false;
      cfg.offset_rotation = 0;

      cfg.i2c_port = 0;
      cfg.i2c_addr = 0x38;
      cfg.pin_sda = 18;
      cfg.pin_scl = 19;
      cfg.freq = 400000;

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }

    setPanel(&_panel_instance);
  }
};

LGFX tft;

WiFiClientSecure espClient;
PubSubClient client(espClient);

// =====================================================
// 3. UI STATE MACHINE
// =====================================================

enum UIState {
  STATE_IDLE,
  STATE_PRINTING,
  STATE_COMPLETED
};

UIState currentUIState = STATE_IDLE;
UIState lastRenderedState = (UIState)-1;

unsigned long completedStartTime = 0;

const unsigned long COMPLETED_DISPLAY_MS = 60000;

// =====================================================
// 4. PRINTER STATUS VARIABLES
// =====================================================

String printState = "IDLE";
String prevPrintState = "";

String partName = "No Active Print";

int progress = 0;

float nozzleTemp = 0.0;
float nozzleTarget = 0.0;

float bedTemp = 0.0;
float bedTarget = 0.0;

float chamberTemp = 0.0;

int currentLayer = 0;
int totalLayers = 0;
int remainingMins = 0;

// =====================================================
// 5. STATUS BAR AND CONNECTION VARIABLES
// =====================================================

String runtimeError = "";
String lastStatusDrawn = "";

uint16_t lastStatusDrawnColor = 0xFFFF;

bool wifiEverConnected = false;
bool mqttAttempted = false;

unsigned long lastMqttAttempt = 0;
unsigned long lastWifiRetry = 0;

const unsigned long MQTT_RETRY_INTERVAL = 3000;
const unsigned long WIFI_RETRY_INTERVAL = 10000;

// =========================================
// DISPLAY REFRESH CONTROL
// =========================================

bool screenUpdatePending = false;

unsigned long lastScreenUpdate = 0;

const unsigned long SCREEN_REFRESH_INTERVAL = 300;

// =====================================================
// 6. TOUCHSCREEN SETTINGS VARIABLES
// =====================================================

enum ScreenMode {
  SCREEN_HOME,
  SCREEN_SETTINGS,
  SCREEN_KEYBOARD
};

ScreenMode screenMode = SCREEN_HOME;

enum SettingField {
  FIELD_NONE,
  FIELD_WIFI_SSID,
  FIELD_WIFI_PASSWORD,
  FIELD_SERVER,
  FIELD_PORT,
  FIELD_USER,
  FIELD_PASSWORD,
  FIELD_SERIAL
};

SettingField activeField = FIELD_NONE;

// Temporary settings.
// Actual MQTT settings are changed only after UPDATE.

String draftServer;
String draftPort;
String draftUser;
String draftPassword;
String draftSerial;

String editBuffer = "";

String draftWifiSSID;
String draftWifiPassword;

bool keyboardUppercase = false;

int touchX = 0;
int touchY = 0;

unsigned long lastTouchTime = 0;
const unsigned long TOUCH_DEBOUNCE = 250;

// =====================================================
// 7. SETTINGS STORAGE
// =====================================================
void loadMQTTSettings() {

  if (!preferences.begin("bambu", true)) {
    Serial.println("ERROR: Cannot open settings storage!");
    return;
  }

  ssid        = preferences.getString("wifi_ssid", ssid);
  password    = preferences.getString("wifi_pass", password);

  mqtt_server = preferences.getString("server", mqtt_server);
  mqtt_port   = preferences.getUShort("port", mqtt_port);
  mqtt_user   = preferences.getString("user", mqtt_user);
  mqtt_pass   = preferences.getString("pass", mqtt_pass);
  serial_num  = preferences.getString("serial", serial_num);

  preferences.end();

  Serial.println("===== SAVED SETTINGS LOADED =====");
  Serial.println("WiFi SSID: " + ssid);
  Serial.println("MQTT Server: " + mqtt_server);
  Serial.println("MQTT Port: " + String(mqtt_port));
  Serial.println("MQTT User: " + mqtt_user);
  Serial.println("Printer Serial: " + serial_num);
  Serial.println("=================================");
}

void saveMQTTSettings() {

  if (!preferences.begin("bambu", false)) {
    Serial.println("ERROR: Cannot open settings storage!");
    return;
  }

  preferences.putString("wifi_ssid", ssid);
  preferences.putString("wifi_pass", password);

  preferences.putString("server", mqtt_server);
  preferences.putUShort("port", mqtt_port);
  preferences.putString("user", mqtt_user);
  preferences.putString("pass", mqtt_pass);
  preferences.putString("serial", serial_num);

  preferences.end();

  Serial.println("===== SETTINGS SAVED TO FLASH =====");
}
// =====================================================
// 8. DRAW SETTINGS ICON
// =====================================================

void drawSettingsIcon() {

  // Button background
  tft.fillRoundRect(
    430, 5, 42, 35, 5, TFT_DARKGREEN);

  // Gear-like icon
  tft.drawCircle(451, 22, 9, TFT_WHITE);
  tft.drawCircle(451, 22, 4, TFT_WHITE);

  tft.drawFastVLine(451, 9, 5, TFT_WHITE);
  tft.drawFastVLine(451, 30, 5, TFT_WHITE);

  tft.drawFastHLine(438, 22, 5, TFT_WHITE);
  tft.drawFastHLine(459, 22, 5, TFT_WHITE);
}

// =====================================================
// 9. DRAW STATIC UI
// =====================================================


void drawStaticUI() {

  tft.fillScreen(TFT_BLACK);

  // Header: 480 x 45
  tft.fillRect(0, 0, 480, 45, 0x05E0);

  tft.setTextColor(TFT_WHITE, 0x05E0);
  tft.setTextSize(2);
  tft.setCursor(12, 14);
  tft.print("BAMBU LAB X1C");

  drawSettingsIcon();

  // Left-side progress
  tft.drawRect(15, 115, 245, 26, TFT_WHITE);

  // Left-side divider
  tft.drawFastHLine(15, 185, 245, TFT_DARKGREY);

  // Right-side active part card
  tft.drawRoundRect(
    275, 60, 190, 240, 8, TFT_DARKGREY
  );

  tft.fillRoundRect(
    275, 60, 190, 34, 8, 0x18E3
  );

  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_WHITE, 0x18E3);
  tft.setTextSize(2);

  tft.drawString("ACTIVE PART", 370, 77);
}


// =====================================================
// 10. DRAW PRINTING ICON
// =====================================================


void drawPrintingIcon(int cx, int cy) {

  // Clear only the illustration area
  tft.fillRect(285, 100, 170, 115, TFT_BLACK);

  // Printer frame
  tft.drawRoundRect(
    cx - 48, cy - 43, 96, 82, 7, TFT_CYAN
  );

  // Vertical frame supports
  tft.drawLine(cx - 36, cy - 32, cx - 36, cy + 25, TFT_WHITE);
  tft.drawLine(cx + 36, cy - 32, cx + 36, cy + 25, TFT_WHITE);

  // Print head
  tft.fillRoundRect(
    cx - 13, cy - 29, 26, 10, 3, TFT_ORANGE
  );

  tft.fillTriangle(
    cx - 6, cy - 19,
    cx + 6, cy - 19,
    cx, cy - 12,
    TFT_ORANGE
  );

  // Build plate
  tft.drawFastHLine(
    cx - 32, cy + 13, 64, TFT_WHITE
  );

  // Printed object
  tft.drawRect(
    cx - 15, cy - 7, 30, 20, TFT_GREEN
  );

  tft.drawFastHLine(
    cx - 15, cy, 30, TFT_GREEN
  );

  tft.drawFastHLine(
    cx - 15, cy + 7, 30, TFT_GREEN
  );

  // Printer base
  tft.fillRoundRect(
    cx - 48, cy + 40, 96, 7, 3, TFT_DARKGREY
  );

  // Printing status
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);

  tft.drawString(
    "PRINTING", cx, cy + 65
  );
}


// =====================================================
// 11. DRAW QUESTION MARK
// =====================================================

void drawQuestionMark(int cx, int cy) {

  tft.fillRect(
    280, 98, 180, 172, TFT_BLACK);

  tft.setTextColor(0x05E0, TFT_BLACK);
  tft.setTextSize(9);
  tft.setTextPadding(0);

  tft.drawString("?", cx - 22, cy - 35);
}

// =====================================================
// 12. SHOW STATUS BAR - FLICKER FREE
// =====================================================

void showStatusBar(String message, uint16_t color) {

  if (message.length() > 25) {
    message = message.substring(0, 25);
  }

  // Do not redraw if nothing has changed
  if (message == lastStatusDrawn &&
      color == lastStatusDrawnColor) {
    return;
  }

  tft.startWrite();

  tft.fillRect(0, 0, 480, 45, color);

  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_WHITE, color);
  tft.setTextSize(2);
  tft.setTextPadding(0);

  tft.drawString(message, 10, 14);

  drawSettingsIcon();

  tft.endWrite();

  lastStatusDrawn = message;
  lastStatusDrawnColor = color;

  Serial.println("[STATUS] " + message);
}
// =====================================================
// 13. AUTOMATIC PRINTER STATUS BAR
// =====================================================

void updateStatusBar() {

  String message;
  uint16_t color = TFT_DARKGREEN;

  if (WiFi.status() != WL_CONNECTED) {

    message = "WIFI DISCONNECTED";
    color = TFT_RED;
  }

  else if (runtimeError.length() > 0) {

    message = runtimeError;
    color = TFT_RED;
  }

  else if (!client.connected()) {

    message = mqttAttempted
              ? "MQTT CONNECTION FAILED"
              : "MQTT CONNECTING";

    color = mqttAttempted ? TFT_RED : TFT_ORANGE;
  }

  else {

    switch (currentUIState) {

      case STATE_IDLE:
        message = "READY FOR PRINT";
        color = TFT_DARKGREEN;
        break;

      case STATE_PRINTING:

        if (printState == "PAUSE") {
          message = "PRINT PAUSED";
          color = TFT_ORANGE;
        }
        else if (printState == "PREPARE") {
          message = "PREPARING PRINT";
          color = TFT_ORANGE;
        }
        else {
          message = "PRINTING IN PROGRESS";
          color = TFT_DARKGREEN;
        }

        break;

      case STATE_COMPLETED:
        message = "PRINT COMPLETED";
        color = TFT_DARKGREEN;
        break;
    }
  }

  showStatusBar(message, color);
}

// =====================================================
// 14. UPDATE SCREEN - OPTIMIZED PARTIAL REFRESH
// =====================================================

// Cached displayed values

int lastProgress = -1;
int lastCurrentLayer = -1;
int lastTotalLayers = -1;
int lastRemainingMins = -1;

String lastNozzleText = "";
String lastBedText = "";
String lastChamberText = "";

String lastPartName = "";
String lastServerLabel = "";

bool screenFirstDraw = true;

void updateScreen() {

  // Detect actual printer UI state change
  bool stateChanged =
    (currentUIState != lastRenderedState);

  if (screenFirstDraw || stateChanged) {

    lastProgress = -1;
    lastCurrentLayer = -1;
    lastTotalLayers = -1;
    lastRemainingMins = -1;

    lastNozzleText = "";
    lastBedText = "";
    lastChamberText = "";

    lastPartName = "";

    lastServerLabel = "";

    screenFirstDraw = false;
  }

  tft.startWrite();

  // =========================================
  // PROGRESS PERCENTAGE
  // =========================================

  if (progress != lastProgress) {

    tft.fillRect(15, 55, 125, 45, TFT_BLACK);

    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(4);
    tft.setTextPadding(0);

    tft.drawString(
      String(progress) + "%",
      15, 60
    );

    lastProgress = progress;
  }

  // =========================================
  // LAYER INFORMATION
  // =========================================

  if (
    currentLayer != lastCurrentLayer ||
    totalLayers != lastTotalLayers
  ) {

    tft.fillRect(140, 55, 120, 45, TFT_BLACK);

    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextPadding(0);

    String layerText =
      "Layer " + String(currentLayer) +
      " / " + String(totalLayers);

    tft.drawString(layerText, 145, 72);

    lastCurrentLayer = currentLayer;
    lastTotalLayers = totalLayers;
  }

  // =========================================
  // PROGRESS BAR
  // =========================================

  static int lastBarProgress = -1;

  if (progress != lastBarProgress || stateChanged) {

    tft.fillRect(17, 117, 241, 22, TFT_BLACK);

    tft.drawRect(15, 115, 245, 26, TFT_WHITE);

    int safeProgress = constrain(progress, 0, 100);

    int barWidth = (241 * safeProgress) / 100;

    if (barWidth > 0) {

      tft.fillRoundRect(
        17, 117,
        barWidth, 22,
        3, TFT_GREEN
      );
    }

    lastBarProgress = progress;
  }

  // =========================================
  // REMAINING TIME
  // =========================================

  if (remainingMins != lastRemainingMins) {

    tft.fillRect(15, 150, 245, 30, TFT_BLACK);

    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextPadding(0);

    String remainingText =
      "Remaining: " +
      String(remainingMins) + " min";

    tft.drawString(remainingText, 15, 155);

    lastRemainingMins = remainingMins;
  }

  // =========================================
  // NOZZLE TEMPERATURE
  // =========================================

  String nozzleText =
    "Nozzle: " + String(nozzleTemp, 1) +
    " / " + String(nozzleTarget, 1) + " C";

  if (nozzleText != lastNozzleText) {

    tft.fillRect(15, 198, 245, 30, TFT_BLACK);

    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextPadding(0);

    tft.drawString(nozzleText, 15, 202);

    lastNozzleText = nozzleText;
  }

  // =========================================
  // BED TEMPERATURE
  // =========================================

  String bedText =
    "Bed: " + String(bedTemp, 1) +
    " / " + String(bedTarget, 1) + " C";

  if (bedText != lastBedText) {

    tft.fillRect(15, 234, 245, 30, TFT_BLACK);

    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextPadding(0);

    tft.drawString(bedText, 15, 238);

    lastBedText = bedText;
  }

  // =========================================
  // CHAMBER TEMPERATURE
  // =========================================

  String chamberText =
    "Chamber: " + String(chamberTemp, 1) + " C";

  if (chamberText != lastChamberText) {

    tft.fillRect(15, 270, 245, 30, TFT_BLACK);

    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextPadding(0);

    tft.drawString(chamberText, 15, 274);

    lastChamberText = chamberText;
  }

  // =========================================
  // ACTIVE PART ILLUSTRATION
  // =========================================

  if (stateChanged) {

    tft.fillRect(280, 98, 180, 175, TFT_BLACK);

    if (currentUIState == STATE_IDLE) {

      drawQuestionMark(370, 150);

      tft.setTextDatum(MC_DATUM);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextSize(2);
      tft.setTextPadding(0);

      tft.drawString(
        "No Active Print",
        370, 230
      );
    }
    else {

      drawPrintingIcon(370, 150);
    }

    lastPartName = "";
  }

  // =========================================
  // PART NAME
  // =========================================

  if (currentUIState != STATE_IDLE) {

    String cleanName = partName;

    cleanName.replace(".gcode.3mf", "");
    cleanName.replace(".3mf", "");

    if (cleanName.length() == 0) {
      cleanName = "Printing Part";
    }

    if (cleanName.length() > 22) {
      cleanName = cleanName.substring(0, 22);
    }

    if (cleanName != lastPartName) {

      tft.fillRect(282, 242, 176, 30, TFT_BLACK);

      tft.setTextDatum(MC_DATUM);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextSize(1);
      tft.setTextPadding(0);

      tft.drawString(cleanName, 370, 255);

      lastPartName = cleanName;
    }
  }

  // =========================================
  // MQTT SERVER ADDRESS
  // =========================================

  String serverLabel = mqtt_server;

  if (serverLabel.length() > 25) {
    serverLabel = serverLabel.substring(0, 25);
  }

  if (serverLabel != lastServerLabel) {

    tft.fillRect(282, 275, 176, 20, TFT_BLACK);

    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.setTextSize(1);
    tft.setTextPadding(0);

    tft.drawString(serverLabel, 370, 285);

    lastServerLabel = serverLabel;
  }

  // =========================================
  // SAVE RENDERED STATE
  // =========================================

  lastRenderedState = currentUIState;

  tft.endWrite();
}

// =====================================================
// 15. DRAW BUTTON
// =====================================================

void drawButton(
  int x, int y, int w, int h,
  String label, uint16_t color) {

  tft.fillRoundRect(x, y, w, h, 6, color);
  tft.drawRoundRect(x, y, w, h, 6, TFT_WHITE);

  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_WHITE, color);
  tft.setTextSize(2);
  tft.setTextPadding(0);

  tft.drawString(
    label,
    x + w / 2,
    y + h / 2
  );
}

// =====================================================
// 16. SETTINGS SCREEN
// =====================================================

void drawSettingsScreen() {

  screenMode = SCREEN_SETTINGS;

  tft.fillScreen(TFT_BLACK);

  // =========================================
  // HEADER
  // =========================================

  tft.fillRect(0, 0, 480, 42, TFT_DARKCYAN);

  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_DARKCYAN);
  tft.setTextSize(2);
  tft.setTextPadding(0);

  tft.drawString(
    "NETWORK & MQTT SETTINGS",
    10, 11
  );

  // =========================================
  // SETTINGS ROW CONFIGURATION
  // =========================================

  const int rowStartY = 46;
  const int rowHeight = 33;

  const int labelX = 10;
  const int fieldX = 165;
  const int fieldW = 303;
  const int fieldH = 28;

  String labels[] = {
    "WIFI SSID",
    "WIFI PASS",
    "MQTT SERVER",
    "MQTT PORT",
    "MQTT USER",
    "MQTT PASS",
    "PRINTER ID"
  };

String wifiPassDisplay = "";
String mqttPassDisplay = "";

// Show saved password as stars.
// If no password is saved, leave the box empty.

if (draftWifiPassword.length() > 0) {
  wifiPassDisplay = "********";
}

if (draftPassword.length() > 0) {
  mqttPassDisplay = "********";
}

String values[] = {
  draftWifiSSID,
  wifiPassDisplay,
  draftServer,
  draftPort,
  draftUser,
  mqttPassDisplay,
  draftSerial
};

  // =========================================
  // DRAW ALL SEVEN SETTINGS
  // =========================================

  for (int i = 0; i < 7; i++) {

    int y = rowStartY + (i * rowHeight);

    // Setting name - left side
    tft.setTextDatum(ML_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextPadding(0);

    tft.drawString(
      labels[i],
      labelX,
      y + 7
    );

    // Input field - right side
    tft.fillRoundRect(
      fieldX,
      y,
      fieldW,
      fieldH,
      5,
      TFT_DARKGREY
    );

    tft.drawRoundRect(
      fieldX,
      y,
      fieldW,
      fieldH,
      5,
      TFT_LIGHTGREY
    );

    // Placeholder / current value
    String displayValue = values[i];

    // Keep long values inside the input field
    if (displayValue.length() > 24) {
      displayValue = displayValue.substring(0, 24);
    }

    tft.setTextDatum(ML_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    tft.setTextSize(2);
    tft.setTextPadding(0);

    tft.drawString(
      displayValue,
      fieldX + 8,
      y + 5
    );
  }

  // =========================================
  // BOTTOM BUTTONS
  // =========================================

  drawButton(
    12, 280, 140, 34,
    "BACK", TFT_BLUE
  );

  drawButton(
    170, 280, 298, 34,
    "UPDATE", TFT_DARKGREEN
  );
}

// =====================================================
// 17. OPEN SETTINGS
// =====================================================

void openSettings() {

  draftWifiSSID = ssid;
  draftWifiPassword = password;

  draftServer = mqtt_server;
  draftPort = String(mqtt_port);
  draftUser = mqtt_user;
  draftPassword = mqtt_pass;
  draftSerial = serial_num;

  activeField = FIELD_NONE;

  drawSettingsScreen();
}
// =====================================================
// 18. OPEN KEYBOARD
// =====================================================

void openKeyboard(SettingField field) {

  activeField = field;

  switch (field) {

    case FIELD_WIFI_SSID:
      editBuffer = draftWifiSSID;
      break;

    case FIELD_WIFI_PASSWORD:
      editBuffer = draftWifiPassword;
      break;

    case FIELD_SERVER:
      editBuffer = draftServer;
      break;

    case FIELD_PORT:
      editBuffer = draftPort;
      break;

    case FIELD_USER:
      editBuffer = draftUser;
      break;

    case FIELD_PASSWORD:
      editBuffer = draftPassword;
      break;

    case FIELD_SERIAL:
      editBuffer = draftSerial;
      break;

    default:
      return;
  }

  keyboardUppercase = false;
  screenMode = SCREEN_KEYBOARD;

  tft.fillScreen(TFT_BLACK);
  tft.fillRect(0, 0, 480, 42, TFT_DARKCYAN);

  String fieldName;

  switch (field) {
    case FIELD_WIFI_SSID: fieldName = "WIFI SSID"; break;
    case FIELD_WIFI_PASSWORD: fieldName = "WIFI PASSWORD"; break;
    case FIELD_SERVER: fieldName = "SERVER"; break;
    case FIELD_PORT: fieldName = "PORT"; break;
    case FIELD_USER: fieldName = "USER"; break;
    case FIELD_PASSWORD: fieldName = "MQTT PASSWORD"; break;
    case FIELD_SERIAL: fieldName = "SERIAL"; break;
    default: fieldName = "EDIT"; break;
  }

  tft.setTextColor(TFT_WHITE, TFT_DARKCYAN);
  tft.setTextSize(2);
  tft.setTextPadding(0);

  tft.drawString("EDIT: " + fieldName, 10, 12);

  drawKeyboard();
}


// =====================================================
// 19. DRAW ON-SCREEN KEYBOARD
// =====================================================

void drawKeyboard() {

  screenMode = SCREEN_KEYBOARD;

  // Clear keyboard area
  tft.fillScreen(TFT_BLACK);

  // =========================================
  // HEADER
  // =========================================

  tft.fillRect(0, 0, 480, 42, TFT_DARKCYAN);

  String fieldName;

  switch (activeField) {
    case FIELD_WIFI_SSID:     fieldName = "WIFI SSID"; break;
    case FIELD_WIFI_PASSWORD: fieldName = "WIFI PASSWORD"; break;
    case FIELD_SERVER:        fieldName = "MQTT SERVER"; break;
    case FIELD_PORT:          fieldName = "MQTT PORT"; break;
    case FIELD_USER:          fieldName = "MQTT USER"; break;
    case FIELD_PASSWORD:      fieldName = "MQTT PASSWORD"; break;
    case FIELD_SERIAL:        fieldName = "PRINTER SERIAL"; break;
    default:                  fieldName = "EDIT"; break;
  }

  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_DARKCYAN);
  tft.setTextSize(2);
  tft.setTextPadding(0);

  tft.drawString("EDIT: " + fieldName, 10, 11);

  // =========================================
  // LARGE INPUT PLACEHOLDER
  // =========================================

  const int inputX = 8;
  const int inputY = 50;
  const int inputW = 464;
  const int inputH = 38;

  tft.fillRoundRect(
    inputX, inputY, inputW, inputH,
    6, TFT_DARKGREY
  );

  tft.drawRoundRect(
    inputX, inputY, inputW, inputH,
    6, TFT_WHITE
  );

  // Display password as stars
  String displayText = editBuffer;

  if (activeField == FIELD_PASSWORD ||
      activeField == FIELD_WIFI_PASSWORD) {

    displayText = "";

    for (int i = 0; i < editBuffer.length(); i++) {
      displayText += "*";
    }
  }

  // Keep text within input box
  const int maxDisplayChars = 28;

  if (displayText.length() > maxDisplayChars) {
    displayText = displayText.substring(
      displayText.length() - maxDisplayChars
    );
  }

  tft.setTextDatum(ML_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft.setTextSize(2);
  tft.setTextPadding(0);

  tft.drawString(
    displayText,
    inputX + 10,
    inputY + 9
  );

  // =========================================
  // KEYBOARD KEYS
  // =========================================

  const char* rows[] = {
    "1234567890",
    "qwertyuiop",
    "asdfghjkl",
    "zxcvbnm.-_"
  };

  for (int r = 0; r < 4; r++) {

    int count = strlen(rows[r]);

    int keyW = 43;
    int gap = 4;

    int totalW =
      count * keyW + (count - 1) * gap;

    int startX = (480 - totalW) / 2;

    for (int c = 0; c < count; c++) {

      String key = String(rows[r][c]);

      if (keyboardUppercase && r > 0) {
        key.toUpperCase();
      }

      drawButton(
        startX + c * (keyW + gap),
        96 + r * 43,
        keyW,
        38,
        key,
        TFT_DARKGREY
      );
    }
  }

  // =========================================
  // BOTTOM CONTROL BUTTONS
  // =========================================

  drawButton(8,   270, 65, 42, "ABC",    TFT_BLUE);
  drawButton(80,  270, 80, 42, "SPACE",  TFT_DARKGREY);
  drawButton(167, 270, 90, 42, "DEL",    TFT_RED);
  drawButton(264, 270, 90, 42, "CANCEL", TFT_ORANGE);
  drawButton(361, 270, 111, 42, "DONE",  TFT_DARKGREEN);
}


// =====================================================
// 20. SAVE KEYBOARD VALUE TO DRAFT
// =====================================================

void saveKeyboardValue() {

  switch (activeField) {

    case FIELD_WIFI_SSID:
      draftWifiSSID = editBuffer;
      break;

    case FIELD_WIFI_PASSWORD:
      draftWifiPassword = editBuffer;
      break;

    case FIELD_SERVER:
      draftServer = editBuffer;
      break;

    case FIELD_PORT:
      {

        long newPort = editBuffer.toInt();

        if (
          newPort <= 0 || newPort > 65535 || editBuffer.length() == 0) {
          showStatusBar("INVALID MQTT PORT", TFT_RED);
          drawKeyboard();
          return;
        }

        draftPort = String(newPort);
        break;
      }

    case FIELD_USER:
      draftUser = editBuffer;
      break;

    case FIELD_PASSWORD:
      draftPassword = editBuffer;
      break;

    case FIELD_SERIAL:
      draftSerial = editBuffer;
      break;

    default:
      break;
  }

  activeField = FIELD_NONE;

  drawSettingsScreen();
}

// =====================================================
// 21. HANDLE KEYBOARD TOUCH
// =====================================================

void handleKeyboardTouch(int x, int y) {

  // Bottom keyboard buttons
  if (y >= 270) {

    if (x < 75) {

      keyboardUppercase = !keyboardUppercase;
      drawKeyboard();
    }

    else if (x < 165) {

      if (editBuffer.length() < 80) {
        editBuffer += " ";
      }

      drawKeyboard();
    }

    else if (x < 260) {

      if (editBuffer.length() > 0) {
        editBuffer.remove(
          editBuffer.length() - 1);
      }

      drawKeyboard();
    }

    else if (x < 360) {

      activeField = FIELD_NONE;
      drawSettingsScreen();
    }

    else {

      saveKeyboardValue();
    }

    return;
  }

  if (y < 96 || y >= 268) {
    return;
  }

  int row = (y - 96) / 43;

  const char* rows[] = {
    "1234567890",
    "qwertyuiop",
    "asdfghjkl",
    "zxcvbnm.-_"
  };

  int count = strlen(rows[row]);

  int keyW = 43;
  int gap = 4;

  int totalW =
    count * keyW + (count - 1) * gap;

  int startX = (480 - totalW) / 2;

  int col = (x - startX) / (keyW + gap);

  if (col < 0 || col >= count) {
    return;
  }

  int keyX = startX + col * (keyW + gap);

  if (x > keyX + keyW) {
    return;
  }

  char ch = rows[row][col];

  if (keyboardUppercase && row > 0) {
    ch = toupper(ch);
  }

  if (editBuffer.length() < 80) {
    editBuffer += ch;
  }

  drawKeyboard();
}


// =====================================================
// 22. UPDATE WIFI AND MQTT SETTINGS
// =====================================================

void applySettings() {

  // =========================================
  // 1. VALIDATE SETTINGS
  // =========================================

  draftWifiSSID.trim();
  draftServer.trim();
  draftPort.trim();
  draftUser.trim();
  draftSerial.trim();

  if (draftWifiSSID.length() == 0) {
    showStatusBar("WIFI SSID REQUIRED", TFT_RED);
    return;
  }

  if (draftServer.length() == 0) {
    showStatusBar("MQTT SERVER REQUIRED", TFT_RED);
    return;
  }

  if (draftSerial.length() == 0) {
    showStatusBar("PRINTER ID REQUIRED", TFT_RED);
    return;
  }

  // Validate port: digits only, range 1–65535
  bool validPort = draftPort.length() > 0;

  for (unsigned int i = 0; i < draftPort.length(); i++) {
    if (!isDigit(draftPort[i])) {
      validPort = false;
      break;
    }
  }

  long newPort = draftPort.toInt();

  if (!validPort || newPort < 1 || newPort > 65535) {
    showStatusBar("INVALID MQTT PORT", TFT_RED);
    return;
  }

  // =========================================
  // 2. COPY DRAFT VALUES TO ACTIVE SETTINGS
  // =========================================

  ssid = draftWifiSSID;
  password = draftWifiPassword;

  mqtt_server = draftServer;
  mqtt_port = (uint16_t)newPort;
  mqtt_user = draftUser;
  mqtt_pass = draftPassword;
  serial_num = draftSerial;

  // =========================================
  // 3. SAVE SETTINGS TO ESP32 FLASH
  // =========================================

  saveMQTTSettings();

  Serial.println();
  Serial.println("===== NEW SETTINGS APPLIED =====");
  Serial.println("WiFi SSID: " + ssid);
  Serial.println("MQTT Server: " + mqtt_server);
  Serial.println("MQTT Port: " + String(mqtt_port));
  Serial.println("MQTT User: " + mqtt_user);
  Serial.println("Printer Serial: " + serial_num);
  Serial.println("================================");

  // =========================================
  // 4. DISCONNECT OLD MQTT CONNECTION
  // =========================================

  if (client.connected()) {
    client.disconnect();
  }

  espClient.stop();

  // =========================================
  // 5. RESET CONNECTION FLAGS
  // =========================================

  mqttAttempted = false;
  wifiEverConnected = false;

  runtimeError = "";

  lastWifiRetry = millis();
  lastMqttAttempt = millis() - MQTT_RETRY_INTERVAL;

  // =========================================
  // 6. RETURN TO HOME SCREEN
  // =========================================

  screenMode = SCREEN_HOME;

  screenFirstDraw = true;
  lastRenderedState = (UIState)-1;

  lastStatusDrawn = "";

  // =========================================
  // 7. RESTART WIFI WITH NEW CREDENTIALS
  // =========================================

  WiFi.disconnect();

  delay(100);

  startWiFiConnection();

  // =========================================
  // 8. REFRESH DISPLAY
  // =========================================

  drawStaticUI();

  updateStatusBar();

  updateScreen();

  screenUpdatePending = false;
  lastScreenUpdate = millis();

  Serial.println("[SETTINGS] Saved and applied successfully.");
}


// =====================================================
// 23. HANDLE SETTINGS TOUCH
// =====================================================

void handleSettingsTouch(int x, int y) {

  // =========================================
  // BOTTOM BUTTONS
  // =========================================

  if (y >= 280 && y <= 319) {

    if (x < 160) {

      // BACK
screenMode = SCREEN_HOME;

screenFirstDraw = true;
lastRenderedState = (UIState)-1;
lastStatusDrawn = "";

drawStaticUI();

updateStatusBar();
updateScreen();
    }

    else if (x >= 160) {

      // UPDATE
      applySettings();
    }

    return;
  }

  // =========================================
  // SETTINGS ROW TOUCH AREAS
  // =========================================

  if (y < 46 || y >= 277) {
    return;
  }

  int row = (y - 46) / 33;

  switch (row) {

    case 0:
      openKeyboard(FIELD_WIFI_SSID);
      break;

    case 1:
      openKeyboard(FIELD_WIFI_PASSWORD);
      break;

    case 2:
      openKeyboard(FIELD_SERVER);
      break;

    case 3:
      openKeyboard(FIELD_PORT);
      break;

    case 4:
      openKeyboard(FIELD_USER);
      break;

    case 5:
      openKeyboard(FIELD_PASSWORD);
      break;

    case 6:
      openKeyboard(FIELD_SERIAL);
      break;
  }
}

// =====================================================
// 24. TOUCHSCREEN INPUT
// =====================================================

void handleTouchInput() {

  if (!tft.getTouch(&touchX, &touchY)) {
    return;
  }

  if (
    millis() - lastTouchTime < TOUCH_DEBOUNCE) {
    return;
  }

  lastTouchTime = millis();

  Serial.print("Touch X: ");
  Serial.print(touchX);
  Serial.print(" Y: ");
  Serial.println(touchY);

  if (screenMode == SCREEN_HOME) {

    // Settings icon
    if (
      touchX >= 425 && touchY <= 45) {
      openSettings();
    }
  }

  else if (screenMode == SCREEN_SETTINGS) {

    handleSettingsTouch(
      touchX,
      touchY);
  }

  else if (screenMode == SCREEN_KEYBOARD) {

    handleKeyboardTouch(
      touchX,
      touchY);
  }
}

// =====================================================
// 25. JSON FLOAT EXTRACTION
// =====================================================

float extractJsonFloat(
  const String& payloadStr,
  const String& key) {

  int idx = payloadStr.indexOf(key);

  if (idx == -1) {
    return -999.0;
  }

  int colonIdx = payloadStr.indexOf(
    ':',
    idx + key.length());

  if (colonIdx == -1) {
    return -999.0;
  }

  int start = colonIdx + 1;

  while (
    start < payloadStr.length() && (payloadStr[start] == ' ' || payloadStr[start] == '"')) {
    start++;
  }

  int end = start;

  while (
    end < payloadStr.length() && (isDigit(payloadStr[end]) || payloadStr[end] == '.' || payloadStr[end] == '-')) {
    end++;
  }

  if (end > start) {
    return payloadStr.substring(
                       start, end)
      .toFloat();
  }

  return -999.0;
}
// =====================================================
// RESET ALL PRINT PARAMETERS TO INITIAL STATE
// =====================================================

void resetPrintParameters() {

  progress = 0;

  currentLayer = 0;
  totalLayers = 0;

  remainingMins = 0;

  partName = "No Active Print";

  Serial.println("[SYSTEM] Print parameters reset to READY state.");
}
// =====================================================
// 26. MQTT CALLBACK
// =====================================================

void mqttCallback(
  char* topic,
  byte* payload,
  unsigned int length) {

  // Do not redraw the monitoring UI while
  // the settings or keyboard screen is open.
  String rawMsg;

  rawMsg.reserve(length + 1);

  for (unsigned int i = 0; i < length; i++) {
    rawMsg += (char)payload[i];
  }

  bool updated = false;

  // Chamber temperature
  float cTemp = extractJsonFloat(
    rawMsg,
    "\"chamber_temper\"");

  if (cTemp > -100.0) {
    chamberTemp = cTemp;
    updated = true;
  }

  // Fallback chamber temperature
  int ctcIdx = rawMsg.indexOf("\"ctc\"");

  if (ctcIdx != -1) {

    String subCtc = rawMsg.substring(
      ctcIdx,
      min(
        (int)rawMsg.length(),
        ctcIdx + 120));

    float ctcTemp = extractJsonFloat(
      subCtc,
      "\"temp\"");

    if (ctcTemp > -100.0) {
      chamberTemp = ctcTemp;
      updated = true;
    }
  }

  // Parse MQTT JSON
  JsonDocument doc;

  DeserializationError error =
    deserializeJson(doc, rawMsg);

  if (error) {

    runtimeError = "ERROR: INVALID MQTT JSON";

    if (screenMode == SCREEN_HOME) {
      updateStatusBar();
    }

    Serial.print("JSON ERROR: ");
    Serial.println(error.c_str());

    return;
  }

  runtimeError = "";

  JsonObject printObj = doc["print"];

  if (!printObj.isNull()) {

    // G-code State
    if (printObj["gcode_state"].is<String>()) {

      prevPrintState = printState;

      printState =
        printObj["gcode_state"].as<String>();

      updated = true;

      if (
        printState == "RUNNING" || printState == "PAUSE" || printState == "PREPARE") {

        currentUIState = STATE_PRINTING;
      }

      else if (
        printState == "FINISH" && prevPrintState != "FINISH" && prevPrintState != "IDLE") {

        currentUIState = STATE_COMPLETED;

        completedStartTime = millis();
      }

      else if (
  printState == "IDLE" || printState == "FAILED") {

  currentUIState = STATE_IDLE;

  // Reset print information in READY state
  progress = 0;

  currentLayer = 0;
  totalLayers = 0;

  remainingMins = 0;

  partName = "No Active Print";
}
    }

    // Part Name
    if (printObj.containsKey("subtask_name")) {

      partName =
        printObj["subtask_name"].as<String>();

      updated = true;
    }

    // Progress
    if (printObj.containsKey("mc_percent")) {

      progress =
        printObj["mc_percent"].as<int>();

      updated = true;
    }

    // Nozzle Temperature
    if (printObj.containsKey("nozzle_temper")) {

      nozzleTemp =
        printObj["nozzle_temper"].as<float>();

      updated = true;
    }

    // Nozzle Target Temperature
    if (printObj.containsKey("nozzle_target_temper")) {

      nozzleTarget =
        printObj["nozzle_target_temper"].as<float>();

      updated = true;
    }

    // Bed Temperature
    if (printObj.containsKey("bed_temper")) {

      bedTemp =
        printObj["bed_temper"].as<float>();

      updated = true;
    }

    // Bed Target Temperature
    if (printObj.containsKey("bed_target_temper")) {

      bedTarget =
        printObj["bed_target_temper"].as<float>();

      updated = true;
    }

    // Current Layer
    if (printObj.containsKey("layer_num")) {

      currentLayer =
        printObj["layer_num"].as<int>();

      updated = true;
    }

    // Total Layers
    if (printObj.containsKey("total_layer_num")) {

      totalLayers =
        printObj["total_layer_num"].as<int>();

      updated = true;
    }

    // Remaining Time
    if (printObj.containsKey("mc_remaining_time")) {

      remainingMins =
        printObj["mc_remaining_time"].as<int>();

      updated = true;
    }
  }

// =========================================
// RESET PARAMETERS WHEN PRINTER IS READY
// =========================================

if (currentUIState == STATE_IDLE) {

  resetPrintParameters();
}

// =========================================
// UPDATE STATUS AND SCHEDULE SCREEN REFRESH
// =========================================

if (screenMode == SCREEN_HOME) {

  updateStatusBar();

  if (updated) {
    screenUpdatePending = true;
  }
}
}

// =====================================================
// 27. MQTT RECONNECTION
// =====================================================

void reconnectMQTT() {

  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  if (
    millis() - lastMqttAttempt < MQTT_RETRY_INTERVAL) {
    return;
  }

  lastMqttAttempt = millis();
  mqttAttempted = true;

  client.setServer(
    mqtt_server.c_str(),
    mqtt_port);

  String reportTopic =
    "device/" + serial_num + "/report";

  String requestTopic =
    "device/" + serial_num + "/request";

  String clientId =
    "WT32-Bambu-" + String(random(0xffff), HEX);

  Serial.println(
    "Connecting to Bambu Lab MQTT...");

  if (
    client.connect(
      clientId.c_str(),
      mqtt_user.c_str(),
      mqtt_pass.c_str())) {

    Serial.println("MQTT Connected!");

    client.subscribe(
      reportTopic.c_str());

    String pushAllCmd =
      "{\"pushing\":{\"sequence_id\":\"0\",\"command\":\"pushall\",\"version\":1,\"push_target\":1}}";

    client.publish(
      requestTopic.c_str(),
      pushAllCmd.c_str());

    Serial.println(
      "Pushall command sent!");

  } else {

    Serial.print("MQTT Failed: ");
    Serial.println(client.state());
  }

  if (screenMode == SCREEN_HOME) {
    updateStatusBar();
  }
}
// =====================================================
// FAST NON-BLOCKING WIFI CONNECTION
// =====================================================

void startWiFiConnection() {

  Serial.println("[WiFi] Starting connection...");

  if (ssid.length() == 0) {
    Serial.println("[WiFi] SSID is empty!");
    return;
  }

  // Configure Wi-Fi
  WiFi.mode(WIFI_STA);

  // Disable power saving for faster responsiveness
  WiFi.setSleep(false);

  // Automatically reconnect if disconnected
  WiFi.setAutoReconnect(true);

  // Avoid unnecessary flash writes
  WiFi.persistent(false);

  // Start connection without waiting
  WiFi.begin(
    ssid.c_str(),
    password.c_str()
  );

  lastWifiRetry = millis();

  Serial.println("[WiFi] Connection started in background.");
}
// =====================================================
// 28. SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  // Load saved Wi-Fi and MQTT settings
  loadMQTTSettings();

  // Initialize display
  tft.init();
  tft.setRotation(1);

  drawStaticUI();

  showStatusBar("SYSTEM STARTING", TFT_DARKGREY);

  // =========================================
  // MQTT CONFIGURATION
  // =========================================

  espClient.setInsecure();

  client.setServer(
    mqtt_server.c_str(),
    mqtt_port
  );

  client.setCallback(mqttCallback);

  // Reduce time spent waiting for an MQTT response
  client.setSocketTimeout(3);
  client.setKeepAlive(15);

  // PubSubClient standard API: one argument
if (!client.setBufferSize(32768,32768)) {
    Serial.println("[MQTT] Buffer allocation failed!");
  } else {
    Serial.println("[MQTT] Buffer configured.");
  }

  // =========================================
  // START WIFI WITHOUT BLOCKING
  // =========================================

  startWiFiConnection();

  // Allow immediate MQTT attempt after Wi-Fi connects
  lastMqttAttempt = millis() - MQTT_RETRY_INTERVAL;

  showStatusBar("WIFI CONNECTING...", TFT_ORANGE);

  updateScreen();

  Serial.println("[SYSTEM] Startup complete.");
}
// =====================================================
// 29. MAIN LOOP - OPTIMIZED
// =====================================================

void loop() {

  // =========================================
  // TOUCHSCREEN INPUT
  // =========================================

  handleTouchInput();

  // =========================================
  // PROCESS MQTT EVEN ON SETTINGS SCREEN
  // =========================================

  if (client.connected()) {
    client.loop();
  }

  // Do not redraw home screen while editing
  if (screenMode != SCREEN_HOME) {

    delay(5);
    return;
  }

  // =========================================
  // WIFI CONNECTION CHECK
  // =========================================

  if (WiFi.status() != WL_CONNECTED) {

    updateStatusBar();

    if (
      millis() - lastWifiRetry >= WIFI_RETRY_INTERVAL
    ) {

      lastWifiRetry = millis();

      Serial.println("Retrying WiFi...");

      WiFi.reconnect();
    }

    delay(10);
    return;
  }

  if (!wifiEverConnected) {

    wifiEverConnected = true;

    updateStatusBar();
  }

  // =========================================
  // MQTT RECONNECTION
  // =========================================

  if (!client.connected()) {

    reconnectMQTT();
  }

  // =========================================
  // CONTROLLED DISPLAY REFRESH
  // =========================================

  if (
    screenUpdatePending &&
    millis() - lastScreenUpdate >= SCREEN_REFRESH_INTERVAL
  ) {

    screenUpdatePending = false;

    lastScreenUpdate = millis();

    updateScreen();
  }

  // =========================================
  // PRINT COMPLETION RESET
  // =========================================

  if (
    currentUIState == STATE_COMPLETED &&
    millis() - completedStartTime >= COMPLETED_DISPLAY_MS
  ) {

    currentUIState = STATE_IDLE;

    resetPrintParameters();

    screenUpdatePending = true;

    lastRenderedState = (UIState)-1;

    updateStatusBar();
  }

  delay(5);
}