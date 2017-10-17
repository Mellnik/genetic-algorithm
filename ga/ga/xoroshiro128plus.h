#ifndef __XOROSHIRO128P_H
#define __XOROSHIRO128P_H

extern "C" void xoroshiro128p_seed(uint64_t seed);
extern "C" int xoroshiro128p_bool();
extern "C" double xoroshiro128p_double();
extern "C" uint64_t xoroshiro128p_next();

#endif /* __XOROSHIRO128P_H */