#include "pch.h"
#include "IocpCore.h"

#include "IocpEvent.h"

#include <chrono>


///////////////////////////////////////////////////////
//	<	IocpCore	>
///////////////////////////////////////////////////////
IocpCore::IocpCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);

	::QueryPerformanceFrequency(&_freq);
}

IocpCore::~IocpCore()
{
	if (_iocpHandle != INVALID_HANDLE_VALUE)
		::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(IocpObjectRef iocpObject)
{
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, /*key*/0, 0);
}

bool IocpCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;
	IocpEvent* iocpEvent = nullptr;

	auto Process = [&](IocpEvent* event, DWORD bytes) {

		LARGE_INTEGER start; ::QueryPerformanceCounter(&start);
		IocpObjectRef iocpObject = event->owner;
		iocpObject->Dispatch(event, bytes);

		if (GServerStats)
		{
			LARGE_INTEGER end; ::QueryPerformanceCounter(&end);
			int64 spendTime = end.QuadPart - start.QuadPart;
			GServerStats->iocp.iocpCallCount.fetch_add(1);
			GServerStats->iocp.totalProcessTImeUs.fetch_add((spendTime * 1000000 / _freq.QuadPart));
		}
	};

	if (::GetQueuedCompletionStatus(_iocpHandle, OUT & numOfBytes, /* key */OUT & key, OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
	{
		Process(iocpEvent, numOfBytes);
	}
	else
	{
		const int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			IocpObjectRef iocpObject = iocpEvent->owner;
			iocpObject->HandleError(iocpEvent, errCode);
			break;
		}
	}
	return true;
}
