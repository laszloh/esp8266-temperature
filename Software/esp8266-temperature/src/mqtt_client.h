/**
 * @file mqtt_client.h
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
#pragma once

#include "settings.h"
#include "rtc.h"

#include "wifi_priv.h"

#define MQTT_HOST_LEN 64
#define MQTT_LOGIN_LEN 32
#define MQTT_PASSWORD_LEN 32
#define MQTT_TOPIC_LEN 64
#define MQTT_ID_LEN 32

#define DEFAULT_MQTT_HOST   "192.168.88.12"
#define DEFAULT_MQTT_TOPIC  "revai/sensors/{i}/{t}"
#define DEFAULT_MQTT_PORT   1883
#define DEFAULT_MQTT_LOGIN  PRIVATE_MQTT_LOGIN
#define DEFAULT_MQTT_PASS   PRIVATE_MQTT_PASS
#define DEFAULT_MQTT_ID     "ESP{m}"

#define MQTT_TIMEOUT        2000

class MqttClient {
public:
    static MqttClient& instace() {
        static MqttClient instace;
        return instace;
    }

    void begin();
    bool connect(const uint32_t timeout=MQTT_TIMEOUT);

    void sendMeasurement(float temperature, float humidity, float pressure);
    void sendStatus(uint16_t voltage);
    void loop();


private:
    MqttClient();
    MqttClient(const MqttClient&);
    MqttClient& operator = (const MqttClient&);


    struct Config {
        char host[MQTT_HOST_LEN];
        uint16_t port;
        char login[MQTT_LOGIN_LEN];
        char pass[MQTT_PASSWORD_LEN];
        char topic[MQTT_TOPIC_LEN];
        char id[MQTT_ID_LEN];

        Config(const char *_host = DEFAULT_MQTT_HOST, uint16_t _port = DEFAULT_MQTT_PORT, const char *_login = DEFAULT_MQTT_LOGIN, 
                const char *_pass = DEFAULT_MQTT_PASS, const char *_topic = DEFAULT_MQTT_TOPIC, const char *_id = DEFAULT_MQTT_ID): port(_port) {
            strncpy(host, _host, sizeof(host));
            strncpy(login, _login, sizeof(login));
            strncpy(pass, _pass, sizeof(pass));
            strncpy(topic, _topic, sizeof(topic));
            strncpy(id, _id, sizeof(id));
        }
    };

    RtcMemory& rtc;
    NvsSettings& settings;
    NvsValue<Config> config;

    char clientId[32];

    int8_t getRssiQuality(const int32_t rssi) const;
    static void callback(char* topic, uint8_t* payload, uint16_t length);
};
