#pragma once
#include <queue>
#include <Windows.h>
#include "ITask.h"

class ThreadPool
{
	public:
	ThreadPool(int maxThread);
	~ThreadPool();

	static DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter);
	static DWORD WINAPI ThreadManagerProc(_In_ LPVOID lpParameter);

	void AddWork(ITask* task);

	protected:
	std::queue<ITask*> m_taskQueue;
	std::vector<HANDLE> m_vecThread;
	HANDLE m_managerThread;

	CRITICAL_SECTION m_csTaskQueue;
	HANDLE m_semaphore;
	HANDLE m_event;
	bool m_stop;
};

