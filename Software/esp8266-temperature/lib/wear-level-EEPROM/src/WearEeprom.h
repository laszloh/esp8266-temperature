/**
 * @file WearEeprom.h
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
#ifndef _WEAREEPROM_H_
#define _WEAREEPROM_H_

#include <EEPROM.h>

class WearEeprom : public EEPROMClass {
public:
    WearEeprom(uint32_t sector);
    WearEeprom(void);

    void begin(size_t size, uint16_t count = 0);
    uint8_t read(int const address);
    void write(int const address, uint8_t const val);
    bool commit();

    bool format();

protected:
    uint16_t _count = 0;
    uint32_t _pool_size = 1;

    uint16_t _offset = 0;
    uint8_t _sector_index = 0;
    uint8_t _sector_value = 0;

    bool _enabled = true;

    void _auto();
    uint32_t _getSector(uint8_t index);
    uint8_t _getIndex(uint32_t sector);
    uint16_t _calculateCRC();
    bool _checkCRC();
};

#endif