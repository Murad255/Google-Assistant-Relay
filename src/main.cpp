#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define MQTT_NAME  "Murad"
#define MQTT_PASS  "e3a6b8cf188e49bb884f938242dfc949"

#define WIFI_SSID "Keenetic-6756"
#define WIFI_PASS "03403blo"

#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define relay D4

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

Adafruit_MQTT_Subscribe text = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/feeds/Relay1");
Adafruit_MQTT_Subscribe relay2 = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/feeds/relay2");



void MQTT_connect();

void setup()
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(relay,1);
  //Connect to WiFi
  Serial.print("\n\nConnecting Wifi... ");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(LED_BUILTIN, 0);
    delay(100);
    digitalWrite(LED_BUILTIN, 1);
  }

  Serial.println("OK!");

  //Subscribe to the onoff topic
  mqtt.subscribe(&relay2);
  mqtt.subscribe(&text);

}

void loop()
{

  MQTT_connect();
  
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &relay2) {
      Serial.print(F("Got: "));
      Serial.println((char *)relay2.lastread);
      String txt = String((char *)relay2.lastread);
      if(txt=="on")       digitalWrite(LED_BUILTIN, 0);
      else if(txt=="off") digitalWrite(LED_BUILTIN, 1);
      }
    if (subscription == &text) {
      Serial.print(F("Got text: "));
      Serial.println((char *)text.lastread);
      String txt = String((char *)text.lastread);
      if(txt=="on")       digitalWrite(relay, 1);
      else if(txt=="off") digitalWrite(relay, 0);
    }
  }
}


void MQTT_connect() 
{
  int8_t ret;
  // Stop if already connected
  if (mqtt.connected())
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
  { 
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) 
    {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}