#include <LWiFi.h>
#include <WiFiClient.h>
#include "MCS.h"


#include "DHT.h" 

#define DHTPIN A0     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)


DHT dht(DHTPIN, DHTTYPE);

// Assign AP ssid / password here
#define _SSID "your_ssid"
#define _KEY  "your_password"

// Assign device id / key of your test device
//MCSLiteDevice mcs("DeviceId","DeviceKey","MCS-Lite address",port)
MCSLiteDevice mcs("Id", "Key","MCS-Lite IP",3000);

// Assign channel id 
// The test device should have 4 channel
// the first channel should be "Controller" - "On/Off"
// the secord channel should be "Display" - "On/Off"
// the third channel should be "Display" - "Float"
// the fourth channel should be "Display" - "Float"
MCSControllerOnOff led("LED_switch");
MCSDisplayOnOff    remote("LED_display");


//Temperature and Humidity
MCSDisplayFloat     temp("Temperature");
MCSDisplayFloat     hum("Humidity");


#define LED_PIN 7

void setup() {
  // setup Serial output at 9600
  Serial.begin(9600);

  // setup LED/Button pin
  pinMode(LED_PIN, OUTPUT);

//Temperature and Humidity
  dht.begin();
  
  // setup Wifi connection
  while(WL_CONNECTED != WiFi.status())
  {
    Serial.print("WiFi.begin(");
    Serial.print(_SSID);
    Serial.print(",");
    Serial.print(_KEY);
    Serial.println(")...");
    WiFi.begin(_SSID, _KEY);
  }
  Serial.println("WiFi connected !!");

  // setup MCS connection
  mcs.addChannel(led);
  mcs.addChannel(remote);

//Temperature and Humidity  
  mcs.addChannel(temp);
  mcs.addChannel(hum);

  while(!mcs.connected())
  {
    Serial.println("MCS.connect()...");
    mcs.connect();
  }
  Serial.println("MCS connected !!");

  // read LED value from MCS server
  while(!led.valid())
  {
    Serial.println("read LED value from MCS...");
    led.value();
  }
  Serial.print("done, LED value = ");
  Serial.println(led.value());
  digitalWrite(LED_PIN, led.value() ? HIGH : LOW);
}

void loop() {
  
  //get Temperature and Humidity
  float DHT_t = dht.readTemperature();
  float DHT_h = dht.readHumidity();
  //print 
  Serial.println(DHT_t);
  Serial.println(DHT_h);
  // set   
  temp.set(DHT_t);
  hum.set(DHT_h);
  
  delay(1000);
  
  // call process() to allow background processing, add timeout to avoid high cpu usage
  Serial.print("process(");
  Serial.print(millis());
  Serial.println(")");
  mcs.process(100);
  
  // updated flag will be cleared in process(), user must check it after process() call.
  if(led.updated())
  {
    Serial.print("LED updated, new value = ");
    Serial.println(led.value());
    digitalWrite(LED_PIN, led.value() ? HIGH : LOW);
    if(!remote.set(led.value()))
    {
      Serial.print("Failed to update remote");
      Serial.println(remote.value());
    }
  }
  
  // check if need to re-connect
  while(!mcs.connected())
  {
    Serial.println("re-connect to MCS...");
    mcs.connect();
    if(mcs.connected())
      Serial.println("MCS connected !!");
  }
}
