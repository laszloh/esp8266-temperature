/**
 * @file rtc.cpp
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
#include <ESP8266WiFi.h>
#include <CRC32.h>
#include <ArduinoLog.h>

#include "rtc.h"

extern "C" {
    #include "user_interface.h"
}

#define RTC_MEMORY_VERSION 1
#define LOG_AS "[RTC] "

void RtcMemory::ReadRtcMemory() {
    ESP.rtcUserMemoryRead(0, (uint32_t*)&_rtc, sizeof(rtcbuf_t));
    
    _valid = true;
    if(_rtc.version != RTC_MEMORY_VERSION || _rtc.crc != CRC32::calculate(&_rtc, sizeof(rtcbuf_t))) {
        Log.error(F(LOG_AS "RTC memory corrupted"));
        memset(&_rtc, 0xFF, sizeof(rtcbuf_t));
        _valid = false;
    }
}

void RtcMemory::WriteRtcMemory() {
    _rtc.version = RTC_MEMORY_VERSION;
    _rtc.crc = CRC32::calculate(&_rtc, sizeof(rtcbuf_t));

    ESP.rtcUserMemoryWrite(0, (uint32_t*)&_rtc, sizeof(rtcbuf_t));
}
