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

#include <ArduinoJson.h>
#include <WifiManager.h>

constexpr size_t paramCount = 13;
constexpr size_t configSize = JSON_OBJECT_SIZE(paramCount) + 437;

class NvsSettings {
public:
    static NvsSettings& instance() {
        static NvsSettings instance;
        return instance;
    }

    int getLastError() const {
        return lastError;
    }

    bool loadConfig(bool force = false);
    bool loadConfig(const JsonDocument& doc, bool force = false);
    
    void loadDefaults();
   
    void saveConfig() const;

    void addParameter(WiFiManager& wm);
    void saveParameter(WiFiManager& wm);
    
    bool isOpen() { return opened; }
    
    // config access by reference
	struct Config {
		uint32_t fingerprint;
		String wifi_ssid;
		String wifi_pass;
		uint32_t wifi_timeout;
		String mqtt_host;
		uint32_t mqtt_port;
		String mqtt_login;
		String mqtt_pass;
		String mqtt_topic;
		String mqtt_id;
		uint32_t mqtt_timeout;
		bool system_led;
		uint32_t system_sleep;
	};

    const Config& config() const { return _config; }

private:
    NvsSettings();
    NvsSettings(const NvsSettings &);
    NvsSettings& operator = (const NvsSettings&);

    void serialize(const JsonDocument& doc) const;

    bool opened;
	int lastError;
	
	Config _config;
};
