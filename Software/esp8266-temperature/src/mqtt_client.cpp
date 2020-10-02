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

#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoLog.h>

#include "mqtt_client.h"
#include "rtc.h"

#define LOG_AS "[MQTT] "

static WiFiClient wifiClient;
static PubSubClient mqtt(wifiClient);
static RtcMemory rtc = RtcMemory::getInstance();

constexpr const char *staticTopic = "/revai/sensors";
constexpr const char *staticMeasurment = "{ \"temperature\":%.2f, \"humidity\":%.2f, \"pressure\":%.2f, \"system\":{ \"rssi\":%d, \"voltage\":%d } }";
constexpr const char *staticStatus = "{ \"rssi\":%d, \"voltage\":%d }";

MqttClient::MqttClient(){
    sniprintf(clientId, sizeof(clientId)-1, "ESP%04X", ESP.getChipId());
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

bool MqttClient::connect(const uint32_t timeout) {
    uint32_t start_ms = millis();

    while(!mqtt.connected()) {
        mqtt.connect(clientId, setting.data.mqtt_login, setting.data.mqtt_password);
        if((millis() - start_ms) > timeout) {
            Log.error(F(LOG_AS "Connection timed out at %l ms" CR), millis());
            return false;
        }
    }
    return true;
}

void MqttClient::sendMeasurment(float temperature, float humidity, float pressure) {
    
}

void MqttClient::sendStatus(int32_t rssi, uint16_t mv) {
    
}

#if 0
        // subscribe to setup message
        client.subscribe()

        // publish the measurements
        snprintf(topic, sizeof(topic), "%s/%s", staticTopic, clientId);
        snprintf(payload, sizeof(payload), staticMessage, temp, humidity, pressure, WiFi.RSSI(), vcc);
        Log.verbose(F("msg: %s" CR), payload);
        client.publish(topic, payload);
#endif