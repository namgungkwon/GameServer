#pragma once

struct Vector3
{
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
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
    float   attackSpeed = 0.f;
};

struct SpawnData
{
    int32 monsterId = 0;
    Vector3 location = Vector3();
    uint64 respawnMs = 5.f;
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