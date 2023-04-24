
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define WIFI_SSID "T"
#define WIFI_PASSWORD "12345678qw"
#define BOT_TOKEN "5343217942:AAH4lnbfmnvSIxuhVr45B4lNZJESuM792T0"
#define CHAT_ID "766993019"
#define temp A0;
int gas_sensor =5;// D1;
int uv_sensor =12; //D6;

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
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);

  bot.sendMessage(CHAT_ID, "Bot started up", "");
  pinMode(gas_sensor, INPUT);
  pinMode(uv_sensor, INPUT);
}
//void chtemp(){
//  int analogValue = analogRead(temp);
//  float millivolts = (analogValue/1024.0) * 3300; //3300 is the voltage provided by NodeMCU
//  float celsius = millivolts/10;
//  char [] tmp= celsius;
//  Serial.print("in DegreeC=   ");
//  Serial.println(celsius);
//  bot.sendMessage(CHAT_ID, tmp, "");
//}
void chgas(){
  if(digitalRead(gas_sensor)){
  bot.sendMessage(CHAT_ID, "gas detected", "");
    }
  }
void chuv(){
  if(digitalRead(uv_sensor)){
  bot.sendMessage(CHAT_ID, "flame detected", "");
    }
  }
void loop() {
//  chtemp();/
  chgas();
  chuv();
  delay(1000);
}
