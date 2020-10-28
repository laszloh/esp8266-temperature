/**
 * @file main.cpp
 * @author Laszlo Hegedüs (laszlo.hegedues@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-09-30
 * 
 * @copyright Copyright (c) 2020 under the MIT License
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
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_BMP085.h>
#include <ArduinoLog.h>

#include "rtc.h"
#include "settings.h"
#include "setup_ap.h"
#include "mqtt_client.h"
#include "wifi.h"
#include "main.h"

#define LOG_AS  "[MAIN] "

static Adafruit_BME280 bme280;
static Adafruit_BMP085 bmp180;
static MqttClient& client = MqttClient::instace();
static RtcMemory& rtc = RtcMemory::instance();
static NvsSettings& nvs = NvsSettings::instance();
static Main& mainClass = Main::instance();

static uint32_t boot_time;
static float temp = NAN;
static float pressure = NAN;
static float humidity = NAN;

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

void enter_sleep(void) {
    uint32_t curTime;

    Log.notice(F(LOG_AS "Going to deep sleep" CR));
    curTime = millis()-boot_time;
    rtc.setLastWakeDuration(curTime);
    rtc.WriteRtcMemory();
    Log.verbose(F(LOG_AS "Was awake for %l ms" CR), curTime);
    Serial.flush();
    ESP.deepSleepInstant(mainClass.getSleepTime(), WAKE_NO_RFCAL);
    delay(10);
}

void setup() {
    bool forceConfig = false;

    system_update_cpu_freq(160);
    randomSeed(analogRead(0));

    boot_time = millis();

    Serial.begin(74880);
    while(!Serial);

    Log.begin(LOG_LEVEL_VERBOSE, &Serial);
    Log.notice(F(LOG_AS "ESP8266 startup..." CR));

    pinMode(D0, WAKEUP_PULLUP);
    rtc.begin();

    Log.verbose(F(LOG_AS "Reset reason: %X" CR), ESP.getResetInfoPtr()->reason);

    if(rtc.isRtcValid()) {
        if (ESP.getResetInfoPtr()->reason == REASON_EXT_SYS_RST) {
            Log.verbose(F(LOG_AS "Reset count: %d" CR), rtc.getResetCounter());
            if(rtc.getResetCounter() < 2) {
                // count to two external resets before enetering captive portal
                rtc.incResetCounter();
            } else {
                forceConfig = true;
                rtc.setResetCounter(0);
            }
            rtc.WriteRtcMemory();
        }
    }

    // load config from EEPROM
    if(!nvs.isOpened() || forceConfig) {
        // if we fail to load the settings, launch AP
        rtc.setReconfigure(false);
        rtc.WriteRtcMemory();
        if(!setup_ap()) {
            // setup AP timed out
            Log.error(F(LOG_AS "Setup AP timed out. Going to forced sleep."));
            enter_sleep();
        }
    }

    // boot up the wifi to send the data
    WiFiModule::instance().begin();

    // boot up the BME280
    if(bme280.begin(BME280_ADDRESS_ALTERNATE)) {
        // we have a BME280 module on board
        bme280.setSampling(Adafruit_BME280::MODE_FORCED,
                        Adafruit_BME280::SAMPLING_X1, // temperature
                        Adafruit_BME280::SAMPLING_X1, // pressure
                        Adafruit_BME280::SAMPLING_X1, // humidity
                        Adafruit_BME280::FILTER_OFF);
        bme280.takeForcedMeasurement();
        pressure = bme280.readPressure();
        Log.verbose(F(LOG_AS "Current Pressure: %l" CR), (uint16_t)pressure);
        humidity = bme280.readHumidity();
        Log.verbose(F(LOG_AS "Current Humidity: %l" CR), (uint16_t)(humidity*100));
        temp = bme280.readTemperature();
        Log.verbose(F(LOG_AS "Current Temperature: %l" CR), (uint16_t)(temp*100));
    } else if(bmp180.begin(BMP085_ULTRALOWPOWER)) {
        // we have a BMP180 on board
        pressure = bmp180.readPressure();
        temp = bmp180.readTemperature();
    } else {
        // neigther BME nor BMP were found
        Log.error(F(LOG_AS "Could not find a valid BME280 or BMP180 sensor, check wiring, address, sensor ID!" CR));
        enter_sleep();
    }

    if(!WiFiModule::instance().connect()) {
        Log.error(F(LOG_AS "Could not connect to WiFi..." CR));
        Log.error(F(LOG_AS "Giving up at %l ms" CR), (uint16_t)(millis()-boot_time));
        Log.error(F(LOG_AS "resetting rtc and going to forced sleep..." CR));
        rtc.InvalidiateRtcMemory();
        enter_sleep();
    }
    Log.verbose(F(LOG_AS "Connect at %l ms" CR), (uint16_t)(millis()-boot_time));

    station_config wifi_conf;
    wifi_station_get_config(&wifi_conf);
    rtc.setBssid(wifi_conf.bssid);
    rtc.setChannel(wifi_get_channel());
    rtc.setIp(WiFi.localIP());
    rtc.setGateway(WiFi.gatewayIP());
    rtc.setMask(WiFi.subnetMask());
    rtc.setDns(WiFi.dnsIP());
    rtc.setResetCounter(0);
    rtc.WriteRtcMemory();

    // Start the MQTT party
    client.begin();
    if(!client.connect())
        enter_sleep();
}

void loop() {
    uint16_t vcc = ESP.getVcc();
    client.sendMeasurement(temp, humidity, pressure);
    client.sendStatus(vcc);

    for(auto i=0;i<50;i++) {
        client.loop();
        delay(10);
    }
    enter_sleep();
}
