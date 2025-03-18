#pragma once
#include "CoreTypes.h"

// Multiple Producer Single Consumer를 가정해서 작성
// 다수가 작업하고 한명이 빼오는 과정
class CORE_API FJobQueue
{
public:
	void Push(std::function<void()> NewJob)
	{
		if (!NewJob)
		{
			_ASSERT(false);
			return;
		}

		//task push
		{
			std::lock_guard<std::mutex> Lock(Mutex);
			Jobs.push(std::move(NewJob));
		}
		CV.notify_one();//thread wait 이면 wake하는 과정

	}

	std::function<void()> Pop()
	{
		//task pull
		std::unique_lock<std::mutex> Lock(Mutex);
		//if (Jobs.empty())
		//{
		//	return nullptr;
		//}
		
		//lock 대상, 조건 람다 처리로 하는중
		//조건식이 false면 wait이 된다.. 즉 jobs가 empty면 wait이 되고 작업이 추가 되면 wait이 풀려서 실행이 됨
		//unlock을 하고 재운 다음 다른 스렝드에서 작업 계속 하는게 들어와서 추가가 되는거임
		CV.wait(Lock, [this] {return !Jobs.empty(); });


		std::function<void()> Job = std::move(Jobs.front());
		Jobs.pop();
		return Job;

	}

	bool Empty() const 
	{ 
		//
		std::lock_guard<std::mutex> Lock(Mutex);
		return Jobs.empty(); 
	}

	bool IsShutdown() const
	{
		return bShutdown;
	}

	void Shutdown() 
	{
		bShutdown = true;
	}


private:
	bool bShutdown = false;
	//mutable 함수 뒤에 this*가 const일때 사용 가능 일종의 const mutex라고 생각 하면 될거 같음
	mutable std::mutex	Mutex; //lock unlock
	//조건 변수 사용 관련
	std::condition_variable CV;
	std::queue<std::function<void()>> Jobs;

};