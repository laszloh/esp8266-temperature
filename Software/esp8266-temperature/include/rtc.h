/**
 * @file rtc.h
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
#ifndef _RTC_H_
#define _RTC_H_H

class RtcMemory{
public:
    static RtcMemory &getInstance() {
        static RtcMemory instance;
        return instance;
    }

    void WriteRtcMemory();

    bool isRtcValid() const {
        return _valid;
    }

    void getBssid(uint8_t bssid[6]) const {
        memcpy(bssid, _rtc.data.bssid, 6);
    }
    void setBssid(const uint8_t bssid[6]) {
        memcpy(_rtc.data.bssid, bssid, 6);
    }

    uint8_t getChannel() const {
        return _rtc.data.chl;
    }
    void setChannel(const uint8_t chl) {
        _rtc.data.chl = chl;
    }

    IPAddress getIp() const{
        return IPAddress(_rtc.data.ip);
    }
    void setIp(const IPAddress ip) {
        _rtc.data.ip = ip;
    }

    IPAddress getMask() const{
        return IPAddress(_rtc.data.msk);
    }
    void setMask(const IPAddress msk) {
        _rtc.data.msk = msk;
    }

    IPAddress getGateway() const{
        return IPAddress(_rtc.data.gw);
    }
    void setGateway(const IPAddress gw) {
        _rtc.data.gw = gw;
    }

    IPAddress getDns() const{
        return IPAddress(_rtc.data.dns);
    }
    void setDns(const IPAddress dns) {
        _rtc.data.dns = dns;
    }

    IPAddress getMqttServerIp() const{
        return IPAddress(_rtc.data.mqtt_ip);
    }
    void setMqttServerIp(const IPAddress mqtt_ip) {
        _rtc.data.mqtt_ip = mqtt_ip;
    }

    uint16_t getMqttLocalPort() const {
        return _rtc.data.mqtt_local_port;
    }
    void setMqttLocalPort(const uint16_t mqtt_local_port) {
        _rtc.data.mqtt_local_port = mqtt_local_port;
    }

    bool getReconfigure() const {
        return (_rtc.data.flags.reconfigure == 1);
    }
    void setReconfigure(const bool state) {
        _rtc.data.flags.reconfigure = (state) ? 1 : 0;
    }

    bool getBootloader() const {
        return (_rtc.data.flags.bootloader == 1);
    }
    void setBootloader(const bool state) {
        _rtc.data.flags.bootloader = (state) ? 1 : 0;
    }

    uint32_t getLastWakeDuration() const {
        return _rtc.data.last_wake_time;
    }
    void setLastWakeDuration(const uint32_t duration) {
        _rtc.data.last_wake_time = duration;
    }

private:
    RtcMemory(){
        ReadRtcMemory();
    };

    typedef struct __attribute__ ((packed)) {
        uint8_t version;
        uint32_t crc;
        struct data_t {
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
            } flags;
            uint32_t last_wake_time;
        } data;
    } rtcbuf_t;

    void ReadRtcMemory();

    bool _valid;
    rtcbuf_t _rtc;
};

#endif