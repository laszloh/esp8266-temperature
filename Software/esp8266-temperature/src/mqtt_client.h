/**
 * @file mqtt_client.h
 * @author Laszlo Hegedüs (laszlo.hegedues@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-10-02
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

#include "settings.h"
#include "rtc.h"

class MqttClient {
public:
    static MqttClient& instace() {
        static MqttClient instace;
        return instace;
    }
	
    void begin();
    bool connect();

    void sendMeasurement(float temperature, float humidity, float pressure);
    void sendStatus(uint16_t voltage);
    void loop();


private:
    MqttClient();
    MqttClient(const MqttClient&);
    MqttClient& operator = (const MqttClient&);

    RtcMemory& rtc;
    NvsSettings& settings;

    char clientId[32];

    int8_t getRssiQuality(const int32_t rssi) const;
    static void callback(char* topic, uint8_t* payload, uint16_t length);
};

