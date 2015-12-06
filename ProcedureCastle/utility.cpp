#include "debug.h"
#include "utility.h"
#include <math.h>
#include <fstream>

std::string textFileToString(const char* _filename)
{
	std::string fileString;
	std::string curLine;
	std::ifstream fileStream;

	fileStream = std::ifstream(_filename, std::ios::in);
	if (fileStream.is_open())
	{
		fileString = "";
		curLine = "";
		while (getline(fileStream, curLine)) fileString += "\n" + curLine;
	}
#if DEBUG_SHOULD_ERROR_CHECK
	else
	{
		DEBUG_ERROR("Cannot open file.");
	}
#endif

	return fileString;
}

uint64_t rndState[2] = { 0x5ec50476f4102274, 0xa50817f914a268a5 };

double rnd(void)
{
	uint64_t	   x = rndState[0];
	uint64_t const y = rndState[1];
	rndState[0] = y;
	x ^= x << 23;
	x ^= x >> 17;
	x ^= y ^ (y >> 26);
	rndState[1] = x;
	return (double)(x + y) / UINT64_MAX;
}

void randomize(const uint64_t _s)
{
	int i;

	rndState[0] = _s;
	rndState[1] = 0;

	for (i = 0; i < 16; i++) rnd();
}

bool rnd_oneInXChance(const int _chance)
{
	return ((int)(rnd() * _chance)) == 0 ? true : false;
}
int rnd_intRange(const int _a, const int _b)
{
	return ((int)(rnd() * (_b - _a + 1))) + _a;
}
double rnd_floatRange(const double _a, const double _b)
{
	return rnd()*(_b - _a) + _a;
}

int roundNearest(double _x)
{
	return (int) floor(_x + 0.5);
}

uint64_t getTimeMs64()
{
#ifdef _WIN32
	FILETIME ft;
	LARGE_INTEGER li;
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	uint64_t ret = li.QuadPart;
	ret -= 116444736000000000LL;
	ret /= 10000; 
	return ret;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	uint64 ret = tv.tv_usec;
	ret /= 1000;
	ret += (tv.tv_sec * 1000);
	return ret;
#endif
}