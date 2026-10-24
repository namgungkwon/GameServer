#pragma once

#include <functional>


using CallbackType = std::function<void()>;

///////////////////////////////////////////////////////
//	<	Job	>
///////////////////////////////////////////////////////
class Job
{
public:
	Job(CallbackType&& callback) : _callback(std::move(callback))
	{

	}

	template<typename T, typename Ret, typename... Args>
	Job(shared_ptr<T> owner, Ret(T::* memfunc)(Args...), Args&&... args)
	{
		_callback = [owner, memfunc, args...]()
			{
				(owner.get()->*memfunc)(args...);
			};
	}

	void Execute()
	{
		_callback();
	}

private:
	CallbackType _callback;
};

