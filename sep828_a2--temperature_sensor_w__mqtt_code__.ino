#include <SoftwareSerial.h>
#include "WiFiEsp.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WLAN_SSID "XXXXX" // your network SSID (name)
#define WLAN_PASS "XXXXXXX" // your network password
int status = WL_IDLE_STATUS; // the Wifi radio's 

#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883 // use 8883 for SSL
#define AIO_USERNAME "XXXXXX" // This is from “key” access window in Section 5
#define AIO_KEY "aio_lVizXXXXXXXXXXXXX" // This is from “key” access window in Section 5

WiFiEspClient client; 

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

SoftwareSerial Serial1(6, 5); //RX is digital pin 6 (connect to TX of device)
                               //TX is digital pin 5 (connect to RX of device)

Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");

const int sensorPin = A0;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  WiFi.init(&Serial1);
  
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
     // don't continue
    while (true);
  }
  
  if( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(WLAN_SSID);
    // Connect to WPA/WPA2 network
    // status =
    WiFi.begin(WLAN_SSID, WLAN_PASS);
    while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
    Serial.println("Connected ");
    Serial.println(WiFi.localIP());
  }   
}

void loop() {
  MQTT_connect();
  //read analog value of sensor
  int sensorValue = analogRead(sensorPin); 
  
  //covert analog value to voltage
  float sensorVoltage = (sensorValue / 1024.0) * 5.0; 

  //convert sensor voltage to temperature in degree C & print out
  float sensorTemp = (sensorVoltage -0.5) * 100;
  Serial.print( "sensorTemp is: ");
  Serial.print( sensorTemp);
  Serial.println(" degree C");

  //publish data to cloud server & ck if data is sent successfully
  if (temperature.publish(sensorTemp)) {
    Serial.println("data sent!");
  }
  else{
    Serial.println("failed to send data");
  }
  
  delay(1000);
}

void MQTT_connect() {
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
  return;
  }
  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5); // wait 5 seconds
    retries--;
    mqtt.connect();
    if (retries == 0) {
     // basically die and wait for WDT to reset me
     break;
     //while (1);
   }
  }
  Serial.println("MQTT Connected!");
}
