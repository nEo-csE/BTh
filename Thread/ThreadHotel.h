#include "TaskThread.h"
#include <map>

namespace Thread
{ \

class CThreadHotel
{
public:
	CThreadHotel();
	~CThreadHotel();
public:
	ThreadID CreateThread	(LPCTSTR threadName);
	size_t	GetThreadCount	();
	BOOL	PostThreadTask	(ThreadID threadID,CTask* pTask,BOOL emergent=FALSE,CTask* finishTask=NULL);
	CString GetThreadName	(ThreadID threadID);
	VOID	KillThread		(ThreadID threadID,BOOL forceExit=FALSE);
private:
	CTaskThread* _AddThread		(LPCTSTR threadName);
	CTaskThread* _GetThreadPtr	(ThreadID threadID);
	ThreadID	_GetThreadIDByName	(LPCTSTR threadName);
	VOID		_DeleteThread	(ThreadID threadID);
	typedef std::map<ThreadID,CTaskThread*>	ThreadMap;

	ATL::CCriticalSection m_cs;
	ThreadMap	m_threads;

	DISALLOW_COPY_AND_ASSIGN(CThreadHotel);
};

}