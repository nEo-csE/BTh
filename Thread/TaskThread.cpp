//#include "stdafx.h"
#include "TaskThread.h"

#include <iostream>

UINT __stdcall _BackendTaskThreadEntry( LPVOID param )
{
	Thread::CTaskThread* p=static_cast<Thread::CTaskThread*>(param);
	if (p)
	{
		p->ThreadPrepare();
		p->ThreadMain();
	}
	p->OnThreadQuit();
	return 0;
}


namespace Thread
{
	CTaskThread::CTaskThread( BOOL bStartNow ) : m_taskListLock(3000)
	{
		m_bAlive = FALSE;
		m_bForceQuit = FALSE;
		m_bPaused = TRUE;
		m_hThread = 0;
		m_threadID = 0;
		if (!m_threadEvent.Create(NULL,FALSE,FALSE,NULL))
		{
			ATLASSERT(FALSE && "CreateEvent Fail");
		}
		if (bStartNow)
		{
			Start();
		}
#ifdef _Thread_Record_Performance
		m_performance.MileStone(L"构造");
#endif
	}

	VOID CTaskThread::Start()
	{
		if (!IsAlive())
		{
			if (!m_newTaskAvaliable.Create(NULL,FALSE,FALSE,NULL))
			{
				ATLASSERT(FALSE && "CreateEvent Fail");
			}
			m_hThread = (HANDLE)_beginthreadex(NULL,0,_BackendTaskThreadEntry,this,0,NULL);
			WaitForSingleObject(m_newTaskAvaliable,INFINITE);
		}
	}

	CTaskThread::~CTaskThread( VOID )
	{
#ifdef _Thread_Record_Performance
		m_performance.MileStone(L"开始析构");
#endif
		m_bForceQuit = TRUE;
		m_bAlive = FALSE;
		m_newTaskAvaliable.Set();
		WaitForSingleObject(m_hThread,10000);
#ifdef _Thread_Record_Performance
		m_performance.MileStone(L"析构完成");
#endif
	}

	VOID CTaskThread::OnThreadQuit()
	{
		m_bAlive = FALSE;	// 理论上到这里肯定已经是FALSE了
	}

	VOID CTaskThread::PostEmergentTask( CTask * pTask )
	{
		CAutoCS Guard(m_taskListLock);
		m_taskList.push_front(pTask);
		m_newTaskAvaliable.Set();
	}

	VOID CTaskThread::PostTask( Thread::CTask * pTask )
	{
		CAutoCS Guard(m_taskListLock);
		m_taskList.push_back(pTask);
		m_newTaskAvaliable.Set();
	}

	VOID CTaskThread::ThreadPrepare()
	{
		m_bAlive = TRUE;
		m_bPaused = FALSE;
		m_threadID = GetCurrentThreadId();
		m_newTaskAvaliable.Set();	// 通知线程准备结束
	}

	VOID CTaskThread::ThreadMain()
	{
		BOOL bQuit;
		Thread::CTask *p;
		TaskList::iterator i;
		do 
		{
			bQuit = FALSE;
			while(!bQuit)
			{
				p = NULL;
				m_taskListLock.Enter();

				i = m_taskList.begin();
				if (i == m_taskList.end())
				{	// 若任务处理光了
					bQuit = TRUE;
				}
				else
				{
					p = *i;
					m_taskList.pop_front();
				}
				m_taskListLock.Leave();
				if (p)
				{
					p->Run();	//执行任务
					delete p;
				}
			}
			do
			{
				WaitForSingleObject(m_newTaskAvaliable,INFINITE);
			}while (m_bPaused && m_bAlive);
			if (!m_bAlive)
			{
				if (m_bForceQuit)
				{
					break;
				}
			}
		} while (m_bAlive);
	}

	INT CTaskThread::WaitingTaskNumber()
	{
		return static_cast<INT>(m_taskList.size());
	}

	VOID CTaskThread::Pause()
	{
		m_bPaused = TRUE;
	}

	VOID CTaskThread::Resume()
	{
		m_bPaused = FALSE;
		m_newTaskAvaliable.Set();
	}

	VOID CTaskThread::Stop( BOOL bWait/*=FALSE*/ )
	{
		m_bAlive = FALSE;
	}

}