/**
 * @file main.h
 * @author Laszlo Hegedüs (laszlo.hegedues@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-10-20
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
#pragma once

#include "settings.h"

#define DEFAULT_SLEEP_TIME  30

class Main {
    friend void setup();
    friend void loop();

public:
    static Main& instance() {
        static Main instance;
        return instance;
    }

    uint32_t getSleepTime() const {
        Config c(config);
        return c.sleepTime;
    }

    void configure(uint32_t sleepTime) {
        Config c(config);
        c.sleepTime = sleepTime *60 * 1000 * 1000;
        config = c;
    }

private:
    Main() :
        settings(NvsSettings::instance()),
        config(Config(), settings, "main") 
        { }
    Main(const Main&);
    Main& operator = (const Main&);
    
    struct Config {
        uint32_t sleepTime;

        Config(uint32_t _sleep =  DEFAULT_SLEEP_TIME) : sleepTime(_sleep * 60 * 1000 * 1000)
        { }
    };

    NvsSettings& settings;
    NvsValue<Config> config;
};
