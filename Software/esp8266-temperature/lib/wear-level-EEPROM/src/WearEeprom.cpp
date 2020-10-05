/**
 * @file WearEeprom.cpp
 * @author Laszlo Hegedüs (laszlo.hegedues@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-10-05
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
#include <Esp.h>
#include <FastCRC.h>
#include "WearEeprom.h"
#include "debug.h"

extern "C" {
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "spi_flash.h"
}

extern "C" uint32_t _EEPROM_start;
static FastCRC16 crc16;

constexpr uint32_t eeprom_magic = 0x50456C57;   //< "WlEP" in little endian
constexpr uint16_t entry_magic = 0x4578;  //< "Ex" in little endian

#define EEPROM_FLAG_SECOND 0x8000   //< primary (0) or secondary eeprom region (1)
#define EEPROM_FLAG_ACTIVE 0x4000   //< stale (0) or active region (1)
#define EEPROM_FLAG_NWIPE  0x2000   //< region can be erased (0) or has active entries (1)

#define ENTRY_FLAG_ALIVE 0x8000     //< alive (0) or deleted (1) entry
#define ENTRY_FLAG_VALID 0x4000     //< valid (1) or invalid (0) entry

struct eeprom_header {
    uint32_t magic;
    uint16_t flags;
    uint16_t erase_count;
} __packed;

struct entry_header {
    uint16_t magic;
    uint16_t crc16;
    uint16_t len;
    uint16_t flags;
} __packed;


WearEeprom::WearEeprom(uint32_t sector) :
    EEPROMClass(sector),
    _count(0)
{ }

WearEeprom::WearEeprom(void) :
    EEPROMClass(),
    _count(0)
{ }

void WearEeprom::begin(size_t size, uint16_t count) {
    size_t entry_size = size * sizeof(entry_header);

    if(count == 0) {
        // set the maximum number of entries for rotation
        _count = ((_pool_size*SPI_FLASH_SEC_SIZE)-sizeof(eeprom_header)*_pool_size)/entry_size;
    } else {
        if( (entry_size*count+sizeof(eeprom_header)*_pool_size) > (_pool_size*SPI_FLASH_SEC_SIZE)) {
            // we do not have enough flash space to store so many entries
            DEBUGV("WearEeprom::begin not enough flash space to hold the entry count\n");
        }
        _count = count;
    }
}

uint8_t WearEeprom::read(int const address) {

}

void WearEeprom::write(int const address, uint8_t const val) {

}

bool WearEeprom::commit() {

}

bool WearEeprom::format() {

}