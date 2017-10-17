#ifndef __BITSET_H
#define __BITSET_H

#include <cstdlib>
#include <iostream>

using std::cout;
using std::endl;

#define TAKE_N_BITS_FROM(b, p, n) ((b) >> (p)) & ((1 << (n)) - 1)

class Bitset
{
public:
	static void setall(unsigned char *ptr, int size)
	{
		for (int i = 0; i < size / CHAR_BIT; i++) {
			for (int c = 0; c < CHAR_BIT; c++) {
				*(unsigned char *)(ptr + i) |= 1 << c;
			}
		}
	}

	static void resetall(unsigned char *ptr, int size)
	{
		for (int i = 0; i < size / CHAR_BIT; i++) {
			for (int c = 0; c < CHAR_BIT; c++) {
				*(unsigned char *)(ptr + i) &= ~(1 << c);
			}
		}
	}

	static void setbit(unsigned char *ptr, int pos)
	{
		int c = pos % CHAR_BIT;
		*(unsigned char *)(ptr + ((pos - c) / CHAR_BIT)) |= 1 << c;
	}

	static void resetbit(unsigned char *ptr, int pos)
	{
		int c = pos % CHAR_BIT;
		*(unsigned char *)(ptr + ((pos - c) / CHAR_BIT)) &= ~(1 << c);
	}

	static void flipbit(unsigned char *ptr, int pos)
	{
		int c = pos % CHAR_BIT;
		*(unsigned char *)(ptr + ((pos - c) / CHAR_BIT)) ^= (1 << c);
	}

	static int testbit(unsigned char *ptr, int pos)
	{
		int c = pos % CHAR_BIT;
		return ((*(unsigned char *)(ptr + ((pos - c) / CHAR_BIT))) & (1 << c));
	}

	static void bitcpy(unsigned char *dst, unsigned char *src, int size)
	{
		int c = size % CHAR_BIT;
		memcpy(dst, src, (size - c) / CHAR_BIT);
		*(unsigned char *)(dst + ((size - c) / CHAR_BIT)) = TAKE_N_BITS_FROM(*(unsigned char *)(src + ((size - c) / CHAR_BIT)), 0, c);
	}

	static void bitcpy(unsigned char *dst, unsigned char *src, int start, int len, int size)
	{
		register int count = 0;
		register int maxb = size / CHAR_BIT;
		for (int i = (((start - (start % CHAR_BIT)) / CHAR_BIT)); i < maxb; i++) {
			for (int c = 0; c < CHAR_BIT; c++) {
				if (((i * CHAR_BIT) + c) >= start/* && ((i * CHAR_BIT) + c) - start < len*/) {
					if (((*(unsigned char *)(src + i)) & (1 << c))) {
						*(unsigned char *)(dst + i) |= 1 << c;
					}
					else {
						*(unsigned char *)(dst + i) &= ~(1 << c);
					}
					if (++count == len) {
						goto out;
					}
				}
			}
		}
	out:
		return; // Make compiler happy
	}

	static unsigned long to_ulong(unsigned char *ptr, int start, int len, int size)
	{
		if (len > 32) // unsigned long can hold 32 bits
			return 0;

		unsigned long ret = 0;
		register int count = 0;
		register int maxb = size / CHAR_BIT;
		for (int i = (((start - (start % CHAR_BIT)) / CHAR_BIT)); i < maxb; i++) {
			for (int c = 0; c < CHAR_BIT; c++) {
				if (((i * CHAR_BIT) + c) >= start) {
					if (((*(unsigned char *)(ptr + i)) & (1 << c))) {
						ret |= 1 << count;
					}
					else {
						ret &= ~(1 << count);
					}
					if (++count == len) {
						goto out;
					}
				}
			}
		}
		out:
		return ret;
	}

	// http://geneticalgorithms.ai-depot.com/Tutorial/Crossover.jpeg
	static void crossover_singlepoint(unsigned char *offspring1, unsigned char *offspring2, int point, int size)
	{
		size_t byte_len = size / CHAR_BIT;
		unsigned char *tmp = (unsigned char *)malloc(byte_len);
		memcpy(tmp, offspring1, byte_len);

		bitcpy(offspring1, offspring2, 0, size - point, size);
		bitcpy(offspring2, tmp, 0, size - point, size);

		free(tmp);
	}

	static void debug(unsigned char *ptr, int size)
	{
		for (int i = 0; i < size / CHAR_BIT; i++) {
			for (int c = 0; c < CHAR_BIT; c++) {
				if ((*(unsigned char *)(ptr + i)) & (1 << c)) {
					cout << "1";
				}
				else {
					cout << "0";
				}
			}
			//cout << " ";
		}
		cout << endl;
	}
};

#endif