/**
 * @file ota.cpp
 * @author Laszlo Hegedüs (laszlo.hegedues@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-10-09
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
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ArduinoLog.h>

#include "ota.h"

#define LOG_AS  "[OTA] "

void OtaWorker::begin() {
    ArduinoOTA.setRebootOnSuccess(true);

    ArduinoOTA.onStart([]() {
        Log.notice(F(LOG_AS "Starting updateing %s" CR), (ArduinoOTA.getCommand() == U_FLASH) ? F("flash") : F("filesystem") );
    });
    ArduinoOTA.onEnd([]() {
        Log.notice(F(LOG_AS "Upgrade End" CR));
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        unsigned int percent = (progress / (total / 100));
        Log.notice(F(LOG_AS "Progress: %d%%" CR), percent);
    });
    ArduinoOTA.onError([](ota_error_t err) {
        Log.error(F(LOG_AS "OTA failed Error: %d" CR), err);
        switch(err) {
            case OTA_AUTH_ERROR:
                Log.error(F(LOG_AS "Auth failed" CR));
                break;
            case OTA_BEGIN_ERROR:
                Log.error(F(LOG_AS "Begin failed" CR));
                break;
            case OTA_CONNECT_ERROR:
                Log.error(F(LOG_AS "Connect failed" CR));
                break;
            case OTA_RECEIVE_ERROR:
                Log.error(F(LOG_AS "Receive failed" CR));
                break;
            case OTA_END_ERROR:
                Log.error(F(LOG_AS "End failed" CR));
                break;
            default:
                Log.error(F(LOG_AS "Unknown error" CR));
                break;
        }
    });
    ArduinoOTA.begin();

    Log.notice(F(LOG_AS "OTA Update started" CR));
    Log.notice(F(LOG_AS "Local IP: %s" CR), WiFi.localIP().toString().c_str());
}

void OtaWorker::handle() {
    ArduinoOTA.handle();
}
