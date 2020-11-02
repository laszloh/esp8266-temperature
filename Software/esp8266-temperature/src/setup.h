#pragma once

// wifi settings
#include "wifi_priv.h"

#define DEFAULT_WIFI_SSID       PRIVATE_WIFI_SSID
#define DEFAULT_WIFI_PASS       PRIVATE_WIFI_PASS
#define DEFAULT_WIFI_TIMEOUT    4000                //< ms


// MQTT settings
#define DEFAULT_MQTT_HOST       "192.168.88.12"
#define DEFAULT_MQTT_PORT       1883
#define DEFAULT_MQTT_LOGIN      PRIVATE_MQTT_LOGIN
#define DEFAULT_MQTT_PASS       PRIVATE_MQTT_PASS
#define DEFAULT_MQTT_TOPIC      "revai/sensors/{i}/{t}"
#define DEFAULT_MQTT_ID         "ESP{m}"
#define DEFAULT_MQTT_TIMEOUT    2000                //< ms

// System settings
#define DEFAULT_SYSTEM_LED	    true
#define DEFAULT_SYSTEM_SLEEP    30                  //< s