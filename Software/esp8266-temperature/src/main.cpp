/**
 * @file main.cpp
 * @author Laszlo Hegedüs (laszlo.hegedues@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-09-30
 * 
 * \copyright Copyright (c) 2020 under the MIT License
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ArduinoLog.h>

#include "rtc.h"
#include "settings.h"
#include "setup_ap.h"

#define LOG_AS  "[MAIN] "

extern "C" {
#include <user_interface.h>
}

#include "wifi.h" // #defines SSID and PASSWD
constexpr const char *ssid = SSID;
constexpr const char *pass = PASSWD;

constexpr uint32_t timeout = ESP_WIFI_TIMEOUT;     //< wifi connect timeout
constexpr uint32_t sleep_time = 15*60*1000*1000;   //< deep sleep time in us
//constexpr uint32_t sleep_time = 5*1000*1000;     //< deep sleep time in us

constexpr const char *mqtt_server = "192.168.88.12";
constexpr const char *staticTopic = "/revai/sensors";
constexpr const char *staticMessage = "{ \"temperature\":%.2f, \"humidity\":%.2f, \"pressure\":%.2f, \"system\":{ \"rssi\":%d, \"voltage\":%d } }";

Adafruit_BME280 bme;

WiFiClient espClient;
PubSubClient client(espClient);
char clientId[64];

uint64_t boot_time;

ADC_MODE(ADC_VCC)

// preinit() is called before system startup
// from nonos-sdk's user entry point user_init()

RF_PRE_INIT()
{
    system_phy_set_powerup_option(2);
}

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

bool mqtt_connect(const char*clientId) {
    uint32_t cur_ms = millis();
 
    while(!client.connected()) {
        client.connect(clientId);
        if(millis() - cur_ms > timeout) {
            Log.error(F("Failed to connect to mqtt broker..." CR));
            return false;
        }
        delay(1);
    }
    return true;
}

void enter_sleep(void) {
    uint16_t curTime;

    Log.notice(F("Going to deep sleep" CR));
    curTime = millis()-boot_time;
    Log.verbose(F("Was awake for %l ms" CR), curTime);
    Serial.flush();
    ESP.deepSleepInstant(sleep_time, WAKE_NO_RFCAL);
    delay(10);
}

void setup() {
    cfgbuf_t cfg;
    float temp, pressure, humidity;
    uint16_t curTime;
    settings_t sett;

    system_update_cpu_freq(160);
    randomSeed(analogRead(0));

    boot_time = millis();

    Serial.begin(74880);
    while(!Serial);

    Log.begin(LOG_LEVEL_VERBOSE, &Serial);

    Log.notice(F("ESP8266 startup..." CR));

    pinMode(D0, WAKEUP_PULLUP);

    // ap testing
    Log.notice(F("Testing Access Point" CR));
    loadConfig(sett);
    setup_ap(sett);

    // boot up the wifi to send the data
    Log.notice(F("RTC config: " CR));
    if(read_rtc_memory(&cfg)) {
        Log.notice(F("    found" CR));
        WiFi.forceSleepWake();
        WiFi.config(cfg.ip, cfg.gw, cfg.msk, cfg.dns);
        WiFi.begin(ssid, pass, cfg.chl, cfg.bssid);
    } else {
        // no data in rtc, normal startup
        Log.notice(F("    NOT found" CR));
        WiFi.forceSleepWake();
        WiFi.begin(ssid, pass);
    }

    // boot up the BME280
    bool status = bme.begin(BME280_ADDRESS_ALTERNATE);
    if(!status) {
        Log.error(F("Could not find a valid BME280 sensor, check wiring, address, sensor ID!" CR));
        Log.error(F("SensorID was: %X" CR), bme.sensorID()); 
        Log.error(F("    ID of 0xFF probably means a bad address, a BMP 180 or BMP 085" CR));
        Log.error(F("    ID of 0x56-0x58 represents a BMP 280," CR));
        Log.error(F("    ID of 0x60 represents a BME 280." CR));
        Log.error(F("    ID of 0x61 represents a BME 680." CR));
        enter_sleep();
    }
    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1, // temperature
                    Adafruit_BME280::SAMPLING_X1, // pressure
                    Adafruit_BME280::SAMPLING_X1, // humidity
                    Adafruit_BME280::FILTER_OFF);
    bme.takeForcedMeasurement();
    pressure = bme.readPressure();
    Log.verbose(F("Current Pressure: %l" CR), (uint32_t)(pressure*100));
    humidity = bme.readHumidity();
    Log.verbose(F("Current Humidity: %l" CR), (uint16_t)(humidity*100));
    temp = bme.readTemperature();
    Log.verbose(F("Current Temperature: %l" CR), (uint16_t)(temp*100));

    uint32_t cur_ms = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if(millis() - cur_ms > timeout) {
            Log.error(F("Could not connect to WiFi..." CR));
            curTime = millis()-boot_time;
            Log.error(F("Giving up at %l ms" CR), curTime);
            Log.error(F("going to forced sleep... CR"));
            enter_sleep();
        }
        delay(10);
    }
    curTime = millis()-boot_time;
    Log.verbose(F("Connect at %l ms" CR), curTime);

    write_rtc_memory();

    espClient.setNoDelay(true);
    client.setServer(mqtt_server, 1883);
    client.setSocketTimeout(1);
    snprintf(clientId, sizeof(clientId)-1, "ESP%04x", ESP.getChipId());

    if(mqtt_connect(clientId)) {
        char topic[128];
        char payload[128];
        uint16_t vcc = ESP.getVcc();

        // subscribe to setup message
        // ToDo: implement

        // publish the measurements
        snprintf(topic, sizeof(topic), "%s/%s", staticTopic, clientId);
        snprintf(payload, sizeof(payload), staticMessage, temp, humidity, pressure, WiFi.RSSI(), vcc);
        Log.verbose(F("msg: %s" CR), payload);
        client.publish(topic, payload);

//        delay(500);
    }
    enter_sleep();
}

void loop() {
    // if we got here, goto sleep
    enter_sleep();
}
