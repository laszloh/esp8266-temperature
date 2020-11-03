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

static constexpr const char *filename = "/config.json";

NvsSettings::NvsSettings(): opened(false), lastError(0) {
    memset(&_config, 0, sizeof(Config));

	LittleFS.begin();
    loadConfig();
}

bool NvsSettings::loadConfig(bool force) {
    StaticJsonDocument<configSize> doc;
    
    if(opened && !force) {
        Log.verbose(F(LOG_AS "Settings are already loaded, ignoring..." CR));
        lastError = 0;
        return false;
    }
    
	File file = LittleFS.open(filename, "r");
    if(!file) {
        Log.warning(F(LOG_AS "Config file not found. Loading default settings..." CR));
        loadDefaults();
    } else {
        Log.notice(F(LOG_AS "Loading config from file %s" CR), filename);

        // Deserialize the JSON document
        DeserializationError error = deserializeJson(doc, file);
        if (error) {
            Log.error(F(LOG_AS "Error Deserializing: %s" CR), error.c_str());
            lastError = -1;
        }
        loadConfig(doc, force);

        // Close the file (Curiously, File's destructor doesn't close the file)
        file.close();
    }
    return true;
}

void NvsSettings::loadDefaults() {
    // overwrite config and save to backend
    
    // reset to default settings
    _config.fingerprint = 0;
    
    // read the wifi settings
    _config.wifi_ssid = DEFAULT_WIFI_SSID;
    _config.wifi_pass = DEFAULT_WIFI_PASS;
    _config.wifi_timeout = DEFAULT_WIFI_TIMEOUT;
    
    // read the mqtt settings
    _config.mqtt_host = DEFAULT_MQTT_HOST;
    _config.mqtt_port = DEFAULT_MQTT_PORT;
    _config.mqtt_login = DEFAULT_MQTT_LOGIN;
    _config.mqtt_pass = DEFAULT_MQTT_PASS;
    _config.mqtt_topic = DEFAULT_MQTT_TOPIC;
    _config.mqtt_id = DEFAULT_MQTT_ID;
    _config.mqtt_timeout = DEFAULT_MQTT_TIMEOUT;

    // read the system settings
    _config.system_led = true;
    _config.system_sleep = DEFAULT_SYSTEM_SLEEP;
    
    opened = true;
    saveConfig();
}

template <typename T>
inline void write_validiate(T& dst, const T& src) {
    if(src && src != dst)
        dst = src;
}

bool NvsSettings::loadConfig(const JsonDocument& doc, bool force) {
    uint32_t fingerprint = doc["fingerprint"] | 0;
    bool newer = fingerprint > _config.fingerprint;
    
    Log.verbose(F(LOG_AS "JSON:" CR));
    Log.verbose(F(LOG_AS "---------" CR));
    String x;
    serializeJsonPretty(doc, x);
    Log.verbose(F(LOG_AS "%s" CR), x.c_str());
    Log.verbose(F(LOG_AS "---------" CR));

    if(opened && !force && !newer) {
        Log.verbose(F(LOG_AS "Settings are already loaded, ignoring..." CR));
        lastError = 0;
        return false;
    }
    Log.verbose(F(LOG_AS "Loading file due to: forced: %T, newer: %T" CR), force, newer);

    // Copy values from the JsonDocument to the Config
    _config.fingerprint = fingerprint;
    
    // read the wifi settings
    write_validiate(_config.wifi_ssid, doc["wifi-ssid"].as<String>());
    write_validiate(_config.wifi_pass, doc["wifi-pass"].as<String>());
    write_validiate(_config.wifi_timeout, doc["wifi-timeout"].as<uint32_t>());

    // read the mqtt settings
    write_validiate(_config.mqtt_host, doc["mqtt-host"].as<String>());
    write_validiate(_config.mqtt_port, doc["mqtt-port"].as<uint16_t>());
    write_validiate(_config.mqtt_login, doc["mqtt-login"].as<String>());
    write_validiate(_config.mqtt_pass, doc["mqtt-pass"].as<String>());
    write_validiate(_config.mqtt_topic, doc["mqtt-topic"].as<String>());
    write_validiate(_config.mqtt_id, doc["mqtt-id"].as<String>());
    write_validiate(_config.mqtt_timeout, doc["mqtt-timeout"].as<uint32_t>());

    // read the system settings
    _config.system_led = doc["system-led"] | false;
    write_validiate(_config.system_sleep, doc["system-sleep"].as<uint32_t>());
    
#if 0
    const char *ssid = doc["wifi-ssid"];
    if(ssid && _config.wifi_ssid != ssid)
        _config.wifi_ssid = ssid;
    const char *wifi_pass = doc["wifi-pass"];
    if(wifi_pass && _config.wifi_pass != wifi_pass)
        _config.wifi_pass = wifi_pass;
    uint32_t timeout = doc["wifi-timeout"];
    if(timeout && timeout != _config.wifi_timeout)
        _config.wifi_timeout = timeout;
    
    // read the mqtt settings
    const char *mqtt_host = doc["mqtt-host"];
    if(mqtt_host && _config.mqtt_host != mqtt_host)
        _config.mqtt_host = mqtt_host;
    uint16_t port = doc["mqtt-port"];
    if(port && port != _config.mqtt_port)
        _config.mqtt_port = port;
    const char *mqtt_login = doc["mqtt-login"];
    if(mqtt_login && _config.mqtt_login != mqtt_login)
        _config.mqtt_login = mqtt_login;
    const char *mqtt_pass = doc["mqtt-pass"];
    if(mqtt_pass && _config.mqtt_pass != mqtt_pass)
        _config.mqtt_pass = mqtt_pass;
    const char *mqtt_topic = doc["mqtt-topic"];
    if(mqtt_topic && _config.mqtt_topic != mqtt_topic)
        _config.mqtt_topic = mqtt_topic;
    const char *mqtt_id = doc["mqtt-id"];
    if(mqtt_id && _config.mqtt_id != mqtt_id)
        _config.mqtt_id = mqtt_id;
    timeout = doc["mqtt-timeout"];
    if(timeout && timeout != _config.wifi_timeout)
        _config.mqtt_timeout = timeout;

    // read the system settings
    _config.system_led = doc["system-led"] | false;
    uint32_t sleep = doc["system-sleep"];
    if(sleep && sleep != _config.system_sleep)
        _config.system_sleep = sleep;
#endif

    opened = true;
    
    return true;
}

void NvsSettings::saveConfig() const {
    StaticJsonDocument<configSize> doc;
    
    // serialze the settings fingerprint
    doc["fingerprint"] = _config.fingerprint;

    // serialze the wifi config
    doc["wifi-ssid"] = _config.wifi_ssid;
    doc["wifi-pass"] = _config.wifi_pass;
    doc["wifi-timeout"] = _config.wifi_timeout;
    
    // serialze the mqtt config
    doc["mqtt-host"] = _config.mqtt_host;
    doc["mqtt-port"] = _config.mqtt_port;
    doc["mqtt-login"] = _config.mqtt_login;
    doc["mqtt-pass"] = _config.mqtt_pass;
    doc["mqtt-topic"] = _config.mqtt_topic;
    doc["mqtt-id"] = _config.mqtt_id;
    doc["mqtt-timeout"] = _config.mqtt_timeout;

    // serialze the system config
    doc["system-led"] = _config.system_led;
    doc["system-sleep"] = _config.system_sleep;

    serialize(doc);
}

void NvsSettings::serialize(const JsonDocument& doc) const {
	size_t x;
    File file = LittleFS.open(filename, "w");
    if (!file) {
        Log.error(F(LOG_AS "Failed to open config file for writing" CR));
        return;
    }

    // Serialize JSON to file
    if ( (x = serializeJson(doc, file)) == 0) {
        Log.error(F(LOG_AS "Failed to write to config file" CR));
    }
    Log.verbose(F(LOG_AS "Wrote: %d" CR), x);

    // Close the file
    file.flush();
    file.close();
}

void NvsSettings::addParameter(WiFiManager& wm) {
    WiFiManagerParameter *param; 

    // wifi parameter
    param = new WiFiManagerParameter("wifi-timeout", "Wifi Timeout", String(_config.wifi_timeout).c_str(), 10);
    param->type() = WiFiManagerParameter::IntegerType;
    wm.addParameter(param);

    // MQTT parameter
    param = new WiFiManagerParameter("<h1>MQTT</h1>");
    wm.addParameter(param);
    param = new WiFiManagerParameter("mqtt-host", "Host", _config.mqtt_host.c_str(), 40);
    param->type() = WiFiManagerParameter::StringType;
    wm.addParameter(param);
    param = new WiFiManagerParameter("mqtt-port", "Port", String(_config.mqtt_port).c_str(), 10);
    param->type() = WiFiManagerParameter::IntegerType;
    wm.addParameter(param);
    param = new WiFiManagerParameter("mqtt-login", "Login", _config.mqtt_login.c_str(), 32);
    param->type() = WiFiManagerParameter::StringType;
    wm.addParameter(param);
    param = new WiFiManagerParameter("mqtt-pass", "Passwort", _config.mqtt_pass.c_str(), 32);
    param->type() = WiFiManagerParameter::StringType;
    wm.addParameter(param);
    param = new WiFiManagerParameter("mqtt-topic", "Topic", _config.mqtt_topic.c_str(), 32);
    param->type() = WiFiManagerParameter::StringType;
    wm.addParameter(param);
    param = new WiFiManagerParameter("mqtt-id", "Chip ID", _config.mqtt_id.c_str(), 32);
    param->type() = WiFiManagerParameter::StringType;
    wm.addParameter(param);
    param = new WiFiManagerParameter("mqtt-timeout", "Timeout", String(_config.mqtt_timeout).c_str(), 10);
    param->type() = WiFiManagerParameter::IntegerType;
    wm.addParameter(param);

    // System Parameter
    param = new WiFiManagerParameter("<h1>System</h1>");
    wm.addParameter(param);
    param = new WiFiManagerParameter("system-led", "LED", String(_config.system_led).c_str(), 10);
    param->type() = WiFiManagerParameter::IntegerType;
    wm.addParameter(param);
    param = new WiFiManagerParameter("system-sleep", "Sleep", String(_config.system_sleep).c_str(), 10);
    param->type() = WiFiManagerParameter::IntegerType;
    wm.addParameter(param);
}

void NvsSettings::saveParameter(WiFiManager& wm) {
    StaticJsonDocument<configSize> doc;

    WiFiManagerParameter **params = wm.getParameters();
    int count = wm.getParametersCount();

    _config.fingerprint++;
    doc["fingerprint"] = _config.fingerprint;
    Log.notice(F(LOG_AS "Saving parameters..." CR));
    Log.verbose(F(LOG_AS "----------" CR));

    doc["wifi-ssid"] = wm.getWiFiSSID();
    doc["wifi-pass"] = wm.getWiFiPass();

    for(int i=0;i<count;i++) {
        WiFiManagerParameter *p = params[i];
        switch(p->type()) {
            case WiFiManagerParameter::IntegerType:
                doc[p->getID()] = atoi(p->getValue());
                break;

            case WiFiManagerParameter::StringType:
                doc[p->getID()] = p->getValue();
                break;

            case WiFiManagerParameter::FloatType:
            break;
        }
        Log.verbose(F(LOG_AS "Saving parameter %s: %s" CR), p->getID(), p->getValue());
    }
    Log.verbose(F(LOG_AS "----------" CR));

    Log.verbose(F(LOG_AS "JSON:" CR));
    Log.verbose(F(LOG_AS "---------" CR));
    String x;
    serializeJsonPretty(doc, x);
    Log.verbose(F(LOG_AS "%s" CR), x.c_str());
    Log.verbose(F(LOG_AS "---------" CR));

    serialize(doc);
}
