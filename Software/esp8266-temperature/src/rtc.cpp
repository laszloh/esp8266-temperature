#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <CRC32.h>

#include "rtc.h"

extern "C" {
    #include "user_interface.h"
    #include <string.h>
}

bool readcfg(cfgbuf_t *config) {
    ESP.rtcUserMemoryRead(0, (uint32_t*) config, sizeof(cfgbuf_t));
    if(CRC32::calculate((uint32_t*)config, sizeof(cfgbuf_t)) != 0) {
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

    wifi_station_get_config(&wifi_conf);
    memcpy(&rtc_buffer.bssid, &wifi_conf.bssid, sizeof(wifi_conf.bssid));
    rtc_buffer.chl = wifi_get_channel();
    rtc_buffer.ip = WiFi.localIP();
    rtc_buffer.gw = WiFi.gatewayIP();
    rtc_buffer.msk = WiFi.subnetMask();
    rtc_buffer.dns = WiFi.dnsIP();
    rtc_buffer.crc = CRC32::calculate((uint32_t*)&rtc_buffer, sizeof(cfgbuf_t) - sizeof(uint32_t));

    ESP.rtcUserMemoryWrite(0, (uint32_t*)&rtc_buffer, sizeof(cfgbuf_t));
}
