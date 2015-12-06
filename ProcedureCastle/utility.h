#pragma once
#include <stdint.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#include <ctime>
#endif
#include <string>

double rnd(void);
bool rnd_oneInXChance(const int _chance);
int rnd_intRange(const int _a, const int _b);
double rnd_floatRange(const double _a, const double _b);
void randomize(const uint64_t _s);
int roundNearest(double _x);

template <typename T>
int sign(T val) {
	return (T(0) < val) - (val < T(0));
}

std::string textFileToString(const char* _filename);

uint64_t getTimeMs64();
