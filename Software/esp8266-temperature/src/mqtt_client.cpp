/**
 * @file mqtt_client.cpp
 * @author Laszlo Hegedüs (laszlo.hegedues@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-10-02
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
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoLog.h>
#include <ArduinoJson.h>

#include "mqtt_client.h"
#include "rtc.h"

#define LOG_AS "[MQTT] "

static WiFiClient wifiClient;
static PubSubClient mqtt(wifiClient);
static RtcMemory& rtc = RtcMemory::instance();

constexpr const char *staticSetupTopic = "/revai/sensors/setup";

MqttClient::MqttClient(){
    snprintf(clientId, sizeof(clientId)-1, "ESP%04X", ESP.getChipId());
}

void MqttClient::begin(const settings_t &sett) {
    IPAddress mqttIp;
    setting = sett;

    wifiClient.setNoDelay(true);
    if(rtc.isRtcValid())
        mqttIp = rtc.getMqttServerIp();

    if(mqttIp.isSet())
        mqtt.setServer(mqttIp, setting.data.mqtt_port);
    else
        mqtt.setServer(setting.data.mqtt_host, setting.data.mqtt_port);
    mqtt.setSocketTimeout(1);
}

void MqttClient::callback(char* topic, uint8_t* payload, uint16_t length) {
    StaticJsonDocument<200> jsonDoc;
    deserializeJson(jsonDoc, payload, length);

    bool forceSetup = jsonDoc["setup-force"];
    bool forceBoot = jsonDoc["boot-force"];

    rtc.setReconfigure(forceSetup);
    rtc.setBootloader(forceBoot);

    if(forceBoot || forceSetup) {
        Log.notice(F(LOG_AS "Entering %s mode" CR), (forceSetup) ? F("setup") : F("bootloader"));
        Serial.flush();
        ESP.wdtDisable();
        while(1);
    }
}

bool MqttClient::connect(const uint32_t timeout) {
    uint32_t start_ms = millis();

    while(!mqtt.connected()) {
        mqtt.connect(clientId, setting.data.mqtt_login, setting.data.mqtt_password, 0, 0, 0, 0, false);
        if((millis() - start_ms) > timeout) {
            Log.error(F(LOG_AS "Connection timed out at %l ms" CR), millis());
            return false;
        }
    }
    mqtt.setCallback(callback);
    mqtt.subscribe(staticSetupTopic);

    return true;
}

void MqttClient::loop() {
    mqtt.loop();
}

void MqttClient::sendMeasurement(float temperature, float humidity, float pressure) {
    char topic[64];
    char message[512];
    DynamicJsonDocument jsonDoc(1024);

    jsonDoc["temperature"] = temperature;
    jsonDoc["humidity"] = humidity;
    jsonDoc["pressure"] = pressure;

    size_t n = serializeJson(jsonDoc, message);
    snprintf(topic, sizeof(topic), setting.data.mqtt_topic, clientId, "measurement");
    Log.verbose(F(LOG_AS "Topic:   %s" CR), topic);
    Log.verbose(F(LOG_AS "Message: %s" CR), message);
    mqtt.publish(topic, message, n);
}

void MqttClient::sendStatus(uint16_t voltage) {
    char topic[64];
    char message[512];
    DynamicJsonDocument doc(1024);
    int32_t rssi = WiFi.RSSI();

    doc["wakeTime"] = rtc.getLastWakeDuration();;
    doc["Voltage"] = voltage;

    JsonObject Memory = doc.createNestedObject(F("Memory"));
    Memory["StackFree"] = ESP.getFreeContStack();
    Memory["HeapFree"] = ESP.getFreeHeap();

    JsonObject wifi = doc.createNestedObject(F("WiFi"));
    wifi["SSId"] = WiFi.SSID();
    wifi["BSSId"] = WiFi.BSSIDstr();
    wifi["Channel"] = WiFi.channel();
    wifi["RSSI"] = getRssiQuality(rssi);
    wifi["Signal"] = rssi;

    size_t n = serializeJson(doc, message);
    snprintf(topic, sizeof(topic), setting.data.mqtt_topic, clientId, "status");
    Log.verbose(F(LOG_AS "Topic:   %s" CR), topic);
    Log.verbose(F(LOG_AS "Message: %s" CR), message);
    mqtt.publish(topic, message, n);
}

int8_t MqttClient::getRssiQuality(const int32_t rssi) const{
    int8_t quality = 0;

    if(rssi >= -50)
        quality = 100;
    else if(rssi >= -100)
        quality = 2 * (rssi + 100);
    return quality;
}