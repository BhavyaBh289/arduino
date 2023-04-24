 #include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>
int ONE_WIRE_BUS =10;   // Data wire is connected to GPIO2
int GAS_PIN = 5;
int UV_SENSOR_PIN = 17;
#define TEMP_SENSOR_PIN A0

OneWire oneWire(ONE_WIRE_BUS); // Create OneWire object
DallasTemperature sensors(&oneWire); // Create DallasTemperature object

#define WIFI_SSID "T"
#define WIFI_PASSWORD "12345678qw"
#define BOT_TOKEN "5836454139:AAF4lkVrgMm8Q2Mu1gCFrtCTzgZ2PTg8B2g"
#define CHAT_ID "766993019"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

void setup() {
  Serial.begin(9600);
  Serial.println();

  // Connect to Wi-Fi network
  Serial.print("Connecting to Wi-Fi network ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 10) {
    Serial.print(".");
    delay(500);
    retries++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("\nWi-Fi connected. IP address: ");
    Serial.println(WiFi.localIP());

    // Retrieve time via NTP
    Serial.print("Retrieving time: ");
    configTime(0, 0, "pool.ntp.org");
    time_t now = time(nullptr);
    while (now < 24 * 3600) {
      Serial.print(".");
      delay(100);
      now = time(nullptr);
    }
    Serial.println(now);

    // Initialize sensors
    pinMode(GAS_PIN, INPUT);
    sensors.begin();
  } else {
    Serial.println("\nFailed to connect to Wi-Fi network");
  }

  // Send startup message to Telegram bot
  bot.sendMessage(CHAT_ID, "Bot started up", "");
}

void sendTemperatureToBot() {
  float temperature = getTemperature();
  char tempString[10];
  dtostrf(temperature, 4, 2, tempString);
  Serial.print("Temperature: ");
  Serial.print(tempString);
  Serial.println("C");
  if (temperature >=30){
    bot.sendMessage(CHAT_ID, "high temperature", "");
    bot.sendMessage(CHAT_ID, tempString, "");
  }else{
    if(temperature !=-1000){
      bot.sendMessage(CHAT_ID, tempString, "");  
      }
  }
}
float getTemperature() {
  byte data[12];
  byte addr[8];
  if (!oneWire.search(addr)) {
    oneWire.reset_search();
    return -1000;
  }
  if (OneWire::crc8(addr, 7) != addr[7]) {
    return -1000;
  }
  if (addr[0] != 0x28) {
    return -1000;
  }
  oneWire.reset();
  oneWire.select(addr);
  oneWire.write(0x44, 1);
  delay(1000);
  oneWire.reset();
  oneWire.select(addr);
  oneWire.write(0xBE);
  for (int i = 0; i < 9; i++) {
    data[i] = oneWire.read();
  }
  int16_t raw = (data[1] << 8) | data[0];
  byte cfg = (data[4] & 0x60);
  if (cfg == 0x00) raw = raw & ~7;
  else if (cfg == 0x20) raw = raw & ~3;
  else if (cfg == 0x40) raw = raw & ~1;
  float celsius = (float)raw / 16.0;
  return celsius;
}
void checkGasSensor() {
  int gasValue = digitalRead(GAS_PIN);

  if (gasValue == LOW) {
    Serial.println("Gas detected!");
    bot.sendMessage(CHAT_ID, "Gas detected", "");
  } else {
    Serial.println("No gas detected.");
  }
}

void checkUvSensor() {
  if (digitalRead(UV_SENSOR_PIN)) {
    bot.sendMessage(CHAT_ID, "Flame detected", "");
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Lost Wi-Fi connection, reconnecting...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 10) {
      Serial.print(".");
      delay(500);
      retries++;
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("\nWi-Fi connected. IP address: ");
      Serial.println(WiFi .status());
// Reinitialize sensors after Wi-Fi reconnection
pinMode(GAS_PIN, INPUT);
sensors.begin();
} else {
Serial.println("\nFailed to reconnect to Wi-Fi network");
return;
}
}

sendTemperatureToBot();
checkGasSensor();
checkUvSensor();

delay(5000); // Wait 5 seconds before looping again
}
