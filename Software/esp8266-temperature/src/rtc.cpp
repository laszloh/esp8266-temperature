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
#define EBOOT_RTC_OFFSET 128/4

struct rtcbuf_t {
    uint8_t version;
    uint32_t crc;
    struct data_t  {
        uint8_t bssid[6];
        uint8_t chl;
        ip_addr_t ip;
        ip_addr_t msk;
        ip_addr_t gw;
        ip_addr_t dns;
        ip_addr_t mqtt_ip;
        uint16_t mqtt_local_port;
        struct flags_t {
            uint8_t reconfigure:1;
            uint8_t bootloader:1;
            uint8_t reserved:6;
        } __attribute__ ((packed)) flags;
        uint32_t last_wake_time;
    } __attribute__ ((packed)) data;
} __attribute__ ((packed));

static rtcbuf_t _rtc;

void RtcMemory::printRtc() {
    Log.verbose(F(LOG_AS "RTC memory content:" CR));
    Log.verbose(F(LOG_AS "    Ver:  %d" CR), _rtc.version);
    Log.verbose(F(LOG_AS "    CRC:  %X" CR), _rtc.crc);
    Log.verbose(F(LOG_AS "    Chan: %d" CR), _rtc.data.chl);
    Log.verbose(F(LOG_AS "    IP:   %s" CR), IPAddress(_rtc.data.ip).toString().c_str());
    Log.verbose(F(LOG_AS "    Mask: %s" CR), IPAddress(_rtc.data.msk).toString().c_str());
    Log.verbose(F(LOG_AS "    GW:   %s" CR), IPAddress(_rtc.data.gw).toString().c_str());
    Log.verbose(F(LOG_AS "    DNS:  %s" CR), IPAddress(_rtc.data.dns).toString().c_str());
    Log.verbose(F(LOG_AS "    mqtt: %s" CR), IPAddress(_rtc.data.mqtt_ip).toString().c_str());
    Log.verbose(F(LOG_AS "    Flag: %X" CR), *((uint8_t*)&_rtc.data.flags));
    Log.verbose(F(LOG_AS "    Port: %d" CR), _rtc.data.mqtt_local_port);
    Log.verbose(F(LOG_AS "    Wake: %d" CR), _rtc.data.last_wake_time);
}

void RtcMemory::ReadRtcMemory() {
    system_rtc_mem_read(64+EBOOT_RTC_OFFSET, &_rtc, sizeof(_rtc));
    
    _valid = true;
    if(_rtc.version != RTC_MEMORY_VERSION || _rtc.crc != CRC32::calculate((uint8_t*)&_rtc.data, sizeof(_rtc.data))) {
        Log.error(F(LOG_AS "RTC memory corrupted" CR));
        memset(&_rtc, 0x00, sizeof(rtcbuf_t));
        _valid = false;
    }
    printRtc();
}

void RtcMemory::WriteRtcMemory() {
    _rtc.version = RTC_MEMORY_VERSION;
    _rtc.crc = CRC32::calculate((uint8_t*)&_rtc.data, sizeof(_rtc.data));

    system_rtc_mem_write(64+EBOOT_RTC_OFFSET, &_rtc, sizeof(_rtc));
}

void RtcMemory::getBssid(uint8_t bssid[6]) const {
    memcpy(bssid, _rtc.data.bssid, 6);
}
void RtcMemory::setBssid(const uint8_t bssid[6]) {
    memcpy(_rtc.data.bssid, bssid, 6);
}

uint8_t RtcMemory::getChannel() const {
    return _rtc.data.chl;
}
void RtcMemory::setChannel(const uint8_t chl) {
    _rtc.data.chl = chl;
}

IPAddress RtcMemory::getIp() const{
    return IPAddress(_rtc.data.ip);
}
void RtcMemory::setIp(const IPAddress ip) {
    _rtc.data.ip = ip;
}

IPAddress RtcMemory::getMask() const{
    return IPAddress(_rtc.data.msk);
}
void RtcMemory::setMask(const IPAddress msk) {
    _rtc.data.msk = msk;
}

IPAddress RtcMemory::getGateway() const{
    return IPAddress(_rtc.data.gw);
}
void RtcMemory::setGateway(const IPAddress gw) {
    _rtc.data.gw = gw;
}

IPAddress RtcMemory::getDns() const{
    return IPAddress(_rtc.data.dns);
}
void RtcMemory::setDns(const IPAddress dns) {
    _rtc.data.dns = dns;
}

IPAddress RtcMemory::getMqttServerIp() const{
    return IPAddress(_rtc.data.mqtt_ip);
}
void RtcMemory::setMqttServerIp(const IPAddress mqtt_ip) {
    _rtc.data.mqtt_ip = mqtt_ip;
}

uint16_t RtcMemory::getMqttLocalPort() const {
    return _rtc.data.mqtt_local_port;
}
void RtcMemory::setMqttLocalPort(const uint16_t mqtt_local_port) {
    _rtc.data.mqtt_local_port = mqtt_local_port;
}

bool RtcMemory::getReconfigure() const {
    return (_rtc.data.flags.reconfigure == 1);
}
void RtcMemory::setReconfigure(const bool state) {
    _rtc.data.flags.reconfigure = (state) ? 1 : 0;
}

bool RtcMemory::getBootloader() const {
    return (_rtc.data.flags.bootloader == 1);
}
void RtcMemory::setBootloader(const bool state) {
    _rtc.data.flags.bootloader = (state) ? 1 : 0;
}

uint32_t RtcMemory::getLastWakeDuration() const {
    return _rtc.data.last_wake_time;
}
void RtcMemory::setLastWakeDuration(const uint32_t duration) {
    _rtc.data.last_wake_time = duration;
}
