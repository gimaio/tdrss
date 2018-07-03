#ifndef CRC22_H_INCLUDED
#define CRC22_H_INCLUDED

#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <pthread.h>
#include <stdio.h>
#include <cstring>
#include <stdexcept>
#include <iterator>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fstream>
#include <inttypes.h>
#include <bitset>

using namespace std;

struct crc32
{
	static void generate_table(uint32_t(&table)[256])
	{
		uint32_t polynomial = 0x005079AB;
		for (uint32_t i = 0; i < 256; i++)
		{
			uint32_t c = i;
			for (size_t j = 0; j < 8; j++)
			{
				if (c & 1) {
					c = polynomial ^ (c >> 1);
				}
				else {
					c >>= 1;
				}
			}
			table[i] = c;
		}
	}

	static uint32_t update(uint32_t (&table)[256], uint32_t initial, const void* buf, size_t len)
	{
		uint32_t c = initial ^ 0x3FFFFF;
		const uint8_t* u = static_cast<const uint8_t*>(buf);
		for (size_t i = 0; i < len; ++i)
		{
			c = table[(c ^ u[i]) & 0xFF] ^ (c >> 8);
		}
		return c ^ 0x3FFFFF;
	}
};


// usage: the following code generates crc for 2 pieces of data
// uint32_t table[256];
// crc32::generate_table(table);
// uint32_t crc = crc32::update(table, 0, data_piece1, len1);
// crc = crc32::update(table, crc, data_piece2, len2);
// output(crc);

#endif // CRC22_H_INCLUDED
