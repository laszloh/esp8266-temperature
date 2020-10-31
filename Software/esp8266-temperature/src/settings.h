/**
 * @file settings.h
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
#pragma once

#include "setup_ap.h"
#include "nvs.h"

class NvsSettings {
public:
    static NvsSettings& instance() {
        static NvsSettings instance;
        return instance;
    }

    bool open(const char *name, void *ptr, size_t dataSize);
    void save(const char *name, const void *ptr, size_t dataSize);

    esp_err_t getLastError() const {
        return lastError;
    }

    uint32_t getFingerprint();
    void setFingerprint(uint32_t fp);

    bool isFirstRun();
    void setFirstRun(bool v);

    bool isOpened() const {
        return opened;
    }

    nvs_handle getHandle() const {
        return handle;
    }

private:
    NvsSettings();
    NvsSettings(const NvsSettings &);
    NvsSettings& operator = (const NvsSettings&);

    bool opened;
    nvs_handle handle;
    esp_err_t lastError;
};

template <size_t N>
class StrValue {
    char value[N+1];
public:
    StrValue() {
        value[0] = 0;
    }
    StrValue(const char* v) {
        if(v)
            strncpy(value, v, N);
        else
            value[0] = 0;
    }

    size_t size() const {
        return N;
    }

    const void *val_ptr() const {
        return &value;
    }

    bool operator == (const StrValue& v) {
        return (strcmp(value, v.value) == 0);
    }

    bool operator != (const StrValue& v) const {
        return (strcmp(value, v.value) != 0);
    }

    void operator = (const StrValue& v) {
        strncpy(value, v.value, N);
    }

    operator const char*() const {
        return value;
    }

};

template <typename N>
class IntValue {
    N value;

public:
    IntValue() {
        value = 0;
    }
    IntValue(const N v) {
        value = v;
    }

    size_t size() const {
        return sizeof(N);
    }

    const void *val_ptr() const {
        return &value;
    }

    bool operator == (const IntValue& v) const {
        return value == v.value;
    }

    bool operator != (const IntValue& v) const {
        return ! (*this) == v;
    }

    void operator = (const IntValue& v) {
        value = v.value;
    }

    operator const char*() const {
        static char buffer[10];
        snprintf(buffer, sizeof(buffer), "%d", value);
        return buffer;
    }

    operator N() const {
        return value;
    }

};

template <class T>
class NvsValue {
    T value;
    NvsSettings& s;
    const char *id;

public:
    NvsValue(const T& _value, NvsSettings& _s, const char *_id) 
    : value(_value), s(s), id(_id) {
        s.open(id, &value, sizeof(value));
    }

    const char* getID() const {
        return id;
    }

    operator const T& () const {
        return value;
    }

    // commit the variable to the backend
    const T& operator = (const T& v) { 
        if(v != value)
            s.save(id, value.val_ptr(), value.size());
        value = v;
        return value; 
    }

    const T& get() const {
        return value;
    }

    operator const char*() const {
        return value;
    }
};
