/**
 * @file setup_ap.cpp
 * @author Laszlo Hegedüs (laszlo.hegedues@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-10-01
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
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WifiManager.h>
#include <ArduinoLog.h>

#include "setup_ap.h"

#define LOG_AS  "[AP] "

WiFiManager wm;

bool setup_ap() {
    bool result;

#ifndef DISABLE_LOGGING
    wm.debugPlatformInfo();
#endif

#if 0
    Log.notice(F(LOG_AS "Entering captive portal" CR));

    std::vector<const char*> menu = {"wifi","info","param","update","sep","restart","exit"};
    wm.setMenu(menu);

    // add additional parameters
    WiFiManagerParameter param_mqtt_host("mhost", "MQTT Host", setting.data.mqtt_host, MQTT_HOST_LEN-1);
    wm.addParameter(&param_mqtt_host);
    LongParameter param_mqtt_port("mport", "MQTT Port", setting.data.mqtt_port);
    wm.addParameter(&param_mqtt_port);
    WiFiManagerParameter param_mqtt_login("mlogin", "MQTT Login", setting.data.mqtt_login, MQTT_LOGIN_LEN-1);
    wm.addParameter(&param_mqtt_login);
    WiFiManagerParameter param_mqtt_password("mpwd", "MQTT Password", setting.data.mqtt_password, MQTT_PASSWORD_LEN-1);
    wm.addParameter(&param_mqtt_password);
    WiFiManagerParameter param_mqtt_topic("mtopic", "MQTT Topic", setting.data.mqtt_topic, MQTT_TOPIC_LEN-1);
    wm.addParameter(&param_mqtt_topic);
    LongParameter param_sleep_time("sleeptime", "Sleep Time", setting.data.sleep_time);
    wm.addParameter(&param_sleep_time);

    wm.setConfigPortalTimeout(SETUP_TIME_SEC);
    wm.setConnectTimeout(ESP_WIFI_TIMEOUT);

    result = wm.startConfigPortal();

    if(result) {
        // we are connected
        Log.notice(F(LOG_AS "Connected to wifi. Saving setings and restarting" CR));
        WiFi.preinitWiFiOff();

        memcpy(setting.data.mqtt_host, param_mqtt_host.getValue(), MQTT_HOST_LEN);
        memcpy(setting.data.mqtt_login, param_mqtt_login.getValue(), MQTT_LOGIN_LEN);
        memcpy(setting.data.mqtt_password, param_mqtt_password.getValue(), MQTT_PASSWORD_LEN);
        memcpy(setting.data.mqtt_topic, param_mqtt_topic.getValue(), MQTT_TOPIC_LEN);
        setting.data.mqtt_port = param_mqtt_port.getValue();
        setting.data.sleep_time = param_sleep_time.getValue();

        memcpy(setting.data.wifi_ssid, wm.getWiFiSSID().c_str(), WIFI_SSID_LEN);
        memcpy(setting.data.wifi_pwd, wm.getWiFiPass().c_str(), WIFI_PWD_LEN);

        Log.verbose(F(LOG_AS "New Settings:" CR));
        Log.verbose(F(LOG_AS "    MQTT Host:  %s" CR), setting.data.mqtt_host);
        Log.verbose(F(LOG_AS "    MQTT Port:  %d" CR), setting.data.mqtt_port);
        Log.verbose(F(LOG_AS "    MQTT Login: %s" CR), setting.data.mqtt_login);
        Log.verbose(F(LOG_AS "    MQTT Pass:  %s" CR), setting.data.mqtt_password);
        Log.verbose(F(LOG_AS "    MQTT Topic: %s" CR), setting.data.mqtt_topic);
        Log.verbose(F(LOG_AS "    Sleep:      %d" CR), setting.data.sleep_time);

        if(saveConfig(setting))
            Log.notice(F(LOG_AS "Settings save: success" CR));
        else
            Log.error(F(LOG_AS "Settings save: FAILED" CR));
        Log.notice(F(LOG_AS "Resetting ESP..." CR));
        Serial.flush();
        ESP.reset();
    }
    return result;

#endif
}