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
#include "settings.h"

#define LOG_AS "[MQTT] "

static WiFiClient wifiClient;
static PubSubClient mqtt(wifiClient);
static RtcMemory& rtc = RtcMemory::instance();

MqttClient::MqttClient(){
}

void MqttClient::begin(const settings_t &sett) {
    IPAddress mqttIp;
    setting = sett;

    snprintf(clientId, sizeof(clientId)-1, setting.data.mqtt_id, ESP.getChipId());
    Log.verbose(F(LOG_AS "Client id: %s" CR), clientId);

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
    bool reboot = false;
    StaticJsonDocument<200> jsonDoc;
    deserializeJson(jsonDoc, payload, length);
    settings_t sett = MqttClient::instace().setting;
    Log.verbose(F(LOG_AS "Got a new setup topic" CR));
    Log.verbose(F(LOG_AS "topic-fingerprint: %d our fingerprint: %d" CR), jsonDoc["fingerprint"].as<int>(), sett.data.fingerprint);

    // check if we have a new setup
    if(jsonDoc["fingerprint"] > sett.data.fingerprint) {
        reboot = true;
        // we have a newer setting than we had before
        sett.data.fingerprint = jsonDoc["fingerprint"];
        strlcpy(sett.data.wifi_ssid, jsonDoc["wifi-ssid"] | DEFAULT_WIFI_SSID, sizeof(sett.data.wifi_ssid));
        strlcpy(sett.data.wifi_pwd, jsonDoc["wifi-pwd"] | DEFAULT_WIFI_PASS, sizeof(sett.data.wifi_pwd));
        strlcpy(sett.data.mqtt_host, jsonDoc["mqtt-host"] | DEFAULT_MQTT_HOST, sizeof(sett.data.mqtt_host));
        sett.data.mqtt_port = jsonDoc["mqtt-port"] | DEFAULT_MQTT_PORT;
        strlcpy(sett.data.mqtt_login, jsonDoc["mqtt-login"] | DEFAULT_MQTT_LOGIN, sizeof(sett.data.mqtt_login));
        strlcpy(sett.data.mqtt_password, jsonDoc["mqtt-pass"] | DEFAULT_MQTT_PASS, sizeof(sett.data.mqtt_password));
        strlcpy(sett.data.mqtt_topic, jsonDoc["mqtt-topic"] | DEFAULT_MQTT_TOPIC, sizeof(sett.data.mqtt_topic));
        strlcpy(sett.data.mqtt_id, jsonDoc["mqtt-id"] | DEFAULT_MQTT_ID, sizeof(sett.data.mqtt_id));
        sett.data.sleep_time = jsonDoc["sleep-time"] | DEFAULT_ESP_SLEEP;
        saveConfig(sett);
    }

    if(reboot) {
        Log.notice(F(LOG_AS "New settings received. Rebooting..." CR));
        Serial.flush();
        ESP.reset();
    }
}

bool MqttClient::connect(const uint32_t timeout) {
    uint32_t start_ms = millis();
    char setup_topic[128];

    while(!mqtt.connected()) {
        mqtt.connect(clientId, setting.data.mqtt_login, setting.data.mqtt_password, 0, 0, 0, 0, false);
        if((millis() - start_ms) > timeout) {
            Log.error(F(LOG_AS "Connection timed out at %l ms" CR), millis());
            return false;
        }
    }
    mqtt.setCallback(callback);
    mqtt.setBufferSize(1024);
    snprintf(setup_topic, sizeof(setup_topic), setting.data.mqtt_topic, clientId, "setup");
    mqtt.subscribe(setup_topic);
    snprintf(setup_topic, sizeof(setup_topic), setting.data.mqtt_topic, "all", "setup");
    mqtt.subscribe(setup_topic);

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
    doc["fingerprint"] = setting.data.fingerprint;

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