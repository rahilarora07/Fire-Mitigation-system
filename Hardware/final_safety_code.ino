#include "DHT.h"
#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>// including the library of DHT11 temperature and humidity sensor
#include <ESP8266WiFi.h>

WiFiClient client;

String MakerIFTTT_Key ;
;String MakerIFTTT_Event;
char *append_str(char *here, String s) {  int i=0; while (*here++ = s[i]){i++;};return here-1;}
char *append_ul(char *here, unsigned long u) { char buf[20]; return append_str(here, ultoa(u, buf, 10));}
char post_rqst[256];char *p;char *content_length_here;char *json_start;int compi;

#define DHTTYPE DHT11  

#define FIREBASE_HOST "smart-shelf-2da91.firebaseio.com"
#define FIREBASE_AUTH "NYwwuTifOlIEJvu5v8VqrE6FvKq6Z6hQF2TN6S4Q"
#define WIFINAME "OnePlus5T" //Your SSID
#define WIFIPASS "" // Your Wifi Pass// DHT 11

#define dhtp 0

int buzzer = 04;
int smokeA0 = A0;
int sensorThres = 600;

DHT dht(dhtp, DHTTYPE); 
int flag=0;
void setup(void)
{ 
  dht.begin();
  pinMode(buzzer, OUTPUT);
  pinMode(smokeA0, INPUT);
  Serial.begin(9600);
  Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);
  Serial.println("Humidity and temperature\n\n");
  delay(200);

  WiFi.disconnect();
  delay(3000);
  Serial.println("START");
   WiFi.begin("OnePlus5T","");
  while ((!(WiFi.status() == WL_CONNECTED))){
    delay(300);
    Serial.print("..");
  }
}
void loop() {
    int analogSensor = analogRead(smokeA0);
    String smoke = String(analogSensor);
    Serial.println(analogSensor);
    Firebase.setString("Smoke Level ",smoke);
    float h = dht.readHumidity();
    String fireHumid = String(h) + String("%"); 
    String a="ON";
    String b="OFF";
    float t = dht.readTemperature();    
    String fireTemp = String(t);     
    Serial.print("Current humidity = ");
    Serial.print(h);
    Firebase.setString("Current humidity = ",fireHumid);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(t);
    Firebase.setString("temperature = ",fireTemp);
    Serial.println("C  ");
    if (analogSensor > sensorThres || t >30)
  {
    Serial.println("buzzer");
    Firebase.setString("Buzzer ",a);
    tone(buzzer, 1000, 200);
    if(flag==0)
    {
      Serial.println("SOS");
    if (client.connect("maker.ifttt.com",80)) {
    MakerIFTTT_Key ="eexP79wX843kVoip_Gz6waLznHBzDuB1RxXJa-mHzff";
    MakerIFTTT_Event ="firesafety";
    p = post_rqst;
    p = append_str(p, "POST /trigger/");
    p = append_str(p, MakerIFTTT_Event);
    p = append_str(p, "/with/key/");
    p = append_str(p, MakerIFTTT_Key);
    p = append_str(p, " HTTP/1.1\r\n");
    p = append_str(p, "Host: maker.ifttt.com\r\n");
    p = append_str(p, "Content-Type: application/json\r\n");
    p = append_str(p, "Content-Length: ");
    content_length_here = p;
    p = append_str(p, "NN\r\n");
    p = append_str(p, "\r\n");
    json_start = p;
    p = append_str(p, "{\"value1\":\"");
    p = append_str(p, "");
    p = append_str(p, "\"}");

    compi= strlen(json_start);
    content_length_here[0] = '0' + (compi/10);
    content_length_here[1] = '0' + (compi%10);
    client.print(post_rqst);

  }
    flag=1;
    }
  }
  else
  {
    noTone(buzzer);
    Firebase.setString("Buzzer ",b);
  }
  delay(100);
}
