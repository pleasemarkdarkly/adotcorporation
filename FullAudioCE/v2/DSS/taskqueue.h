// TaskQueue.h: interface for the TaskQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKQUEUE_H__C9F82097_19E6_11D3_A181_00A0C9777D6E__INCLUDED_)
#define AFX_TASKQUEUE_H__C9F82097_19E6_11D3_A181_00A0C9777D6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(_WIN32_WCE)
#include <windows.h>
#elif defined(WIN32) && !defined(_WIN32_WCE_AUTOPC)
#include <process.h>
#endif
#include "TQueue.h"
#include "Dss.h"
#include "CriticalSection.h"

template <class _Node>
class TaskQueue : public TQueue<_Node>
{
public:
	typedef void (ProcessTaskFunc)(const _Node*);

	TaskQueue(ProcessTaskFunc*);
	virtual ~TaskQueue();

#ifdef _WIN32_WCE_AUTOPC
	static TRESULT __cdecl ThreadFunc(void* pvParam);
#else
	static TRESULT _stdcall ThreadFunc(void* pvParam);
#endif

	virtual void ClearQueue() 
	{
		CLock lock(&m_cs);
		TQueue<_Node>::ClearQueue();
	}
	
	virtual DWORD Push(const _Node& node)
	{
		CLock lock(&m_cs);
		DWORD size;
		size = TQueue<_Node>::Push(node);
		if(size > 0)
			SetEvent(m_hEvtWorkToDo);
		return size;
	}

	virtual DWORD Pop(_Node** ppNode)
	{
		CLock lock(&m_cs);
		DWORD size;
		size = TQueue<_Node>::Pop(ppNode);
		if(size == 0 || size == 0xFFFFFFFF)
			ResetEvent(m_hEvtWorkToDo);
		return size;
	}

	virtual DWORD Size() const
	{
		CLock lock(const_cast<CCriticalSection*>(&m_cs));
		return TQueue<_Node>::Size();
	}


protected:
	CCriticalSection m_cs;
	HANDLE m_hEvtWorkToDo;
	HANDLE m_hEvtTerminateThread;
	HANDLE m_hThread;
	TRESULT m_ThreadId;
	ProcessTaskFunc* m_pFunc;
};

template <class _Node>
TaskQueue<_Node>::TaskQueue(ProcessTaskFunc* pFunc) : m_pFunc(pFunc)
{
	m_hEvtWorkToDo = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hEvtTerminateThread = CreateEvent(NULL,TRUE,FALSE,NULL);

#if defined(THREAD_USES_CRT)
	m_hThread = (HANDLE) _beginthreadex(NULL, //security
#else
	m_hThread = CreateThread(NULL,
#endif
				   200*1024, //thread stack size
				   TaskQueue<_Node>::ThreadFunc, //starting point
				   reinterpret_cast<void*>(this),
				   0,
				   &m_ThreadId);
}

template <class _Node>
TaskQueue<_Node>::~TaskQueue()
{
	SetEvent(m_hEvtTerminateThread);
	ClearQueue();

	if(WAIT_TIMEOUT == WaitForSingleObject(m_hThread,2000))
		TerminateThread(m_hThread,42);

	CloseHandle(m_hEvtWorkToDo);
	CloseHandle(m_hEvtTerminateThread);
	CloseHandle(m_hThread);
}

template <class _Node>
#ifdef _WIN32_WCE_AUTOPC
TRESULT _cdecl TaskQueue<_Node>::ThreadFunc(void* pvParam)
#else
TRESULT _stdcall TaskQueue<_Node>::ThreadFunc(void* pvParam)
#endif
{
	DWORD dwRes;
	TaskQueue<_Node>* pThis = reinterpret_cast<TaskQueue<_Node>* >(pvParam);
	HANDLE Events[2];
	Events[0] = pThis->m_hEvtTerminateThread;
	Events[1] = pThis->m_hEvtWorkToDo;

	for(;;)
	{
		dwRes = WaitForMultipleObjects(2,Events,FALSE,INFINITE);
		if((dwRes - WAIT_OBJECT_0) == 0) //m_hEvtTerminateThread
		{
			break;
		}
		else if((dwRes - WAIT_OBJECT_0) == 1) //m_hEvtWorkToDo
		{
			_Node* pNode;
			//Pop the Queue and process commands
			dwRes = pThis->Pop(&pNode);
			if(dwRes != 0xFFFFFFFF && pNode)
			{
				(*(pThis->m_pFunc))(pNode);
				delete pNode;
			}
		}
		else //Timeout or Abandoned
			break;
	}

	return 42;
}

#endif // !defined(AFX_TASKQUEUE_H__C9F82097_19E6_11D3_A181_00A0C9777D6E__INCLUDED_)
