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
#include <ArduinoJson.h>

#include "rtc.h"
#include "settings.h"
#include "wifi_priv.h"

#define WIFI_SSID_LEN 33
#define WIFI_PWD_LEN 64

#define DEFAULT_WIFI_SSID       PRIVATE_WIFI_SSID
#define DEFAULT_WIFI_PASS       PRIVATE_WIFI_PASS
#define DEFAULT_WIFI_TIMEOUT    15000

class WifiModule {
public:
    static WifiModule& instance() {
        static WifiModule instance;
        return instance;
    }

    void begin() {
        if(rtc.isRtcValid()) {
            Log.notice(F("[WiFi] " "RTC config: found" CR));
            WiFi.forceSleepWake();
            WiFi.config(rtc.getIp(), rtc.getGateway(), rtc.getMask(), rtc.getDns());
            uint8_t bssid[6];
            rtc.getBssid(bssid);
            WiFi.begin(ssid, pass, rtc.getChannel(), bssid);
        } else {
            // no data in rtc, normal startup
            Log.notice(F("[WiFi] " "RTC config: NOT found" CR));
            WiFi.forceSleepWake();
            WiFi.begin(ssid, pass);
        }
    }

    bool isConnected() const {
        return (WiFi.status() != WL_CONNECTED);
    }

    bool connect() {
        uint32_t curMillis = millis();

        while(!isConnected()) {
            if(millis() - curMillis > timeout.get()) 
                return false;
            delay(10);
        }
        return true;
    }

	void updateSettings(const JsonDocument &jsonDoc) {
        ssid = jsonDoc["wifi-ssid"].as<const char*>();
        pass = jsonDoc["wifi-pass"].as<const char*>();
        timeout = jsonDoc["wifi-timeout"].as<uint32_t>();
    }

private:
    WifiModule(): 
        rtc(RtcMemory::instance()), 
        settings(NvsSettings::instance()),
        ssid(DEFAULT_WIFI_SSID, settings, "wifi-ssid"),
        pass(DEFAULT_WIFI_PASS, settings, "wifi-pass"),
        timeout(DEFAULT_WIFI_TIMEOUT, settings, "wifi-timeout"),
        pTimeout(timeout.getID(), "Wifi Timeout", timeout, true)
        {
            wm.addParameter(&pTimeout);
        }
    WifiModule(const WifiModule &);
    WifiModule & operator = (const WifiModule&);

    RtcMemory& rtc;
    NvsSettings& settings;
    NvsValue< StrValue<WIFI_SSID_LEN> > ssid;
    NvsValue< StrValue<WIFI_PWD_LEN> > pass;
    NvsValue< IntValue<uint32_t> > timeout;

    class Parameter : public WiFiManagerParameter {
    public:
        Parameter(const char *id, const char *label, const char *defaultValue, bool integer, int length = 10) : 
            WiFiManagerParameter(id, label, defaultValue, length) {
            if(integer)
                init(id, label, defaultValue, length, " type='number'", WFM_LABEL_BEFORE);
        }

        void saveParameter() override {
            WifiModule& w = WifiModule::instance();
            if(strcmp(w.timeout.getID(), getID()) == 0) {
                w.timeout = atoi(getValue());
            }
            w.ssid = wm.getWiFiSSID().c_str();
            w.pass = wm.getWiFiPass().c_str();
        }
    };

    Parameter pTimeout;

};
