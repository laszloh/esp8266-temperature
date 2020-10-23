/**
 * @file settings.h
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
#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "rtc.h"
#include "settings.h"
#include "wifi_priv.h"

#define WIFI_SSID_LEN 33
#define WIFI_PWD_LEN 64

#define DEFAULT_WIFI_SSID       PRIVATE_WIFI_SSID
#define DEFAULT_WIFI_PASS       PRIVATE_WIFI_PASS
#define DEFAULT_WIFI_TIMEOUT    15000

class WiFiModule {
public:
    static WiFiModule& instance() {
        static WiFiModule instance;
        return instance;
    }
	
	bool config() {
		return (!config.getDefaultValue());
	}

    void begin() {
        
        if(rtc.isRtcValid()) {
            Log.notice(F("[WiFi] " "RTC config: found" CR));
            WiFi.forceSleepWake();
            WiFi.config(rtc.getIp(), rtc.getGateway(), rtc.getMask(), rtc.getDns());
            uint8_t bssid[6];
            rtc.getBssid(bssid);
            WiFi.begin(c.ssid, c.pwd, rtc.getChannel(), bssid);
        } else {
            // no data in rtc, normal startup
            Log.notice(F("[WiFi] " "RTC config: NOT found" CR));
            WiFi.forceSleepWake();
            WiFi.begin(c.ssid, c.pwd);
        }
    }

    bool isConnected() const {
        return (WiFi.status() != WL_CONNECTED);
    }

    bool connect() {
        uint32_t curMillis = millis();
        Config c(config);

        while(!isConnected()) {
            if(millis() - curMillis > c.timeout) 
                return false;
            delay(10);
        }
        return true;
    }

    void configure(const char *_ssid = nullptr, const char *_pass = nullptr) {
        Config c(config);
        if(_ssid != nullptr)
            strncpy(c.ssid, _ssid, sizeof(c.ssid));
        if(_pass != nullptr)
            strncpy(c.pwd, _pass, sizeof(c.pwd));
        config = c;
    }

private:
    WiFiModule(): 
        rtc(RtcMemory::instance()), 
        settings(NvsSettings::instance()),
        config(Config(), settings, "wifi") { }
    WiFiModule(const WiFiModule &);
    WiFiModule & operator = (const WiFiModule&);

    struct Config {
        char ssid[WIFI_SSID_LEN];
        char pwd[WIFI_PWD_LEN];
        uint32_t timeout;

        Config(const char *_ssid = DEFAULT_WIFI_SSID, const char *_pass = DEFAULT_WIFI_PASS, 
                uint32_t _timeout = DEFAULT_WIFI_TIMEOUT) : timeout(_timeout) {
            strncpy(ssid, _ssid, sizeof(ssid));
            strncpy(pwd, _pass, sizeof(pwd));
        }
    };

    RtcMemory& rtc;
    NvsSettings& settings;
    NvsValue<Config> config;
};
