#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "rtc.h"

#include "wifi.h" // #defines SSID and PASSWD
constexpr const char *ssid = SSID;
constexpr const char *pass = PASSWD;

constexpr uint32_t timeout = 500;               //< wifi connect timeout
constexpr uint32_t sleep_time = 30*1000*1000;   //< deep sleep time in us

constexpr const char *mqtt_server = "192.168.0.201";
constexpr const char *staticId = "ESP8266";
constexpr const char *staticTopic = "/revai/sensors/";

#define SERIAL_DEBUG 1

#ifdef SERIAL_DEBUG
    #define print(...)  Serial.print(__VA_ARGS__)
    #define println(...)  Serial.println(__VA_ARGS__)
#else
    #define print(...)  {}
    #define println(...)  {}
#endif

Adafruit_BME280 bme;
WiFiClient espClient;
PubSubClient client(espClient);
char clientId[64];

ADC_MODE(ADC_VCC)

// preinit() is called before system startup
// from nonos-sdk's user entry point user_init()

void preinit() {
  // Global WiFi constructors are not called yet
  // (global class instances like WiFi, Serial... 
  // are not yet initialized)..
  // No global object methods or C++ exceptions
  // can be called in here!
  // The code line below is a static class method, 
  // which is similar to a function, so it's ok.
  ESP8266WiFiClass::preinitWiFiOff();
}

bool mqtt_connect() {
    uint32_t cur_ms = millis();
 
    while(!client.connected()) {
        client.connect(clientId);
        if(millis() - cur_ms > timeout) {
            println("Failed to connect to mqtt broker...");
            return false;
        }
        delay(1);
    }
    return true;
}

void setup() {
    cfgbuf_t cfg;
    float temp, pressure, humidity;

    system_update_cpu_freq(80);
#ifdef SERIAL_DEBUG
    Serial.begin(115200);
    while(!Serial) { }
#endif
    println("ESP8266 startup...");

    // boot up the BME280
    bool status = bme.begin();
    if(!status) {
        println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        print("SensorID was: 0x"); println(bme.sensorID(),16);
        println("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        println("   ID of 0x56-0x58 represents a BMP 280,\n");
        println("        ID of 0x60 represents a BME 280.\n");
        println("        ID of 0x61 represents a BME 680.\n");
#ifdef SERIAL_DEBUG
        while (1)
            delay(10);
#else
        return;
#endif
    }
    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1, // temperature
                    Adafruit_BME280::SAMPLING_X1, // pressure
                    Adafruit_BME280::SAMPLING_X1, // humidity
                    Adafruit_BME280::FILTER_OFF);
    bme.takeForcedMeasurement();
    pressure = bme.readPressure();
    print("Current Pressure: ");println(pressure);
    humidity = bme.readHumidity();
    print("Current Humidity: ");println(humidity);
    temp = bme.readTemperature();
    print("Current Temperature: ");println(temp);

    // boot up the wifi to send the data
    if(readcfg(&cfg)) {
        WiFi.forceSleepWake();
        WiFi.config(cfg.ip, cfg.gw, cfg.msk, cfg.dns);
        WiFi.begin(ssid, pass, cfg.chl, cfg.bssid);
    } else {
        // no data in rtc, normal startup
        WiFi.forceSleepWake();
        WiFi.begin(ssid, pass);
    }

    uint32_t cur_ms = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if(millis() - cur_ms > timeout) {
            println("Could not connect to WiFi...");
            println("going to forced sleep...");
            return;
        }
        delay(1);
    }

    client.setServer(mqtt_server, 1883);
    snprintf(clientId, sizeof(clientId), "%s-%06X", staticId, system_get_chip_id());

    if(mqtt_connect()) {
        char topic[128];
        char payload[128];

        // publish the temperature
        snprintf(topic, sizeof(topic), "%s/%s/temperature", staticTopic, clientId);
        dtostrf(temp, 2, 2, payload);
        client.publish(topic, payload);

        // publish the temperature
        snprintf(topic, sizeof(topic), "%s/%s/humidity", staticTopic, clientId);
        dtostrf(humidity, 2, 2, payload);
        client.publish(topic, payload);

        // publish the temperature
        snprintf(topic, sizeof(topic), "%s/%s/pressure", staticTopic, clientId);
        dtostrf(pressure, 2, 2, payload);
        client.publish(topic, payload);

        uint16_t vcc = ESP.getVcc();
        print("Current battery voltage: ");println(vcc);
        // publish the battery voltage
        snprintf(topic, sizeof(topic), "%s/%s/state/voltage", staticTopic, clientId);
        snprintf(payload, "%d", vcc);
        client.publish(topic, payload);
    }

    ESP.deepSleep(sleep_time, WAKE_NO_RFCAL);
    delay(5000);
}

void loop() {
    // if we got here, goto sleep
    ESP.deepSleep(sleep_time, WAKE_NO_RFCAL);
    delay(5000);
}

