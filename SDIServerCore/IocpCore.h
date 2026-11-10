#pragma once


///////////////////////////////////////////////////////
//	<	IocpObject	>
///////////////////////////////////////////////////////
class IocpObject : public enable_shared_from_this<IocpObject>
{
	friend class IocpCore;
private:
	virtual HANDLE			GetHandle() abstract;
	virtual void			Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;
	virtual void			HandleError(const IocpEvent* iocpEvent, const int32 errorCode) abstract;
};



///////////////////////////////////////////////////////
//	<	IocpCore	>
///////////////////////////////////////////////////////
class IocpCore
{
public:
	IocpCore();
	~IocpCore();

public:
	HANDLE			GetHandle() { return _iocpHandle; }

public:
	bool			Register(IocpObjectRef iocpObject);
	bool			Dispatch(uint32 timeoutMs = INFINITE);

private:
	HANDLE			_iocpHandle;
};