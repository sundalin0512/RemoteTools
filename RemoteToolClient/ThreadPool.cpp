#include "stdafx.h"
#include "ThreadPool.h"

ThreadPool::ThreadPool(int maxThread)
	: m_stop(false)
{
	InitializeCriticalSection(&m_csTaskQueue);
	m_semaphore = CreateSemaphore(NULL, 0, maxThread, NULL);
	m_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_managerThread = CreateThread(NULL, 0, ThreadManagerProc, this, 0, NULL);
	m_vecThread.resize(maxThread);
	for(int i = 0; i < maxThread; i++)
	{
		m_vecThread[i]= CreateThread(NULL, 0, ThreadProc, this, 0, NULL);
	}
}

ThreadPool::~ThreadPool()
{
	DeleteCriticalSection(&m_csTaskQueue);
	CloseHandle(m_semaphore);
}

DWORD ThreadPool::ThreadProc(LPVOID lpParameter)
{
	ThreadPool& thisThreadPool = *static_cast<ThreadPool*>(lpParameter);
	while(!thisThreadPool.m_stop)
	{
		WaitForSingleObject(thisThreadPool.m_event, INFINITE);
		EnterCriticalSection(&thisThreadPool.m_csTaskQueue);
		ITask* pTask = thisThreadPool.m_taskQueue.front();
		thisThreadPool.m_taskQueue.pop();
		if(!thisThreadPool.m_taskQueue.empty())
		{
			SetEvent(thisThreadPool.m_event);
		}
		LeaveCriticalSection(&thisThreadPool.m_csTaskQueue);
		pTask->Run();
		delete pTask;
	}
	return 0;
}

DWORD ThreadPool::ThreadManagerProc(LPVOID lpParameter)
{
	return 0;
}

void ThreadPool::AddWork(ITask* task)
{
	EnterCriticalSection(&m_csTaskQueue);
	m_taskQueue.push(task);
	SetEvent(m_event);
	LeaveCriticalSection(&m_csTaskQueue);
}
