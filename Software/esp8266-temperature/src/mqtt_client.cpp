/**
 * @file mqtt_client.cpp
 * @author Laszlo Hegedüs (laszlo.hegedues@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-10-02
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
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoLog.h>
#include <ArduinoJson.h>

#include "mqtt_client.h"
#include "rtc.h"
#include "settings.h"
#include "wifi.h"
#include "main.h"

#define LOG_AS "[MQTT] "

static WiFiClient wifiClient;
static PubSubClient mqtt(wifiClient);

#define PH_MAC  "{m}"
#define PH_ID   "{i}"
#define PH_TYPE "{t}"

MqttClient::MqttClient() : 
    rtc(RtcMemory::instance()), 
    settings(NvsSettings::instance())
{
    String id = settings.config().mqtt_id;
    id.replace(F(PH_MAC), String(ESP.getChipId(), 16));
    strlcpy(clientId, id.c_str(), sizeof(clientId));
}

void MqttClient::begin() {
    uint16_t port = settings.config().mqtt_port;
    
    wifiClient.setNoDelay(true);
    if(rtc.isRtcValid() && rtc.getMqttServerIp().isSet())
        mqtt.setServer(rtc.getMqttServerIp(), port);
    else
        mqtt.setServer(settings.config().mqtt_host.c_str(), port);
    mqtt.setSocketTimeout(1);
}

void MqttClient::callback(char* topic, uint8_t* payload, uint16_t length) {
    NvsSettings& settings = NvsSettings::instance();
    StaticJsonDocument<200> jsonDoc;

    Log.verbose(F(LOG_AS "Got a new setup topic" CR));

    deserializeJson(jsonDoc, payload, length);
    if(settings.loadConfig(jsonDoc)) {
        Log.notice(F(LOG_AS "New settings received. Saving and rebooting..." CR));
        settings.saveConfig();
        
        Serial.flush();
        ESP.reset();
    }
}

bool MqttClient::connect() {
    uint32_t start_ms = millis();
    const uint32_t timeout = settings.config().mqtt_timeout;

    while(!mqtt.connected()) {
        mqtt.connect(clientId, settings.config().mqtt_login.c_str(), settings.config().mqtt_pass.c_str(), 0, 0, 0, 0, false);
        if((millis() - start_ms) > timeout) {
            Log.error(F(LOG_AS "Connection timed out at %l ms" CR), millis());
            return false;
        }
    }
    rtc.setMqttServerIp(wifiClient.remoteIP());

    mqtt.setCallback(callback);
    mqtt.setBufferSize(1024);

    String topic = settings.config().mqtt_topic.c_str();
    topic.replace(F(PH_ID), clientId);
    topic.replace(F(PH_TYPE), F("setup"));
    mqtt.subscribe(topic.c_str());

    topic = settings.config().mqtt_topic.c_str();
    topic.replace(F(PH_ID), F("all"));
    topic.replace(F(PH_TYPE), F("setup"));
    mqtt.subscribe(topic.c_str());

    return true;
}

void MqttClient::loop() {
    mqtt.loop();
}

void MqttClient::sendMeasurement(float temperature, float humidity, float pressure) {
    String topic = settings.config().mqtt_topic.c_str();
    char message[512];
    DynamicJsonDocument jsonDoc(1024);

    jsonDoc["temperature"] = temperature;
    jsonDoc["humidity"] = humidity;
    jsonDoc["pressure"] = pressure;

    size_t n = serializeJson(jsonDoc, message);

    topic.replace(F(PH_ID), clientId);
    topic.replace(F(PH_TYPE), F("measurement"));

    Log.verbose(F(LOG_AS "Topic:   %s" CR), topic.c_str());
    Log.verbose(F(LOG_AS "Message: %s" CR), message);
    mqtt.publish(topic.c_str(), message, n);
}

void MqttClient::sendStatus(uint16_t voltage) {
    String topic = settings.config().mqtt_topic.c_str();
    char message[512];
    DynamicJsonDocument doc(1024);
    int32_t rssi = WiFi.RSSI();

    doc["wakeTime"] = rtc.getLastWakeDuration();;
    doc["Voltage"] = voltage;
    doc["fingerprint"] = settings.config().fingerprint;

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

    topic.replace(F(PH_ID), clientId);
    topic.replace(F(PH_TYPE), F("status"));

    Log.verbose(F(LOG_AS "Topic:   %s" CR), topic.c_str());
    Log.verbose(F(LOG_AS "Message: %s" CR), message);
    mqtt.publish(topic.c_str(), message, n);
}

int8_t MqttClient::getRssiQuality(const int32_t rssi) const{
    int8_t quality = 0;

    if(rssi >= -50)
        quality = 100;
    else if(rssi >= -100)
        quality = 2 * (rssi + 100);
    return quality;
}