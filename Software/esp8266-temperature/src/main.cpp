#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "LittleFS.h"

#include "rtc.h"

#include "wifi.h" // #defines SSID and PASSWD
constexpr const char *ssid = SSID;
constexpr const char *pass = PASSWD;

constexpr uint32_t timeout = 2000;               //< wifi connect timeout
constexpr uint32_t sleep_time = 15*60*1000*1000;   //< deep sleep time in us
//constexpr uint32_t sleep_time = 5*1000*1000;   //< deep sleep time in us

constexpr const char *mqtt_server = "192.168.88.12";
constexpr const char *staticId = "ESP8266";
constexpr const char *staticTopic = "/revai/sensors";

constexpr const char *staticMessage = "{ \"temperatue\":%.2f, \"humidity\":%.2f, \"pressure\":%.2f, \"system\":{ \"rssi\":%d, \"voltage\":%d } }";

#define DEBUG

#ifdef DEBUG
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

uint64_t boot_time;

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

bool mqtt_connect(void) {
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

void enter_sleep(void) {
    println("Going to deep sleep");
    print("Was awake for: ");print((uint32_t)(millis()-boot_time),10);println(" ms");
    ESP.deepSleep(sleep_time, WAKE_NO_RFCAL);
    delay(10);
}

void setup() {
    cfgbuf_t cfg;
    float temp, pressure, humidity;

    system_update_cpu_freq(80);
    boot_time = millis();
#ifdef DEBUG
    Serial.begin(74880);
    while(!Serial) { }
#endif
    println("ESP8266 startup...");

    pinMode(D0, WAKEUP_PULLUP);

    // boot up the BME280
    bool status = bme.begin(BME280_ADDRESS_ALTERNATE);
    if(!status) {
        println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        print("SensorID was: 0x"); println(bme.sensorID(),16);
        println("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        println("   ID of 0x56-0x58 represents a BMP 280,\n");
        println("        ID of 0x60 represents a BME 280.\n");
        println("        ID of 0x61 represents a BME 680.\n");
#ifdef DEBUG
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
    print("RTC config: ");
    uint8_t multi = 1;
    if(readcfg(&cfg)) {
        println(" found");
        WiFi.forceSleepWake();
        WiFi.config(cfg.ip, cfg.gw, cfg.msk, cfg.dns);
        WiFi.begin(ssid, pass, cfg.chl, cfg.bssid);
    } else {
        // no data in rtc, normal startup
        println(" NOT found");
        WiFi.forceSleepWake();
        WiFi.begin(ssid, pass);
        multi = 3; 
    }

    uint32_t cur_ms = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if(millis() - cur_ms > timeout * multi) {
            print("Giving up at: ");print((uint32_t)(millis()-boot_time),10);println(" ms");
            println("Could not connect to WiFi...");
            println("going to forced sleep...");
            return;
        }
        delay(1);
    }
    print("Connect at: ");print((uint32_t)(millis()-boot_time),10);println(" ms");

    writecfg();

    client.setServer(mqtt_server, 1883);
    client.setSocketTimeout(1);
    snprintf(clientId, sizeof(clientId), "%s-%06X", staticId, system_get_chip_id());

    if(mqtt_connect()) {
        char topic[128];
        char payload[128];
        uint16_t vcc = ESP.getVcc();

        // publish the measurements
        snprintf(topic, sizeof(topic), "%s/%s", staticTopic, clientId);
        snprintf(payload, sizeof(payload), staticMessage, temp, humidity, pressure, WiFi.RSSI(), vcc);
        print("msg: ");println(payload);
        client.publish(topic, payload);

        // subscribe to setup message
        // ToDo: implement

        delay(500);
    }
    enter_sleep();
}

void loop() {
    // if we got here, goto sleep
    enter_sleep();
}
