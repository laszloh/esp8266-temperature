/**
 * @file settings.cpp
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

#include <Arduino.h>
#include <ArduinoLog.h>
#include <FS.h>
#include <LittleFS.h>

#include "settings.h"
#include "setup.h"

#define LOG_AS "[FLASH] "

static constexpr const char *filename = "config.json";

NvsSettings::NvsSettings(): opened(false), lastError(0) {
	LittleFS.begin();
	loadConfig();
}

bool NvsSettings::loadConfig(bool force) {
    if(opened && !force) {
        Log.verbose(F(LOG_AS "Settings are already loaded, ignoring..."));
        lastError = 0;
        return false;
    }
    
	File cfgData = LittleFS.open(filename);
	if(!cfgData) {
		lastError = -1;
        return false;
	} else {
		StaticJsonDocument<configSize> doc;

		// Deserialize the JSON document
		DeserializationError error = deserializeJson(doc, cfgData);
		if (error)
			lastError = -1;

        loadConfig(doc);

		// Close the file (Curiously, File's destructor doesn't close the file)
		file.close();
	}
    return true;
}

bool NvsSettings::loadConfig(const JsonDocument& doc, bool force) {
    uint32_t fingerprint = doc["fingerprint"] | 0;
    bool newer = fingerprint > config.fingerprint;
    
    if(opened && !force && !newer) {
        Log.verbose(F(LOG_AS "Settings are already loaded, ignoring..."));
        lastError = 0;
        return false;
    }
    Log.verbose(F(LOG_AS "Loading file due to: forced: %B, newer: %B"), forced, newer);

    // Copy values from the JsonDocument to the Config
    config.fingerprint = fingerprint;
    
    // read the wifi settings
    config.wifi_ssid = String(doc["wifi-ssid"] | DEFAULT_WIFI_SSID);
    config.wifi_pass = String(doc["wifi-pass"] | DEFAULT_WIFI_PASS);
    config.wifi_timeout = doc["wifi-timeout"] | DEFAULT_WIFI_TIMEOUT;
    
    // read the mqtt settings
    config.mqtt_host = String(doc["mqtt-host"] | DEFAULT_MQTT_HOST);
    config.mqtt_port = doc["mqtt-port"] | DEFAULT_MQTT_PORT;
    config.mqtt_login = String(doc["mqtt-login"] | DEFAULT_MQTT_LOGIN);
    config.mqtt_pass = String(doc["mqtt-pass"] | DEFAULT_MQTT_PASS);
    config.mqtt_topic = String(doc["mqtt-topic"] | DEFAULT_MQTT_TOPIC);
    config.mqtt_id = String(doc["mqtt-id"] | DEFAULT_MQTT_ID);
    config.mqtt_timeout = doc["mqtt-port"] | DEFAULT_MQTT_TIMEOUT;

    // read the system settings
    config.system_led = doc["system-led"] | true;
    config.system_sleep = doc["system-sleep"] | DEFAULT_SYSTEM_SLEEP;
    
    return true;
}

void NvsSettings::saveConfig() const {
    StaticJsonDocument<configSize> doc;
    
	File file = LittleFS.open(filename, FILE_WRITE);
    if (!file) {
        Log.error(F(LOG_AS "Failed to open config file for writing"));
        return;
    }

    // serialze the settings fingerprint
    doc["fingerprint"] = config.fingerprint;

    // serialze the wifi config
    doc["wifi-ssid"] = config.wifi_ssid;
    doc["wifi-pass"] = config.wifi_pass;
    doc["wifi-timeout"] = config.wifi_timeout;
    
    // serialze the mqtt config
    doc["mqtt-host"] = config.mqtt_host;
    doc["mqtt-port"] = config.mqtt_port;
    doc["mqtt-login"] = config.mqtt_login;
    doc["mqtt-pass"] = config.mqtt_pass;
    doc["mqtt-topic"] = config.mqtt_topic;
    doc["mqtt-id"] = config.mqtt_id;
    doc["mqtt-port"] = config.mqtt_timeout;

    // serialze the system config
    doc["system-led"] = config.system_led;
    doc["system-sleep"] = config.system_sleep;
    
    // Serialize JSON to file
    if (serializeJson(doc, file) == 0) {
        Log.error(F(LOG_AS "Failed to write to config file"));
    }

    // Close the file
    file.close();
}

