#pragma once

#include <ESP8266WiFi.h>

typedef struct __attribute__ ((packed)) {
  uint8 bssid[6];
  uint8 chl;
  ip_addr_t ip;
  ip_addr_t gw;
  ip_addr_t msk;
  ip_addr_t dns;
  uint16_t localPort;
  uint32_t crc;
} cfgbuf_t;

/**
 * @brief 
 * 
 * @param config 
 * @return true 
 * @return false 
 */
bool readcfg(cfgbuf_t *config);

/**
 * @brief 
 * 
 */
void writecfg(void);
