#include "ThreadHotel.h"

namespace Thread
{\

CThreadHotel::CThreadHotel()
{

}

Thread::CThreadHotel::~CThreadHotel()
{
	CAutoCS ac(m_cs);
	ThreadMap::iterator i;
	for (i=m_threads.begin(); i!=m_threads.end(); ++i)
	{
		CTaskThread* pThread=i->second;
		if (pThread)
		{
			pThread->SetForceQuit(TRUE);
			pThread->Stop(TRUE);
			delete pThread;
		}
	}
}

ThreadID CThreadHotel::CreateThread( LPCTSTR threadName )
{
	ThreadID id = _GetThreadIDByName(threadName);
	if (id == InvalidThreadID)
	{
		CTaskThread* taskThread = _AddThread(threadName);
		if (taskThread)
		{
			taskThread->SetName(threadName);
		}
	}
	return id;
}

CString CThreadHotel::GetThreadName( ThreadID threadID )
{
	CTaskThread* taskThread = NULL;
	taskThread = _GetThreadPtr(threadID);
	if (taskThread)
	{
		return taskThread->GetName();
	}
	return _T("");
}

VOID CThreadHotel::KillThread( ThreadID threadID,BOOL forceExit )
{
	CAutoCS ac(m_cs);
	CTaskThread* taskThread = _GetThreadPtr(threadID);
	if (taskThread)
	{
		taskThread->SetForceQuit(forceExit);
		pThread->Stop(TRUE);
		delete taskThread;
	}
	{
		m_threads.erase(threadID);
	}
}

CTaskThread* CThreadHotel::_GetThreadPtr( ThreadID threadID )
{
	Thread::CAutoCS ac(m_cs);
	ThreadMap::const_iterator i = m_threads.find(threadID);
	if (i != m_threads.end())
	{
		return i->second;
	}
	return NULL;
}

CTaskThread* CThreadHotel::_AddThread( LPCTSTR threadName )
{
	ThreadID id = _GetThreadIDByName(threadName);
	CTaskThread* pThread = InvalidThreadID;
	if (id == InvalidThreadID)
	{
		Thread::CAutoCS ac(m_cs);
		pThread = new CTaskThread(TRUE);
		pThread->SetName(threadName);
		id = pThread->GetThreadID();
		m_threads[id] = pThread;
	}
	else
	{
		ATLASSERT(FALSE && "thread already exist");
		pThread = _GetThreadPtr(id);
	}
	return pThread;
}

Thread::ThreadID CThreadHotel::_GetThreadIDByName( LPCTSTR threadName )
{
	CAutoCS ac(m_cs);
	ThreadMap::const_iterator i;
	for (i=m_threads.begin(); i!=m_threads.end(); ++i)
	{
		CTaskThread* pThread = i->second;
		ATLASSERT(pThread);
		if (pThread)
		{
			if (pThread->GetName().Compare(threadName) == 0)
			{
				return pThread->GetThreadID();
			}
		}
	}
	return InvalidThreadID;
}

size_t CThreadHotel::GetThreadCount()
{
	return m_threads.size();
}

BOOL CThreadHotel::PostThreadTask( ThreadID threadID,CTask* pTask,BOOL emergent/*=FALSE*/,CTask* finishTask/*=NULL*/ )
{
	CTaskThread* pThread = _GetThreadPtr(threadID);
	if (pThread)
	{
		if (emergent)
		{
			pThread->PostEmergentTask(pTask);
		}
		else
		{
			pThread->PostTask(pTask);
		}
		pThread->PostTask(finishTask);
		return TRUE;
	}
	return FALSE;
}

}