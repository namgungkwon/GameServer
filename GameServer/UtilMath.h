#pragma once

#include "Protocol.pb.h"
#define PI (3.1415926535)

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

	static int64 GetRandom(int64 min, int64 max);

	static inline float GetYawFromVector(Vector3 vector)
	{
		if (vector.IsZero())
			return 0.f;

		const float yawRadian = std::atan2(vector.y, vector.x);

		float yawDegree = yawRadian * (180 / PI);

		if (yawDegree < 0.f)
			yawDegree += 360.f;

		return yawDegree;
	}

	static inline Vector3 GetForwardFromYaw(float yaw)
	{
		return Vector3{ std::cos(yaw), std::sin(yaw), 0.f };
	}

	static Vector3 ToServerLocation(const Protocol::Vector3& from);
	static Protocol::Vector3 ToProtoLocation(const Vector3& from);
};
