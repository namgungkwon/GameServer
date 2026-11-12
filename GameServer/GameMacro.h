#pragma once

using CallBack = function<void(void)>;

///////////////////////////////////////////////////////
//	<	UseRef	>
///////////////////////////////////////////////////////
useRef_Class(GameSession)

useRef_Class(GameScene)
useRef_Class(Zone)
useRef_Struct(MoveResult)
useRef_Struct(SceneRequest)

useRef_Class(Entity)
useRef_Class(Player)
useRef_Class(NPC)
useRef_Class(Monster)


///////////////////////////////////////////////////////
//	<	GetSet	>
///////////////////////////////////////////////////////
#define GETSET(type, name, funcName)								\
public:																\
	void Set##funcName(type name) { this->name = name; }			\
	type Get##funcName() const { return name; }						\
private:															\
	type name;


#define GETSET_SHARED(type, name, funcName)							\
public:																\
	void Set##funcName(type##Ref name) { this->name = name; }		\
	type##Ref Get##funcName() const { return name; }				\
private:															\
	type##Ref name;


#define GETSET_WEAK(type, name, funcName)							\
public:																\
	void Set##funcName(type##Ref name) { this->name = name; }		\
	type##Ref Get##funcName() const { return name.lock(); }			\
private:															\
	std::weak_ptr<class type> name;


// 문자열 등 원본 참조로 전달하고 싶을 때 사용
#define GETSET_REF(type, name, funcName)							\
public:																\
	void Set##funcName(const type##& name) { this->name = name; }	\
	type##& Get##funcName() { return name; }						\
private:															\
	type name;


#define GET(type, name, funcName)									\
public:																\
	type Get##funcName() const { return name; }						\
private:															\
	type name;


#define GET_REF(type, name, funcName)								\
public:																\
	type##& Get##funcName() { return name; }						\
private:															\
	type name;