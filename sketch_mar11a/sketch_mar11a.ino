#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS A0 // Data pin for DS18B20

OneWire oneWire(ONE_WIRE_BUS); // Create OneWire object
DallasTemperature sensors(&oneWire); // Create DallasTemperature object

#define WIFI_SSID "Google"
#define WIFI_PASSWORD "muaz.love"
#define BOT_TOKEN "5343217942:AAH4lnbfmnvSIxuhVr45B4lNZJESuM792T0"
#define CHAT_ID "766993019"
#define GasPin 5 ;
#define uv_sensor 12 ;


X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

void setup() {
  Serial.begin(115200);
  Serial.println();

  // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600) {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);

  bot.sendMessage(CHAT_ID, "Bot started up", "");
 pinMode(GasPin, INPUT); // Set the flame sensor pin as input  
 pinMode(uv_sensor, INPUT); // Set the flame sensor pin as input  
 sensors.begin(); // Start communication with DS18B20 sensor temp

}

void chtemp() {
sensors.requestTemperatures(); // Send command to read temperature from DS18B20 sensor
  float temperature = sensors.getTempCByIndex(0); // Read temperature data from DS18B20 sensor

  char tempString[10]; // allocate a buffer to hold the temperature string
  dtostrf(temperature, 4, 2, tempString); // convert the temperature value to a string with 2 decimal places
  Serial.print("Temperature: ");
  Serial.print(tempString);
  Serial.println("C");
  bot.sendMessage(CHAT_ID, tempString, ""); // send the temperature value as a message to the Telegram bot
}

void chgas() {
  int GasValue = digitalRead(GasPin); // Read the flame sensor value

  if (GasValue == LOW) { // If flame is detected
    Serial.println("GAS detected!"); // Print message to serial monitor
    bot.sendMessage(CHAT_ID, "Gas detected", "");
  } else { // If no flame is detected
    Serial.println(" NO Gas Detected."); // Print message to serial monitor
  }
}

void chuv() {
  if (digitalRead(uv_sensor)) {
    bot.sendMessage(CHAT_ID, "Flame detected", "");
  }
}

void loop() {
  chtemp(); // read temperature data and send it to the Telegram bot
  chgas(); // check gas sensor and send message if detected
  chuv(); // check UV sensor and send message if detected
  delay(1000);
}
