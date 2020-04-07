// CriticalSection.h: interface for the CCriticalSection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRITICALSECTION_H__C9F82098_19E6_11D3_A181_00A0C9777D6E__INCLUDED_)
#define AFX_CRITICALSECTION_H__C9F82098_19E6_11D3_A181_00A0C9777D6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
#include <winbase.h>
#endif

class LockableObject
{
public:
	LockableObject() {}
	virtual ~LockableObject() {}
	virtual void Lock() = 0;
	virtual void Unlock() = 0;
};

class CCriticalSection : public LockableObject
{
public:
	CCriticalSection() { InitializeCriticalSection(&m_CritSect); }
	virtual ~CCriticalSection() { DeleteCriticalSection(&m_CritSect); }
	virtual void Lock() { EnterCriticalSection(&m_CritSect); }
	virtual void Unlock() { LeaveCriticalSection(&m_CritSect); }
	
private:
	CRITICAL_SECTION m_CritSect;
};

class CLock
{
public:
	CLock(LockableObject* pLockable) : m_pLockable(pLockable) { m_pLockable->Lock(); }
	~CLock() { m_pLockable->Unlock(); }

private:
	LockableObject* m_pLockable;
};

#endif // !defined(AFX_CRITICALSECTION_H__C9F82098_19E6_11D3_A181_00A0C9777D6E__INCLUDED_)
