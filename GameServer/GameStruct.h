#pragma once

struct Vector3
{
    Vector3() = default;
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {};

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

    Vector3 operator+(const Vector3& rhs) const { return { x + rhs.x, y + rhs.y, z + rhs.z }; }
    Vector3 operator-(const Vector3& rhs) const { return { x - rhs.x, y - rhs.y, z - rhs.z }; }
    Vector3 operator*(float s)          const { return { x * s,   y * s,   z * s }; }
    Vector3 operator/(float s)          const { return { x / s, y / s, z / s }; }
    bool    operator==(const Vector3& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }

    float LengthSq() const { return x * x + y * y + z * z; }
    float Length() const { return ::sqrt(LengthSq()); }

    float DistanceSq(const Vector3& rhs) const { return (*this - rhs).LengthSq(); }
    float Distance(const Vector3& ths)   const { return (*this - ths).Length(); }

    Vector3 Normalized() const
    {
        float len = Length();
        if (len < 1e-6f) return { 0.f, 0.f, 0.f };
        return { x / len, y / len, z / len };
    }

    bool IsZero() const { return LengthSq() < 1e-12f; }

    float Dot(const Vector3& o) const { return x * o.x + y * o.y + z * o.z; }
};

struct PlayerSummaryData
{
    uint64 dbId = 0;
    uint64 playerId = 0;
    wstring name;
    int32 templateId = 0;
    int32 level = 1;
};

struct PlayerLoadData
{
    Vector3 location;
    float yaw = 0.f;
    int32 hp = 0;
    int32 mp = 0;
    int64 exp = 0;
};


struct PlayerData
{
    int32 id = 0;
    int32 level = 1;
    int32 maxHp = 200;
    int32 maxMp = 100;
    int32 attack = 10;
    int32 defense = 5;
    int64 requireExp = 100;
    float speed = 5.f;
};

struct MonsterData
{
    int32   id = 0;
    int32   level = 1;
    wstring name;
    int32   maxHp = 0;
    int32   attack = 0;
    int32   defense = 0;
    int64   rewardExp = 0;
    float   speed = 0.f;
    float   searchRange = 0.f;
    float   maxSearchRange = 0.f;
    float   attackRange = 0.f;
    uint64  attackDelayMs = 0.f;
};

struct SpawnData
{
    int32 monsterId = 0;
    Vector3 location = Vector3();
    uint64 respawnMs = 5.f;
    uint64 spawnCount = 1;
    float patrolRadius = 300.f;
};

struct SkillData
{
    int32 id = 0;
    int32 level = 1;
    float attackRate = 1.0f;
    float attackRange = 10.0f;
    float attackAngle = 90.0f;
    uint64 cooldownMs = 1000;
    uint64 moveDelayMs = 500;
};