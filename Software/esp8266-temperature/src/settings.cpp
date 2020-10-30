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

#include "settings.h"
#include "nvs_flash.h"

#define LOG_AS "[FLASH] "

extern "C" uint32_t _FS_start;
extern "C" uint32_t _FS_end;

static const uint32_t sector_start = ((uint32_t)&_FS_start - 0x40200000) / SPI_FLASH_SEC_SIZE;
static const uint32_t sector_count = (((uint32_t)&_FS_end - 0x40200000) / SPI_FLASH_SEC_SIZE) - sector_start;

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

bool NvsSettings::open(const char *name, void *ptr, size_t dataSize) {
    if(dataSize <= 8) {
        // we can load the data as a primitive
        uint64_t data;
        lastError = nvs_get_u64(handle, name, &data);
        if(lastError != ESP_OK)
            return false;
        memcpy(ptr, &data, dataSize);
    } else {
        // we saved it as a blob
        size_t size = dataSize;
        lastError = nvs_get_blob(handle, name, ptr, &size);
    }
    return (lastError == ESP_OK);
}

void NvsSettings::save(const char *name, const void *ptr, size_t dataSize) {
    if(dataSize <= 8) {
        // save it as a primitive
        uint64_t data;
        memcpy(&data, ptr, dataSize);
        lastError = nvs_set_u64(handle, name, data);
    } else {
        // save it as a blob
        lastError = nvs_set_blob(handle, name, ptr, dataSize);
    }
}

uint32_t NvsSettings::getFingerprint() {
    uint32_t fp = 0;
    lastError = nvs_get_u32(handle, "sett-print", &fp);
    return fp;
}

void NvsSettings::setFingerprint(uint32_t fp) {
    lastError = nvs_set_u32(handle, "sett-print", fp);
}

bool NvsSettings::isFirstRun() {
    uint8_t firstRun = 1;
    lastError = nvs_get_u8(handle, "sett-first", &firstRun);
    return (firstRun);
}

void NvsSettings::setFirstRun(bool v) {
    uint8_t firstRun = (v) ? 1 : 0;
    lastError = nvs_set_u8(handle, "sett-first", firstRun);
}
