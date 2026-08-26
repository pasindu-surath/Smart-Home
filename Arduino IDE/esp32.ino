#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Token generation process info
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// =====================================================
// WIFI & FIREBASE CONFIG
// =====================================================

#define WIFI_SSID "Pixel_1186"      // Oyage WiFi nama methanata danna
#define WIFI_PASSWORD "srw222777"  // Oyage WiFi password eka methanata danna

#define API_KEY "AIzaSyBknnhznrjjmUyTKxjylUvyJGAQFudcibk"
#define DATABASE_URL "https://smart-home-iot-9226f-default-rtdb.firebaseio.com"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
const long sendDataInterval = 2000; // Thappara 2katasarayak data yawai

// =====================================================
// PIN DEFINITIONS
// =====================================================

#define DHT_PIN        4
#define DHT_TYPE       DHT11

#define LDR_PIN        34
#define GAS_PIN        35

#define LASER_BUZZER   25
#define GAS_BUZZER     26

#define OLED_SDA       21
#define OLED_SCL       22

// =====================================================
// OLED
// =====================================================

#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// =====================================================
// DHT
// =====================================================

DHT dht(DHT_PIN, DHT_TYPE);

// =====================================================
// FIXED THRESHOLDS
// =====================================================

// ----- LASER -----
const int LASER_ON_THRESHOLD = 100;
const int LASER_OFF_THRESHOLD = 50;

// ----- GAS / SMOKE -----
const int GAS_ON_THRESHOLD = 3900;
const int GAS_OFF_THRESHOLD = 3900;

// =====================================================
// VARIABLES
// =====================================================

int ldrValue = 0;
int gasValue = 0;

float temperature = 0;
float humidity = 0;

bool laserAlarm = false;
bool gasAlarm = false;

unsigned long lastDHTRead = 0;
unsigned long systemStartTime = 0;
const unsigned long GAS_WARMUP_TIME = 30000; // 30 sec

// =====================================================
// AVERAGE LDR
// =====================================================

int readLDR()
{
  long total = 0;
  for (int i = 0; i < 10; i++)
  {
    total += analogRead(LDR_PIN);
    delay(2);
  }
  return total / 10;
}

// =====================================================
// AVERAGE GAS SENSOR
// =====================================================

int readGas()
{
  long total = 0;
  for (int i = 0; i < 10; i++)
  {
    total += analogRead(GAS_PIN);
    delay(2);
  }
  return total / 10;
}

// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);
  analogReadResolution(12);

  pinMode(LASER_BUZZER, OUTPUT);
  pinMode(GAS_BUZZER, OUTPUT);
  digitalWrite(LASER_BUZZER, LOW);
  digitalWrite(GAS_BUZZER, LOW);

  dht.begin();
  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED ERROR!");
    while (true) delay(1000);
  }

  // --- CONNECT TO WIFI ---
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Connecting to WiFi:");
  display.println(WIFI_SSID);
  display.display();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // --- CONNECT TO FIREBASE ---
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Anonymous Sign in (Public read/write rules aniwaryen thiyenna one)
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase Ready!");
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  // Show normal startup screen
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("SMART HOME SYSTEM");
  display.println();
  display.println("WiFi & Firebase OK!");
  display.println("Laser Security");
  display.println("Gas / Smoke");
  display.display();
  delay(2000);

  systemStartTime = millis();
}

// =====================================================
// LASER SECURITY
// =====================================================

void checkLaser()
{
  ldrValue = readLDR();
  if (!laserAlarm)
  {
    if (ldrValue > LASER_ON_THRESHOLD)
    {
      laserAlarm = true;
      Serial.println("!!! LASER SECURITY ALERT !!!");
    }
  }
  else
  {
    if (ldrValue < LASER_OFF_THRESHOLD)
    {
      laserAlarm = false;
      Serial.println("Laser alarm OFF.");
    }
  }
}

// =====================================================
// GAS / SMOKE SECURITY
// =====================================================

void checkGas()
{
  gasValue = readGas();
  if (millis() - systemStartTime < GAS_WARMUP_TIME)
  {
    gasAlarm = false;
    return;
  }

  if (!gasAlarm)
  {
    if (gasValue > GAS_ON_THRESHOLD)
    {
      gasAlarm = true;
      Serial.println("!!! GAS / SMOKE ALERT !!!");
    }
  }
  else
  {
    if (gasValue < GAS_OFF_THRESHOLD)
    {
      gasAlarm = false;
      Serial.println("Gas alarm OFF.");
    }
  }
}

// =====================================================
// BUZZERS
// =====================================================

void updateBuzzers()
{
  if (laserAlarm)
  {
    if ((millis() / 150) % 2 == 0) digitalWrite(LASER_BUZZER, HIGH);
    else digitalWrite(LASER_BUZZER, LOW);
  }
  else digitalWrite(LASER_BUZZER, LOW);

  if (gasAlarm)
  {
    if ((millis() / 400) % 2 == 0) digitalWrite(GAS_BUZZER, HIGH);
    else digitalWrite(GAS_BUZZER, LOW);
  }
  else digitalWrite(GAS_BUZZER, LOW);
}

// =====================================================
// DHT11
// =====================================================

void readDHT()
{
  if (millis() - lastDHTRead >= 2000)
  {
    lastDHTRead = millis();
    float newHumidity = dht.readHumidity();
    float newTemperature = dht.readTemperature();

    if (!isnan(newHumidity) && !isnan(newTemperature))
    {
      humidity = newHumidity;
      temperature = newTemperature;
    }
  }
}

// =====================================================
// SEND TO FIREBASE
// =====================================================

void sendToFirebase()
{
  // Firebase.ready() check karanne connection eka thiyenawada kiyala
  if (Firebase.ready() && (millis() - sendDataPrevMillis > sendDataInterval || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    // Firebase ekata yawanna JSON object ekak hadanawa
    FirebaseJson json;
    json.set("temperature", temperature);
    json.set("humidity", humidity);
    json.set("gas", gasValue);
    json.set("securityAlert", laserAlarm);
    json.set("gasAlert", gasAlarm);

    // "smartHome" path ekata data tika push karanawa
    if (Firebase.RTDB.updateNode(&fbdo, "/smartHome", &json)) {
      Serial.println("Firebase Data Uploaded Successfully!");
    } else {
      Serial.println("Firebase Error: " + fbdo.errorReason());
    }
  }
}

// =====================================================
// OLED DISPLAY
// =====================================================

void updateDisplay()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print("TEMP: ");
  display.print(temperature, 1);
  display.println(" C");

  display.print("HUM : ");
  display.print(humidity, 1);
  display.println(" %");

  display.print("LDR : ");
  display.println(ldrValue);

  display.print("GAS : ");
  display.println(gasValue);

  display.println();
  if (laserAlarm && gasAlarm) display.println("!!! BOTH ALARMS !!!");
  else if (laserAlarm) display.println("!! LASER ALERT !!");
  else if (gasAlarm) display.println("!! GAS ALERT !!");
  else display.println("SYSTEM NORMAL");

  display.display();
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop()
{
  checkLaser();
  checkGas();
  readDHT();
  
  updateBuzzers();
  updateDisplay();
  
  // Data tika Firebase ekata yawanawa (Me function eka athule 2 sec delay ekak thiyenawa)
  sendToFirebase();

  delay(50); // Loop eka too fast wena eka nawaththanna podi delay ekak
}
