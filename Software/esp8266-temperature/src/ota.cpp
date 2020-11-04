/**
 * @file ota.cpp
 * @author Laszlo Hegedüs (laszlo.hegedues@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-11-04
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
#include <Arduino.h>
#include <ArduinoLog.h>
#include <WiFiClient.h>
#include <ESP8266httpUpdate.h>

#include "ota.h"

#ifndef VERSION_MAJOR
    #define VERSION_MAJOR 0
#endif

#ifndef VERSION_MINOR
    #define VERSION_MINOR 0
#endif

#define LOG_AS "[OTA] "


constexpr uint16_t majorVersion = VERSION_MAJOR;
constexpr uint16_t minorVersion = VERSION_MINOR;

constexpr const char* versionString = "ESP-TEMP_v" VERSION_MAJOR "_" VERSION_MINOR;

Ota::Ota() {
}

bool Ota::checkVersion(uint16_t major, uint16_t minor) const {
    return ( major > majorVersion || minor > minorVersion );
}

const char* Ota::toString() const {
    return versionString;
}

bool Ota::upgrade(uint16_t major, uint16_t minor, const char* folderUrl, bool force) {
    if(!checkVersion(major, minor) && !force) {
        Log.notice(F(LOG_AS "Current or newer firmware already installed" CR));
        return false;
    }
    Log.verbose(F(LOG_AS "Upgrading due to newer=%T forced=%T" CR), checkVersion(major, minor), forced);
    Log.notice(F(LOG_AS "Installing new version v%d.%d" CR), major, minor);
    
    WiFiClient client;
    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
    
    t_httpUpdate_return ret = ESPhttpUpdate.updateFS(client, String(folderUrl).concat("/littleFs.bin"));
    Log.notice(F(LOG_AS "Updating filesystem: %s" CR), (ret==HTTP_UPDATE_OK) ? "Done" : "Failed");
    switch (ret) {
        case HTTP_UPDATE_FAILED:
            Log.error(F(LOG_AS "HTTP update failed Error %d: %s" CR), ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            break;

        case HTTP_UPDATE_NO_UPDATES:
            Log.notice(F(LOG_AS "No updated avaliable" CR);
            break;
    }


    if (ret == HTTP_UPDATE_OK || ESPhttpUpdate.getLastError() ==  HTTP_UE_SERVER_FILE_NOT_FOUND) {
        ret = ESPhttpUpdate.update(client, String(folderUrl).concat("/firmware.bin"));
        Log.notice(F(LOG_AS "Updating firmware: %s" CR), (ret==HTTP_UPDATE_OK) ? "Done" : "Failed");

        switch (ret) {
            case HTTP_UPDATE_FAILED:
                Log.error(F(LOG_AS "HTTP update failed Error %d: %s" CR), ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                break;

            case HTTP_UPDATE_NO_UPDATES:
                Log.notice(F(LOG_AS "No updated avaliable" CR);
                break;
        }
    }
    return (ret == HTTP_UPDATE_OK);
}
