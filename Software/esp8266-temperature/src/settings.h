/**
 * @file settings.h
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
#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "wifi.h"

#define SETUP_TIME_SEC  120
#define ESP_WIFI_TIMEOUT 15000
#define MQTT_TIMEOUT 2000

#define DEFAULT_WIFI_SSID   PRIVATE_WIFI_SSID
#define DEFAULT_WIFI_PASS   PRIVATE_WIFI_PASS

#define DEFAULT_MQTT_HOST   "192.168.88.12"
#define DEFAULT_MQTT_TOPIC  "revai/sensors/%s/%s"
#define DEFAULT_MQTT_PORT   1883
#define DEFAULT_MQTT_LOGIN  PRIVATE_MQTT_LOGIN
#define DEFAULT_MQTT_PASS   PRIVATE_MQTT_PASS
#define DEFAULT_MQTT_ID     "ESP%04X"

#define DEFAULT_ESP_SLEEP   (30*60)

#define WIFI_SSID_LEN 33
#define WIFI_PWD_LEN 64

#define MQTT_HOST_LEN 64
#define MQTT_LOGIN_LEN 32
#define MQTT_PASSWORD_LEN 32
#define MQTT_TOPIC_LEN 64
#define MQTT_ID_LEN 32




class Settings {
public:
    Settings &instance() {
        static Settings instance;
        return instance;
    }

    bool loadConfig(settings_t &setting);
    bool saveConfig(const settings_t &setting);

private:
    typedef struct __packed {
        uint32_t crc;

        struct data_t {
            uint32 fingerprint;
            char wifi_ssid[WIFI_SSID_LEN];
            char wifi_pwd[WIFI_PWD_LEN];

            char mqtt_host[MQTT_HOST_LEN];
            uint16_t mqtt_port;
            char mqtt_login[MQTT_LOGIN_LEN];
            char mqtt_password[MQTT_PASSWORD_LEN];
            char mqtt_topic[MQTT_TOPIC_LEN];
            char mqtt_id[MQTT_ID_LEN];
            uint32_t sleep_time;
        } __packed data;
    } settings_t;

    settings_t setting;

    Settings() { }
    Settings(const Settings&);
    Settings& operator = (const Settings&);
};

#endif