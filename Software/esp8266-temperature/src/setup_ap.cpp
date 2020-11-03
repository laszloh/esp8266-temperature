/**
 * @file setup_ap.cpp
 * @author Laszlo Hegedüs (laszlo.hegedues@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-10-01
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
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WifiManager.h>
#include <ArduinoLog.h>

#include "setup_ap.h"
#include "settings.h"

#define LOG_AS  "[AP] "

#define SETUP_TIME_SEC 180

static WiFiManager wm;

bool setup_ap() {
    bool result;
    NvsSettings& settings = NvsSettings::instance();

#ifndef DISABLE_LOGGING
    wm.debugPlatformInfo();
#endif

    Log.notice(F(LOG_AS "Entering captive portal" CR));

    settings.addParameter(wm);

    wm.setConfigPortalTimeout(SETUP_TIME_SEC);
    wm.setConnectTimeout(NvsSettings::instance().config().wifi_timeout);

    result = wm.startConfigPortal();

    if(result) {
        // we are connected
        Log.notice(F(LOG_AS "Connected to wifi. Saving settings and restarting" CR));
        WiFi.preinitWiFiOff();

        settings.saveParameter(wm);

        Log.notice(F(LOG_AS "Resetting ESP..." CR));
        Serial.flush();
        ESP.reset();
    }
    return result;

}