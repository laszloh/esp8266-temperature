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
    static RtcMemory &instance() {
        static RtcMemory instance;
        return instance;
    }

    void begin();

    void WriteRtcMemory();

    bool isRtcValid() const {
        return _valid;
    }

    void getBssid(uint8_t bssid[6]) const;
    void setBssid(const uint8_t bssid[6]);

    uint8_t getChannel() const;
    void setChannel(const uint8_t chl);

    IPAddress getIp() const;
    void setIp(const IPAddress ip);

    IPAddress getMask() const;
    void setMask(const IPAddress msk);

    IPAddress getGateway() const;
    void setGateway(const IPAddress gw);

    IPAddress getDns() const;
    void setDns(const IPAddress dns);

    IPAddress getMqttServerIp() const;
    void setMqttServerIp(const IPAddress mqtt_ip);

    uint16_t getMqttLocalPort() const;
    void setMqttLocalPort(const uint16_t mqtt_local_port);

    bool isReconfigure() const;
    void setReconfigure(const bool state);

    bool isBootloader() const;
    void setBootloader(const bool state);

    uint8_t getResetCounter() const;
    void incResetCounter();
    void setResetCounter(const uint8_t value);

    uint32_t getLastWakeDuration() const;
    void setLastWakeDuration(const uint32_t duration);

private:
    RtcMemory() { };
    RtcMemory(const RtcMemory &);
    RtcMemory & operator = (const RtcMemory&);

    void ReadRtcMemory();
    void printRtc();

    bool _valid;
};

#endif