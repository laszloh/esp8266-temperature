#ifndef _CRC_H_
#define _CRC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

uint32_t crc32_le(uint32_t crc, unsigned char const *p, size_t len);

#ifdef __cplusplus
}
#endif

#endif