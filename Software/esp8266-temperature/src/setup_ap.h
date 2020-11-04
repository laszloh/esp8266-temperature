/**
 * @file setup_ap.h
 * @author Laszlo Hegedüs (laszlo.hegedues@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-10-01
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

#include <WiFiManager.h>

bool setup_ap();

class Parameter : public WiFiManagerParameter {
  public:
    /** 
        Create custom parameters that can be added to the WiFiManager setup web page
        @id is used for HTTP queries and must not contain spaces nor other special characters
    */
    Parameter() : WiFiManagerParameter() { }
    Parameter(const char *custom) : WiFiManagerParameter(custom) { }
    Parameter(const char *id, const char *label) : WiFiManagerParameter(id, label) { }
    Parameter(const char *id, const char *label, const char *defaultValue, int length) : 
        WiFiManagerParameter(id, label, defaultValue, length) { }
    Parameter(const char *id, const char *label, const char *defaultValue, int length, const char *custom) : 
        WiFiManagerParameter(id, label, defaultValue, length, custom) { }
    Parameter(const char *id, const char *label, const char *defaultValue, int length, const char *custom, int labelPlacement) : 
        WiFiManagerParameter(id, label, defaultValue, length, custom, labelPlacement) { }

    enum ParamType {
        StringType,
        IntegerType,
        FloatType
    };

    const char *getID();
    const char *getValue();
    const char *getLabel();
    const char *getPlaceholder(); // @deprecated, use getLabel
    int         getValueLength();
    int         getLabelPlacement();
    const char *getCustomHTML();
    void        setValue(const char *defaultValue, int length);
    ParamType&  type() { return _type; }

  protected:
    void init(const char *id, const char *label, const char *defaultValue, int length, const char *custom, int labelPlacement);

  private:
    Parameter& operator=(const Parameter&);
    const char *_id;
    const char *_label;
    char       *_value;
    int         _length;
    int         _labelPlacement;
    const char *_customHTML;
    ParamType   _type;
    
    friend class WiFiManager;
};
