#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define WIFI_SSID "T"
#define WIFI_PASSWORD "12345678qw"
#define BOT_TOKEN "5343217942:AAErwRs9eK8RDPPITNT8tb5M_wME_GgM7rM"
#define CHAT_ID "766993019"
unsigned long Channel_ID = 12345; // Your Channel ID
const char * myWriteAPIKey = "ABCDEF12345"; //Your write API key

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

int  GAS_PIN =5 ;//D1
#define LM35_PIN A0
int UV_SENSOR_PIN =12; // D6

int led = 0 ;//D3
int RELAY_PIN = 4 ;//D2
int Buzzer=14 ;// D5
bool tempp = false;
bool gass = false;
bool flamee = false;

void setup() {
  Serial.begin(9600);
  Serial.println();

 Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
 
  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  ThingSpeak.begin(client);
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);

  bot.sendMessage(CHAT_ID, "Bot started up", "");
  Serial.println("bot");
  // Initialize sensors
  pinMode(GAS_PIN, INPUT);
  pinMode(UV_SENSOR_PIN, INPUT);
  pinMode(LM35_PIN, INPUT);
  
  // Initialize output
  pinMode(led, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(led, HIGH);      
  noTone(Buzzer);

}

float getTemperature() {
  int raw = analogRead(LM35_PIN);
  float voltage = (raw / 1023.0) * 3.3;  // 3.3V is the operating voltage of the ESP8266
  float temperature = voltage * 100.0 + 25;
  ThingSpeak.writeField(Channel_ID, temperature, temperature, myWriteAPIKey);

  return temperature;
}

void sendTemperatureToBot() {
  float temperature = getTemperature();
  char tempString[10];
  dtostrf(temperature, 4, 2, tempString);
  Serial.print("Temperature: ");
  Serial.print(tempString);
  Serial.println("C");
  if (temperature >= 30) {
//    bot.sendMessage(CHAT_ID, "Bot started upp", "");
    bot.sendMessage(CHAT_ID, "High temperature", "");
    tempp = true;    
//    bot.sendMessage(CHAT_ID, tempString, "");
  } else {
    tempp = false;
    bot.sendMessage(CHAT_ID, tempString, "");
    bot.sendMessage(CHAT_ID, "normal temperature", "");
  }
}

void checkoutputs(){
  if(flamee==true && tempp == true){ //case 8 and 6 led and buzzer on 
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(led, HIGH);
    tone(Buzzer, 1000);
    Serial.println("68");

  }else{ 
    if(flamee==true && tempp == false){  //case 7 and 5 buzzer on 
      digitalWrite(RELAY_PIN, HIGH);
      digitalWrite(led, LOW);
      tone(Buzzer, 1000);
      Serial.println("75");

    }else{
      if(flamee==false && gass==true && tempp == true){  //case 4 all on
        digitalWrite(RELAY_PIN, LOW);
        digitalWrite(led, HIGH);
        tone(Buzzer, 1000);
        Serial.println("4");

      }else{
        if(flamee==false && gass==true && tempp == false){  //case 3 fan buzzer on
          digitalWrite(RELAY_PIN, LOW);
          digitalWrite(led, LOW);
          tone(Buzzer, 1000);
          Serial.println("3");

        }else{
          if(flamee==false && gass==false && tempp == true){  //case 2 fan led on
            digitalWrite(RELAY_PIN, LOW);
            digitalWrite(led, HIGH);
            noTone(Buzzer);
            Serial.println("2");
          }else{
            if(flamee==false && gass==false && tempp == false){  //case 1 all off
              digitalWrite(RELAY_PIN, HIGH);
              digitalWrite(led, LOW);
              noTone(Buzzer);
              Serial.println("1");
            }
          }
        }
      }
    }
  }
}
void checkGasSensor(){
  if (!digitalRead(GAS_PIN)) {
    Serial.println("Gas detected!");
    bot.sendMessage(CHAT_ID, "gas detected", "");
    ThingSpeak.writeField(Channel_ID, gas, 1, myWriteAPIKey);
    gass = true;
  } else {
    Serial.println(" No Gas detected");
    ThingSpeak.writeField(Channel_ID, gas, 0, myWriteAPIKey);
    gass = false;
  }
}

void checkUvSensor(){
  if (!digitalRead(UV_SENSOR_PIN)) {
     Serial.println("flame detected!");
    bot.sendMessage(CHAT_ID, "flame detected", "");
    ThingSpeak.writeField(Channel_ID, flame, 1, myWriteAPIKey);

    flamee = true;
  }else{
     Serial.println("no flame detected!");
         ThingSpeak.writeField(Channel_ID, flame, 0, myWriteAPIKey);

    flamee = false;
  }
}

void loop(){
//  if (WiFi.status() != WL_CONNECTED) {
//     Serial.print("Connecting to Wifi SSID ");
//  Serial.print(WIFI_SSID);
//  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
//  while (WiFi.status() != WL_CONNECTED)
//  {
//    Serial.print(".");
//    delay(500);
//  }
//  Serial.print("\nWiFi connected. IP address: ");
//  Serial.println(WiFi.localIP());
//
//  Serial.print("Retrieving time: ");
//  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
//  time_t now = time(nullptr);
//  while (now < 24 * 3600)
//  {
//    Serial.print(".");
//    delay(100);
//    now = time(nullptr);
//  }
//  Serial.println(now);
//
//  bot.sendMessage(CHAT_ID, "Bot started up", "");
//
//}
sendTemperatureToBot();
checkGasSensor();
checkUvSensor();
checkoutputs();
//bot.sendMessage(CHAT_ID, "loop  ", "");
delay(1000); // Wait 5 seconds before looping again
//bot.sendMessage(CHAT_ID, "loop", "");
}
