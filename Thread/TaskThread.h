#pragma once
#include "ThreadDefine.h"
#include <queue>
#include <list>
#include <atlstr.h>
#include <atlsync.h>
#include <Windows.h>
#ifdef _Thread_Record_Performance
#include "Performance.h"
#endif

namespace Thread
{
	typedef DWORD64 ThreadID;
	#define InvalidThreadID	0

	class CTaskThread
	{
	public:
		CTaskThread(BOOL bStartNow=TRUE);
		~CTaskThread(VOID);

	public:
		VOID Start();
		VOID Pause();
		VOID Resume();
		VOID Stop(BOOL bWait=FALSE);
		VOID SetForceQuit(BOOL bForceSuspendThreadWhenDestruct){m_bForceQuit = bForceSuspendThreadWhenDestruct;};
		HANDLE GetThreadHandle() const { return m_hThread; }
		DWORD GetThreadID() const {return m_threadID;}
		BOOL IsAlive() const { return m_bAlive; }

		// 请保证线程已经被启动！(构造函数参数true，或调用过Start)
		VOID PostEmergentTask(CTask * pTask);
		VOID PostTask(CTask * pTask);
		INT WaitingTaskNumber();

		VOID SetName(LPCTSTR name){m_threadName=name;}
		CString GetName(){return m_threadName;}
		VOID OnThreadQuit();
		VOID ThreadPrepare();
		VOID ThreadMain();
	private:
		typedef std::list<CTask*>	TaskList;
		CString	m_threadName;
		ATL::CCriticalSection m_taskListLock;
		volatile BOOL m_bAlive;
		volatile BOOL m_bForceQuit;
		volatile BOOL m_bPaused;
		HANDLE m_hThread;
		DWORD m_threadID;
		ATL::CEvent m_newTaskAvaliable;
		ATL::CEvent m_threadEvent;
		TaskList m_taskList;

#ifdef _Thread_Record_Performance
		Performance::CPerformance m_performance;
#endif
	private:
		DISALLOW_COPY_AND_ASSIGN(CTaskThread);
	};
}