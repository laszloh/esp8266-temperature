/**
 * @file settings.cpp
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

#include <Arduino.h>

#include <ArduinoLog.h>
#include <CRC32.h>
#include <EEPROM.h>

#include "settings.h"

settings_t settings;

bool loadConfig(settings_t &setting) {
    bool ret = true;

    EEPROM.begin(sizeof(settings_t));
    EEPROM.get(0, setting);
    EEPROM.end();

    if(setting.version == SETTINGS_REVISION && 
        setting.crc == CRC32::calculate((uint8_t*)&setting.data, sizeof(setting.data))) {
        Log.notice(F("Loading settings: OK" CR));
    } else {
        Log.error(F("Loading settings: FAILED" CR));
        // set up the settings variable with default values
        memset(&setting, 0, sizeof(settings_t));

        setting.data.mqtt_port = 1883;
        strncpy(setting.data.mqtt_host, DEFAULT_MQTT_HOST, MQTT_HOST_LEN);
        strncpy(setting.data.mqtt_topic, DEFAULT_MQTT_TOPIC, MQTT_TOPIC_LEN);
    }

    uint32_t crc = settings.crc;
    settings.crc = 0x00;

    if(CRC32::calculate((uint8_t*)&settings, sizeof(settings_t)) == crc) {
    } else {
        // crc failed! load default settings
        ret = false;
    }
    return ret;
}

bool saveConfig(const settings_t &setting) {
    bool ret;
    settings_t store;

    EEPROM.begin(sizeof(settings_t));

    store.version = SETTINGS_REVISION;
    store.crc = CRC32::calculate((uint8_t*)&setting.data, sizeof(setting.data));
    store.data = setting.data;

    EEPROM.put(0, store);

    ret = EEPROM.commit();
    EEPROM.end();

    return ret;
}
