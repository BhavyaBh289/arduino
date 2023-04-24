#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
WiFiClient client;
long myChannelNumber = 2025395;
const char myWriteAPIKey[] = "5OZ28SEW1L7BMPDH";
const int sensor=A0; // Assigning analog pin A0 to variable 'sensor'
float tempc;  //variable to store temperature in degree Celsius
float tempf;  //variable to store temperature in Fahreinheit 
float vout;  //temporary variable to hold sensor reading

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
WiFi.begin("T","12345678qw");
while(WiFi.status() !=WL_CONNECTED)
{delay(100);
Serial.print(".");
}
Serial.println();
Serial.println("NodeMCU is Connected");
Serial.println(WiFi.localIP());
ThingSpeak.begin(client);
pinMode(sensor,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
vout=analogRead(sensor);
vout=(vout*5.00)/1024-18;
tempc=vout; // Storing value in Degree Celsius
tempf=(vout*1.8)+32; // Converting to Fahrenheit 
  Serial.println("Temperature in C: " + (String) tempc);
  Serial.println("Temperature in F: " + (String) tempf);
  ThingSpeak.writeField(myChannelNumber, 1, tempc, myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, 2, tempf, myWriteAPIKey);
  delay(2000);
}
