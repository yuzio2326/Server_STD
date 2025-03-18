#include "ServerGameMode.h"
#include <iostream>

void PrintThreadID()
{
	std::stringstream ss;
	ss << std::this_thread::get_id();
	string str = "Thread ID: " + ss.str() + "\n";
	cout << str;
}

AServerGameMode::AServerGameMode()
{
}

void ThreadMainAddLocalVar(const char* Name)
{
	PrintThreadID();
	// Thread마다 Stack이 별도로 존재하기 때문에 문제가 없다
	int64 a = 0;
	for (int64 i = 0; i <= 100000; ++i)
	{
		a += i;
	}

	// 동일한 값 2개가 출력 된다
	string Str = std::format("{} a : {}\n", Name, a);
	cout << Str;
}

int64 GA = 0;
void ThreadMainAddGlobalVar_NoLock(const char* Name)
{
	PrintThreadID();
	for (int64 i = 0; i <= 100000; ++i)
	{
		GA += i;
	}
}

std::mutex Mutex;

void ThreadMainAddGlobalVar_Lock(const char* Name)
{
	PrintThreadID();
	// lock을 이런식으로 쓰면 Thread를 안쓰는 쪽이 더 빠르다
	// (무식하게 lock을 많이 걸면 성능이 떨어진다)
	for (int64 i = 0; i <= 100000; ++i)
	{
		// 생성자에서 lock 걸고, 소멸자에서 unlock 처리
		std::lock_guard<std::mutex> AutoLock(Mutex);
		//Mutex.lock();
		GA += i;
		//Mutex.unlock();
	}
}

std::atomic_uint64_t GAtomic = 0;

void ThreadMainAddGlobalVar_Atomic(const char* Name)
{
	PrintThreadID();
	for (int64 i = 0; i <= 100000; ++i)
	{
		// Atomic 변수는 원자성이 보장 되기 때문에 여러 Thread에서 접근 해도 문제가 없다
		//GAtomic += i;
		GAtomic.fetch_add(i);
	}
}

void AServerGameMode::BeginPlay()
{
	Super::BeginPlay();
	{
		// jthread는 내가 join하지 않아도 자동으로 소멸자에서 join 해준다

		Thread = std::jthread(ThreadMainAddLocalVar, "Thread");
		Thread2 = std::jthread(ThreadMainAddLocalVar, "Thread2");

		// join은 해당 Thread가 종료될 때 까지 main 함수에서 기다린다
		Thread.join();
		Thread2.join();
	}

	{
		Thread = std::jthread(ThreadMainAddGlobalVar_NoLock, "Thread");
		Thread2 = std::jthread(ThreadMainAddGlobalVar_NoLock, "Thread2");

		Thread.join();
		Thread2.join();

		// 의도와 다르게 값이 출력 된다.
		// 변수 이름으로 접근 가능한 Stack 메모리가 아닌 메모리 영역은 다른 Thread들이 공유하는 자원입니다.
		// 메모리를 읽고 쓰는(Read Write) 과정은
		// 1. 메모리에서 데이터를 Read해서 Cache(및 레지스터)로 올린다
		// 2. Cache(및 레지스터)에서 값을 수정한다
		// 3. 수정한 값을 메모리에 Write 한다
		// 와 같은 단계로 나뉘어져 있기 때문에
		// 동시에 이 과정이 발생하면서 의도와 다르게 값이 저장된다
		string Str = std::format("GA : {}\n", GA);
		cout << Str;
	}

	// 그렇기 때문에 공유 자원에 안전하게 접근 하기 위해서는 동기화 과정이 필요하다
	// Mutex를 사용하면 하나의 Thread만 해당 영역에 접근 할 수 있다.
	// 나머지는 lock에서 멈춰 있다
	// 멈추는 방법은 커널에 재워달라고 하는 요청이 있고
	// 반복문을 돌면서 유저 영역에서 기다리는 방법이 있다(SpinLock)
	{
		GA = 0;
		Thread = std::jthread(ThreadMainAddGlobalVar_Lock, "Thread");
		Thread2 = std::jthread(ThreadMainAddGlobalVar_Lock, "Thread2");
		Thread.join();
		Thread2.join();
		string Str = std::format("GA : {}\n", GA);
		cout << Str;
	}	
	{
		Thread = std::jthread(ThreadMainAddGlobalVar_Atomic, "Thread");
		Thread2 = std::jthread(ThreadMainAddGlobalVar_Atomic, "Thread2");
		Thread.join();
		Thread2.join();
		string Str = std::format("Atomic : {}\n", GAtomic.load());
		cout << Str;
	}

	{
		auto Sum = [](int32 Start, int32 End) -> int64
			{
				PrintThreadID();
				int64 Result = 0;
				for (int64 i = Start; i <= End; ++i)
				{
					Result += i;
				}
				return Result;
			};

		int32 StartValue = 0;
		int32 EndValue = 100000;
		int32 NumThreads = std::thread::hardware_concurrency() - 1;
		cout << "NumThread: " << NumThreads << std::endl;

		const int64 RangePerThread = (EndValue - StartValue) / NumThreads;
		TArray<std::future<int64>> Futures; Futures.reserve(NumThreads);

		for (int32 i = 0; i < NumThreads; ++i)
		{
			const int64 Start = StartValue + i * RangePerThread;
			const int64 End = (i == NumThreads - 1) ? EndValue : Start + RangePerThread - 1;
			std::future<int64> Result = std::async(std::launch::async, Sum, Start, End);

			Futures.push_back(move(Result));
		}

		int64 TotalSum = 0;
		for (std::future<int64>& Future : Futures)
		{
			TotalSum += Future.get();
		}
		string Str = std::format("Future: {}\n", TotalSum);
		cout << Str;
	}
}

void AServerGameMode::Tick(float DeltaSceonds)
{
	Super::Tick(DeltaSceonds);
	/*if (GetAsyncKeyState(VK_F2) & 0x8000)
	{
		RequestEngineExit(TEXT("Test"));
	}*/
}
