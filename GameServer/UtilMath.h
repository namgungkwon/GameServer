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

	static inline Vector3 GetForwardVectorFromYaw(float yaw)
	{
		const float yawRadian = yaw * (PI / 180);
		return Vector3{ std::cos(yawRadian), std::sin(yawRadian), 0.f };
	}

	static inline Vector3 GetMoveDirectionFromInput(const Vector3 input, float yaw)
	{
		const Vector3 forward = GetForwardVectorFromYaw(yaw);
		const Vector3 right = Vector3{ -forward.y, forward.x, 0.f };

		Vector3 dir;
		dir.x = forward.x * input.x + right.x * input.y;
		dir.y = forward.y * input.x + right.y * input.y;
		dir.z = 0.f;

		if (dir.LengthSq() > 0.f)
			dir = dir.Normalized();

		return dir;
	}

	static Vector3 ToServerLocation(const Protocol::Vector3& from);
	static Protocol::Vector3 ToProtoLocation(const Vector3& from);
};
