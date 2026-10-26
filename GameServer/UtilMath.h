#pragma once

class UtilMath
{
public:
	template<typename T>
	static inline T Max(T a, T b) { return a > b ? a : b; }

	template<typename T>
	static inline T Min(T a, T b) { return a < b ? a : b; }

	template<typename T>
	static inline T Abs(T a) { return a < 0 ? -a : a; }

	template<typename T>
	static inline T Clamp(T a, T min, T max) { if (a < min) return a; if (a > max) return max; return a; }

	static inline int64 GetRandom(int64 min, int64 max);
};
