/**
 * @file mqtt_client.h
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
#pragma once

#include <ArduinoJson.h>

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

#define DEFAULT_MQTT_TIMEOUT	2000

constexpr unsigned int str2int(const char* str, int h = 0)
{
    return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

class MqttClient {
public:
    static MqttClient& instace() {
        static MqttClient instace;
        return instace;
    }
	
	void updateSettings(const JsonDocument &jsonDoc);
	
    void begin();
    bool connect();

    void sendMeasurement(float temperature, float humidity, float pressure);
    void sendStatus(uint16_t voltage);
    void loop();


private:
    MqttClient();
    MqttClient(const MqttClient&);
    MqttClient& operator = (const MqttClient&);

    RtcMemory& rtc;
    NvsSettings& settings;
	
	NvsValue<char[MQTT_HOST_LEN]> host;
	NvsValue<uint16_t> port;
	NvsValue<char[MQTT_LOGIN_LEN]> login;
	NvsValue<char[MQTT_PASSWORD_LEN]> pass;
	NvsValue<char[MQTT_TOPIC_LEN]> topic;
	NvsValue<char[MQTT_ID_LEN]> id;
	NvsValue<uint32_t> timeout;

    char clientId[32];

    int8_t getRssiQuality(const int32_t rssi) const;
    static void callback(char* topic, uint8_t* payload, uint16_t length);

    class Parameter : public WiFiManagerParameter {
    public:
        Parameter(const char *id, const char *label, const char *defaultValue, bool integer = false, int length = 10) : 
            WiFiManagerParameter(id, label, defaultValue, length) {
            if(integer)
                init(id, label, defaultValue, length, " type='number'", WFM_LABEL_BEFORE);
        }

        void saveParameter() override {
            MqttClient& c = MqttClient::instace();

            if(strcmp(c.host.getID(), getID()) == 0) {
                c.host = getValue();
            } else if(strcmp(c.port.getID(), getID()) == 0) {
                c.port = atoi(getValue());
            } else if(strcmp(c.login.getID(), getID()) == 0) {
                c.login = getValue();
            } else if(strcmp(c.pass.getID(), getID()) == 0) {
                c.pass = getValue();
            } else if(strcmp(c.topic.getID(), getID()) == 0) {
                c.topic = getValue();
            } else if(strcmp(c.id.getID(), getID()) == 0) {
                c.id = getValue();
            } else if(strcmp(c.timeout.getID(), getID()) == 0) {
                c.timeout = atoi(getValue());                
            }
        }
    };

    Parameter pHost;
    Parameter pPort;
    Parameter pLogin;
    Parameter pPass;
    Parameter pTopic;
    Parameter pId;
    Parameter pTimeout;
};

