#include "pch.h"
#include "UtilMath.h"

#include <random>



int64 UtilMath::GetRandom(int64 min, int64 max)
{
	if (min > max)
		return 0;

	std::random_device rd;  // CSPRNG 소스 (구현 의존 but 대부분 안전)
	std::uniform_int_distribution<std::size_t> dist;

	int64 range = max - min + 1;

	int64 rnd = dist(rd) % range;
	return rnd + min;
}