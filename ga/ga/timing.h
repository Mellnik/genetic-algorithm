#ifndef __TIMING_H
#define __TIMING_H

#include <chrono>

#define TIMING_START(tt) std::chrono::high_resolution_clock::time_point start_##tt = std::chrono::high_resolution_clock::now()
#define TIMING_END(tt) std::chrono::high_resolution_clock::time_point end_##tt = std::chrono::high_resolution_clock::now()
#define TIMING_RES(tt) cout << #tt << ": " << static_cast<unsigned>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::duration(end_##tt - start_##tt)).count()) << endl
#define TIMING_RES2(tt, tt2) cout << #tt << ": " << static_cast<unsigned>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::duration(end_##tt - start_##tt)).count()) << " " << #tt2 << ": " << static_cast<unsigned>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::duration(end_##tt2 - start_##tt2)).count()) << endl

#endif