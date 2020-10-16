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
#include <FastCRC.h>
#include <EEPROM.h>

#include "settings.h"

#include "nvs_flash.h"
#include "nvs.h"
#include "user_interface.h"

#define LOG_AS "[FLASH] "
#define SETTINGS_REVISION 2

static FastCRC32 crc32;

extern "C" uint32_t _FS_start;
extern "C" uint32_t _FS_end;

static const uint32_t sector_start = ((uint32_t)&_FS_start - 0x40200000) / SPI_FLASH_SEC_SIZE;
static const uint32_t sector_count = (((uint32_t)&_FS_end - 0x40200000) / SPI_FLASH_SEC_SIZE) - sector_start;

static void printSettings(const settings_t &setting) {
    Log.verbose(F(LOG_AS "Flash Settings:" CR));
    Log.verbose(F(LOG_AS "    SSID:  %s" CR), setting.data.wifi_ssid);
    Log.verbose(F(LOG_AS "    PWD:   %s" CR), setting.data.wifi_pwd);
    Log.verbose(F(LOG_AS "    Host:  %s" CR), setting.data.mqtt_host);
    Log.verbose(F(LOG_AS "    Port:  %d" CR), setting.data.mqtt_port);
    Log.verbose(F(LOG_AS "    Login: %s" CR), setting.data.mqtt_login);
    Log.verbose(F(LOG_AS "    Pass:  %s" CR), setting.data.mqtt_password);
    Log.verbose(F(LOG_AS "    Topic: %s" CR), setting.data.mqtt_topic);
    Log.verbose(F(LOG_AS "    Sleep: %d" CR), setting.data.sleep_time);
}

esp_err_t nvs_flash_erase_partition(const char *part_name) {
    for(uint32_t i = 0;i<sector_count;i++) {
        auto err = spi_flash_erase_sector(sector_start + i);
        if(err != ESP_OK)
            return err;
    }
    return ESP_OK;
}

NvsSettings::NvsSettings(): opened(false), handle(0) {
    Log.verbose(F(LOG_AS "Opening Non-Volatile Storage (NVS) handle... " CR));
    lastError = nvs_flash_init_custom("nvs", sector_start, sector_count);
    if (lastError == ESP_ERR_NVS_NO_FREE_PAGES) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        if(nvs_flash_erase()!=ESP_OK) {
            Log.error(F(LOG_AS "Failed to format flash" CR));
            return;
        }
        lastError = nvs_flash_init_custom("nvs", sector_start, sector_count);
    }
    if(lastError!=ESP_OK) {
        Log.error(F(LOG_AS "Failed to init nvs region" CR));
        return;
    }
    lastError = nvs_open("storage", NVS_READWRITE, &handle);
    if (lastError != ESP_OK) {
        Log.error(F(LOG_AS "Error (%X) opening NVS handle!" CR), lastError);
    }
    opened = (lastError == ESP_OK);
}

void NvsSettings::commit() {
    nvs_commit(handle);
}

bool loadConfig(settings_t &setting) {
#if 0
    EEPROM.begin(sizeof(settings_t));
    EEPROM.get(0, setting);
    EEPROM.end();

    if(setting.crc != crc32.cksum((uint8_t*)&setting.data, sizeof(setting.data))) {
        Log.error(F(LOG_AS "Loading settings: FAILED" CR));
        // set up the settings variable with default values
        memset(&setting, 0, sizeof(settings_t));

        setting.data.mqtt_port = DEFAULT_MQTT_PORT;
        strlcpy(setting.data.mqtt_host, DEFAULT_MQTT_HOST, MQTT_HOST_LEN);
        strlcpy(setting.data.mqtt_topic, DEFAULT_MQTT_TOPIC, MQTT_TOPIC_LEN);
        return false;;
    }
    Log.notice(F(LOG_AS "Loading settings: OK" CR));
    printSettings(setting);
    return true;
#endif

    noInterrupts();

    Log.verbose(F(LOG_AS "Opening Non-Volatile Storage (NVS) handle... " CR));
    esp_err_t err = nvs_flash_init_custom("nvs", sector_start, sector_count);
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        if(nvs_flash_erase()!=ESP_OK) {
            Log.error(F(LOG_AS "Failed to format flash" CR));
            return false;
        }
        err = nvs_flash_init_custom("nvs", sector_start, sector_count);
    }
    if(err!=ESP_OK) {
        Log.error(F(LOG_AS "Failed to init nvs region" CR));
        return false;
    }

    // Open
    nvs_handle my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        Log.error(F(LOG_AS "Error (%X) opening NVS handle!" CR), err);
    } else {
        // Read
        Log.verbose(F(LOG_AS "Reading restart counter from NVS ... " CR));
        int32_t restart_counter = 0; // value will default to 0, if not set yet in NVS
        err = nvs_get_i32(my_handle, "restart_counter", &restart_counter);
        switch (err) {
            case ESP_OK:
                Log.verbose(F(LOG_AS "success!" CR));
                Log.verbose(F(LOG_AS "Restart counter = %d" CR), restart_counter);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                Log.verbose(F(LOG_AS"The value is not initialized yet!" CR));
                break;
            default :
                Log.error(F(LOG_AS "Error (%X) reading!" CR), err);
        }

        // Write
        restart_counter++;
        err = nvs_set_i32(my_handle, "restart_counter", restart_counter);
        Log.verbose(F(LOG_AS "Updating restart counter in NVS ... %s" CR), (err != ESP_OK) ? "Failed!" : "Done");

        // Commit written value.
        // After setting any values, nvs_commit() must be called to ensure changes are written
        // to flash storage. Implementations may write to storage at other times,
        // but this is not guaranteed.
        err = nvs_commit(my_handle);
        Log.verbose(F(LOG_AS "Committing updates in NVS ... %s" CR), (err != ESP_OK) ? "Failed!" : "Done");

        // Close
        nvs_close(my_handle);
    }
    return true;
}

bool saveConfig(const settings_t &setting) {
    bool ret;
    settings_t store;

    EEPROM.begin(sizeof(settings_t));

    store.crc = crc32.cksum((uint8_t*)&setting.data, sizeof(setting.data));
    store.data = setting.data;

    EEPROM.put(0, store);

    ret = EEPROM.commit();
    EEPROM.end();

    Log.notice(F(LOG_AS "EEPROM commit finished. Return value: %T" CR), ret);

    return ret;
}
