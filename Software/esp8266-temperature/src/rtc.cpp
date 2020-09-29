#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <CRC32.h>

#include "rtc.h"

extern "C" {
    #include "user_interface.h"
    #include <string.h>
}

//#define DEBUG

#ifdef DEBUG
    #define print(...)  Serial.print(__VA_ARGS__)
    #define println(...)  Serial.println(__VA_ARGS__)

    //prints all rtcData, including the leading crc32
    void printMemory(uint8_t *buffer, uint8_t size) {
    char buf[3];
    uint8_t *ptr = buffer;
    for (size_t i = 0; i < size; i++) {
        sprintf(buf, "%02X", ptr[i]);
        print(buf);
        if ((i + 1) % 32 == 0) {
        println();
        } else {
        print(" ");
        }
    }
    println();
    }
#else
    #define print(...)  {}
    #define println(...)  {}
    #define printMemory(...)    {}
#endif

static uint32_t calculateCRC32(const uint8_t *data, size_t length) {
  uint32_t crc = 0xffffffff;
  while (length--) {
    uint8_t c = *data++;
    for (uint32_t i = 0x80; i > 0; i >>= 1) {
      bool bit = crc & 0x80000000;
      if (c & i) {
        bit = !bit;
      }
      crc <<= 1;
      if (bit) {
        crc ^= 0x04c11db7;
      }
    }
  }
  return crc;
}

bool readcfg(cfgbuf_t *config) {
    ESP.rtcUserMemoryRead(0, (uint32_t*) config, sizeof(cfgbuf_t));
    printMemory((uint8_t*)config, sizeof(cfgbuf_t));

    uint32_t crc = config->crc;
    config->crc = 0;
    if(calculateCRC32((uint8_t*)config, sizeof(cfgbuf_t)) != crc) {
        // CRC should have been 0
        memset(config, 0, sizeof(cfgbuf_t));
        config->msk = IPAddress(255,255,255,0);
        return false;
    }
    return true;
}

void writecfg(void) {
    struct station_config wifi_conf;
    cfgbuf_t rtc_buffer;

    memset(&rtc_buffer, 0, sizeof(cfgbuf_t));

    wifi_station_get_config(&wifi_conf);
    memcpy(&rtc_buffer.bssid, &wifi_conf.bssid, sizeof(wifi_conf.bssid));
    rtc_buffer.chl = wifi_get_channel();
    rtc_buffer.ip = WiFi.localIP();
    rtc_buffer.gw = WiFi.gatewayIP();
    rtc_buffer.msk = WiFi.subnetMask();
    rtc_buffer.dns = WiFi.dnsIP();
    rtc_buffer.crc = calculateCRC32((uint8_t*)&rtc_buffer, sizeof(cfgbuf_t));

    ESP.rtcUserMemoryWrite(0, (uint32_t*)&rtc_buffer, sizeof(cfgbuf_t));
}
