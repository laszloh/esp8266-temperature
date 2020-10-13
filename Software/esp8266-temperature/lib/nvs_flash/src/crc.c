#include <stdint.h>
#include <stddef.h>

#include "crc.h"

#define CRCPOLY_LE 0xedb88320
uint32_t crc32table_le[1][256];

/**
 * crc32_le_generic() - Calculate bitwise little-endian Ethernet AUTODIN II
 *			CRC32/CRC32C
 * @crc: seed value for computation.  ~0 for Ethernet, sometimes 0 for other
 *	 uses, or the previous crc32/crc32c value if computing incrementally.
 * @p: pointer to buffer over which CRC32/CRC32C is run
 * @len: length of buffer @p
 * @tab: little-endian Ethernet table
 * @polynomial: CRC32/CRC32c LE polynomial
 */
static inline uint32_t crc32_le_generic(uint32_t crc, unsigned char const *p,
					  size_t len, const uint32_t (*tab)[256],
					  uint32_t polynomial)
{
	/* aka Sarwate algorithm */
	while (len--) {
		crc ^= *p++;
		crc = (crc >> 8) ^ tab[0][crc & 255];
	}
	return crc;
}

uint32_t crc32_le(uint32_t crc, unsigned char const *p, size_t len)
{
	return crc32_le_generic(crc, p, len,
			(const uint32_t (*)[256])crc32table_le, CRCPOLY_LE);
}
