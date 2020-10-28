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
static RtcMemory& rtc = RtcMemory::instance();

#define PH_MAC  "{m}"
#define PH_ID   "{i}"
#define PH_TYPE "{t}"

MqttClient::MqttClient() : 
    rtc(RtcMemory::instance()), 
    settings(NvsSettings::instance()),
	host(DEFAULT_MQTT_HOST, settings, "mqtt-host"),
	port(DEFAULT_MQTT_PORT, settings, "mqtt-port"),
	login(DEFAULT_MQTT_LOGIN, settings, "mqtt-login"),
	pass(DEFAULT_MQTT_PASS, settings, "mqtt-pass"),
	topic(DEFAULT_MQTT_TOPIC, settings, "mqtt-topic"),
	id(DEFAULT_MQTT_ID, settings, "mqtt-id"),
	timeout(DEFAULT_MQTT_TIMEOUT, settings, "mqtt-timeout")
{
    String id = id;
    id.replace(F(PH_MAC), String(ESP.getChipId(), 16));
    strncpy(clientId, id.c_str(), sizeof(clientId));
}

void MqttClient::begin() {
    IPAddress mqttIp;

    wifiClient.setNoDelay(true);
    if(rtc.isRtcValid())
        mqttIp = rtc.getMqttServerIp();

    if(mqttIp.isSet())
        mqtt.setServer(mqttIp, port);
    else
        mqtt.setServer(host, port);
    mqtt.setSocketTimeout(1);
}

void MqttClient::callback(char* topic, uint8_t* payload, uint16_t length) {
    bool reboot = false;
    NvsSettings& settings = NvsSettings::instance();
    uint32_t fingerprint = settings.getFingerprint();
    StaticJsonDocument<200> jsonDoc;
    Log.verbose(F(LOG_AS "Got a new setup topic" CR));

    deserializeJson(jsonDoc, payload, length);
    Log.verbose(F(LOG_AS "topic-fingerprint: %d our fingerprint: %d" CR), jsonDoc["fingerprint"].as<int>(), fingerprint);

    if(fingerprint < jsonDoc["fingerprint"]) {
        Log.verbose(F(LOG_AS "updating settings" CR));
		// Wifi setup
		WiFiModule::instance().updateSettings(jsonDoc);
		// MQTT
		MqttClient::instace().updateSettings(jsonDoc);
		// System
		Main::instance().updateSettings(jsonDoc);
    }

    if(reboot) {
        Log.notice(F(LOG_AS "New settings received. Rebooting..." CR));
        Serial.flush();
        ESP.reset();
    }
}

void MqttClient::updateSettings(const JsonDocument &jsonDoc) {
    host = jsonDoc["mqtt-host"].as<const char*>();
    port = jsonDoc["mqtt-port"].as<uint32_t>();
    login = jsonDoc["mqtt-login"].as<const char*>();
    pass = jsonDoc["mqtt-pass"].as<const char*>();
    topic = jsonDoc["mqtt-topic"].as<const char*>();
    id = jsonDoc["mqtt-id"].as<const char*>();
    timeout = jsonDoc["mqtt-timeout"].as<uint32_t>();
}

void getDefaultSettings(JsonDocument &jsonDoc) {
	jsonDoc["mqtt-host"] = DEFAULT_MQTT_HOST;
	jsonDoc["mqtt-port"] = DEFAULT_MQTT_PORT;
	jsonDoc["mqtt-login"] = DEFAULT_MQTT_LOGIN;
	jsonDoc["mqtt-pass"] = DEFAULT_MQTT_PASS;
	jsonDoc["mqtt-topic"] = DEFAULT_MQTT_TOPIC;
	jsonDoc["mqtt-id"] = DEFAULT_MQTT_ID;
	jsonDoc["mqtt-timeout"] = DEFAULT_MQTT_TIMEOUT;
}

bool MqttClient::connect() {
    uint32_t start_ms = millis();

    while(!mqtt.connected()) {
        mqtt.connect(clientId, login, pass, 0, 0, 0, 0, false);
        if((millis() - start_ms) > timeout) {
            Log.error(F(LOG_AS "Connection timed out at %l ms" CR), millis());
            return false;
        }
    }
    mqtt.setCallback(callback);
    mqtt.setBufferSize(1024);

    String topic = topic;
    topic.replace(F(PH_ID), clientId);
    topic.replace(F(PH_TYPE), F("setup"));
    mqtt.subscribe(topic.c_str());

    topic = topic;
    topic.replace(F(PH_ID), F("all"));
    topic.replace(F(PH_TYPE), F("setup"));
    mqtt.subscribe(topic.c_str());

    return true;
}

void MqttClient::loop() {
    mqtt.loop();
}

void MqttClient::sendMeasurement(float temperature, float humidity, float pressure) {
    String topic = topic;
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
    String topic = topic;
    char message[512];
    DynamicJsonDocument doc(1024);
    int32_t rssi = WiFi.RSSI();

    doc["wakeTime"] = rtc.getLastWakeDuration();;
    doc["Voltage"] = voltage;
    doc["fingerprint"] = settings.getFingerprint();

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