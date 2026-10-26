#pragma once

#include <typeindex>
#include <typeinfo>
#include "Protocol.pb.h"

enum EntityState
{
	Idle,
	Move,
	Chase,
	Attack,
	Dead,
};

class Entity;
class IObjectComponent
{
public:
	IObjectComponent(EntityRef& owner) : _owner(owner) {};
	virtual ~IObjectComponent() = default;
	virtual void OnAttach(Entity& owner) {}
	virtual void OnDetach(Entity& owner) {}

protected:
	std::weak_ptr<Entity> _owner;
};

class IdGenerator
{
public:
	static uint64 Get() { return id.fetch_add(1); }
private:
	static Atomic<uint64> id;
};

class Entity : public enable_shared_from_this<Entity>
{
public:
	template <typename T>
	T* AddComponent(EntityRef owner);
	template<typename T>
	T* GetComponent();

public:
	virtual void Respawn(uint64 nowMs);

public:
	void MakePkt(OUT Protocol::ObjectData& objectData);

public:
	void MakePkt(OUT Protocol::ObjectSummaryData& summary);
	void MakePkt(OUT Protocol::ObjectPosData& pos);
	void MakePkt(OUT Protocol::ObjectStatData& stat);

private:
	HashMap<std::type_index, IObjectComponent*> Components;

public:
	GETSET_WEAK(Zone,						_ownerZone,		Zone);
	GETSET_WEAK(GameScene,					_ownerScene,	Scene);

	GETSET		(uint64,					_objectId,		ObjectId);
	GETSET		(uint64,					_templateId,	TemplateId);
	
	GETSET_REF	(wstring,					_name,			Name);
	GETSET		(int32,						_level,			Level);
	GETSET		(int32,						_exp,			Exp);
	GETSET		(Vector3,					_location,		Location);
	GETSET		(float,						_yaw,			Yaw);

	GETSET		(Protocol::ObjectCategory,	_type,			Type);
	GETSET		(EntityState,				_state,			State);
};

template<typename T>
inline T* Entity::AddComponent(EntityRef owner)
{
	std::type_index idx = std::type_index(typeid(T));
	if (Components.find(idx) == Components.end())
	{
		Components.insert(make_pair(idx, cnew<T>(owner)));
	}

	return static_cast<T*>(Components[idx]);
}

template<typename T>
inline T* Entity::GetComponent()
{
	std::type_index idx = std::type_index(typeid(T));
	if (Components.find(idx) == Components.end())
		return nullptr;
	return static_cast<T*>(Components[idx]);
}

