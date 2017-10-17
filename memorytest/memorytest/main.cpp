// http://stackoverflow.com/questions/36029872/using-a-void-pointer-to-store-a-128-bit-number-in-c/36030133
// http://stackoverflow.com/questions/11815894/how-to-read-write-arbitrary-bits-in-c-c

#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <bitset>
#include <iostream>

#include "xoroshiro128p.h"

using std::cout;
using std::endl;
using std::cin;
using std::bitset;

#define CHROMO_LEN 128

#define TAKE_N_BITS_FROM(b, p, n) ((b) >> (p)) & ((1 << (n)) - 1)

typedef struct Chromo {
	unsigned char *genes;
} Chromo;

void setall(unsigned char *ptr, int size)
{
	for (int i = 0; i < size / CHAR_BIT; i++) {
		for (int c = 0; c < CHAR_BIT; c++) {
			*(unsigned char *)(ptr + i) |= 1 << c;
		}
	}
}

void resetall(unsigned char *ptr, int size)
{
	for (int i = 0; i < size / CHAR_BIT; i++) {
		for (int c = 0; c < CHAR_BIT; c++) {
			*(unsigned char *)(ptr + i) &= ~(1 << c);
		}
	}
}

void setbit(unsigned char *ptr, int pos)
{
	int c = pos % CHAR_BIT;
	*(unsigned char *)(ptr + ((pos - c) / CHAR_BIT)) |= 1 << c;
}

void resetbit(unsigned char *ptr, int pos)
{
	int c = pos % CHAR_BIT;
	*(unsigned char *)(ptr + ((pos - c) / CHAR_BIT)) &= ~(1 << c);
}

int testbit(unsigned char *ptr, int pos)
{
	int c = pos % CHAR_BIT;
	return ((*(unsigned char *)(ptr + ((pos - c) / CHAR_BIT))) & (1 << c));
}

void bitcpy(unsigned char *dst, unsigned char *src, int len)
{
	int c = len % CHAR_BIT;
	memcpy(dst, src, (len - c) / CHAR_BIT);
	*(unsigned char *)(dst + ((len - c) / CHAR_BIT)) = TAKE_N_BITS_FROM(*(unsigned char *)(src + ((len - c) / CHAR_BIT)), 0, c);
}

void bitcpy(unsigned char *dst, unsigned char *src, int start, int len, int size)
{
	for (int i = 0; i < size / CHAR_BIT; i++) {
		for (int c = 0; c < CHAR_BIT; c++) {
			if (((i * CHAR_BIT) + c) >= start && ((i * CHAR_BIT) + c) - start < len) {
				if (testbit(src, ((i * CHAR_BIT) + c)))
					setbit(dst, ((i * CHAR_BIT) + c));
				else
					resetbit(dst, ((i * CHAR_BIT) + c));
			}			
		}
	}
}

// http://geneticalgorithms.ai-depot.com/Tutorial/Crossover.jpeg
void crossover(unsigned char *offspring1, unsigned char *offspring2, int point, int size)
{
	size_t byte_len = size / CHAR_BIT;
	unsigned char *tmp = (unsigned char *)malloc(byte_len);
	memcpy(tmp, offspring1, byte_len);

	bitcpy(offspring1, offspring2, 0, size - point, size);
	bitcpy(offspring2, tmp, 0, size - point, size);

	free(tmp);
}

void debug(unsigned char *ptr, int len)
{
	for (int i = 0; i < len / CHAR_BIT; i++) {
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
	cout << endl << endl;
}

#include <ctime>

int main(int argc, char *argv[])
{
	/*const int CHROMO2_LEN = 16;
	unsigned char *g = (unsigned char *)malloc(CHROMO2_LEN / CHAR_BIT);
	unsigned char *g2 = (unsigned char *)malloc(CHROMO2_LEN / CHAR_BIT);

	resetall(g, CHROMO2_LEN);
	setall(g2, CHROMO2_LEN);
	resetbit(g2, 6);
	resetbit(g2, 7);
	resetbit(g2, 8);
	resetbit(g2, 9);
	
	debug(g, CHROMO2_LEN);
	debug(g2, CHROMO2_LEN);

	//bitcpy(g, g2, 11/*, 5);
	//bitcpy(g, g2, 0, 2005);

	debug(g, CHROMO2_LEN);
	debug(g2, CHROMO2_LEN);*/

	xoroshiro128p_seed((uint64_t)time(NULL));

	for (int i = 0; i < 100; i++) {
		//cout << ((float)rand() / (RAND_MAX + 1)) << endl;
		cout.precision(std::numeric_limits<double>::digits10);
		cout << std::fixed << xoroshiro128p_double() << endl;
	}

	Chromo off1;
	Chromo off2;
	off1.genes = (unsigned char *)malloc(CHROMO_LEN / CHAR_BIT);
	off2.genes = (unsigned char *)malloc(CHROMO_LEN / CHAR_BIT);

	resetall(off2.genes, CHROMO_LEN);
	setall(off1.genes, CHROMO_LEN);
	resetbit(off1.genes, 40);

	debug(off1.genes, CHROMO_LEN);
	debug(off2.genes, CHROMO_LEN);

	crossover(off1.genes, off2.genes, CHROMO_LEN / 2, CHROMO_LEN);

	debug(off1.genes, CHROMO_LEN);
	debug(off2.genes, CHROMO_LEN);
	
	cin >> argc;
	return 0;
}