#ifndef __SPLITMIX64_H
#define __SPLITMIX64_H

void splitmix64_seed(uint64_t seed);
uint64_t splitmix64_next();

#endif /* __SPLITMIX64_H */