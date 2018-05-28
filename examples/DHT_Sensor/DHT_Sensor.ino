#include "IOTMonitor.h"
#if defined(ARDUINO_AVR_YUN)
#include "YunClient.h"
YunClient client;
#else
#if defined(USE_WIFI101_SHIELD) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_ARCH_ESP8266)
// Use WiFi
#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#else
#include <SPI.h>
#include <WiFi101.h>
#endif
char ssid[] = "<YOURNETWORK>";    //  your network SSID (name)
char pass[] = "<YOURPASSWORD>";   // your network password
int status = WL_IDLE_STATUS;
WiFiClient  client;
#elif defined(USE_ETHERNET_SHIELD)
// Use wired ethernet shield
#include <SPI.h>
#include <Ethernet.h>
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
EthernetClient client;
#endif
#endif

#ifdef ARDUINO_ARCH_AVR
// On Arduino:  0 - 1023 maps to 0 - 5 volts
#define VOLTAGE_MAX 5.0
#define VOLTAGE_MAXCOUNTS 1023.0
#elif ARDUINO_SAMD_MKR1000
// On MKR1000:  0 - 1023 maps to 0 - 3.3 volts
#define VOLTAGE_MAX 3.3
#define VOLTAGE_MAXCOUNTS 1023.0
#elif ARDUINO_SAM_DUE
// On Due:  0 - 1023 maps to 0 - 3.3 volts
#define VOLTAGE_MAX 3.3
#define VOLTAGE_MAXCOUNTS 1023.0
#elif ARDUINO_ARCH_ESP8266
// On ESP8266:  0 - 1023 maps to 0 - 1 volts
#define VOLTAGE_MAX 1.0
#define VOLTAGE_MAXCOUNTS 1023.0
#endif
int ChannelNumber = My Channel ID;
String WriteApiKey = "My API Key";
#include "DHT.h"
#define DHTPIN 2
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);
void setup() {
#ifdef ARDUINO_AVR_YUN
  Bridge.begin();
#else
#if defined(ARDUINO_ARCH_ESP8266) || defined(USE_WIFI101_SHIELD) || defined(ARDUINO_SAMD_MKR1000)
  WiFi.begin(ssid, pass);
#else
  Ethernet.begin(mac);
#endif
#endif

  IOTMonitor.begin(client);

}

void loop() {
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
	delay(20000);
    return;
  }
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");
  IOTMonitor.SendData(ChannelNumber, 1, t, WriteApiKey);
  IOTMonitor.SendData(ChannelNumber, 2, h, WriteApiKey);
  delay(20000);
}