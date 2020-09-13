#pragma once

#include <ESP8266WiFi.h>

typedef struct {
  uint8 bssid[6];
  uint8 chl;
  IPAddress ip;
  IPAddress gw;
  IPAddress msk;
  IPAddress dns;
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
