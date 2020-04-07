// TaskQueue.h: interface for the CTaskQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKQUEUE_H__C9F82094_19E6_11D3_A181_00A0C9777D6E__INCLUDED_)
#define AFX_TASKQUEUE_H__C9F82094_19E6_11D3_A181_00A0C9777D6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Dss.h"

template<class _Node>
class TQueue_BaseNode
{
public:
	TQueue_BaseNode() : next(NULL), prev(NULL) {};
	TQueue_BaseNode(const TQueue_BaseNode& node) : next(node.next), prev(node.prev) {};
	virtual ~TQueue_BaseNode() {};
	_Node* next;
	_Node* prev;
};

template<class _Node>
class TQueue  
{
public:
	TQueue();
	virtual ~TQueue();

	virtual void ClearQueue();
	virtual DWORD Push(const _Node& node);
	virtual DWORD Pop(_Node** ppNode);
	virtual _Node* Front();
	virtual DWORD Size() const;

private:
	DWORD m_size;
	_Node* m_head;
	_Node* m_tail;
};

template<class _Node>
TQueue<_Node>::TQueue()
{
	m_head = NULL;
	m_tail = NULL;
	m_size = 0;
}

template<class _Node>
TQueue<_Node>::~TQueue()
{
	ClearQueue();
}

template<class _Node>
void TQueue<_Node>::ClearQueue()
{
	_Node* node = m_head;
	while(node)
	{	
		node = m_head->next;
		delete m_head;
		m_head = node;
	}
	m_head = m_tail = NULL;
	m_size = 0;
}

template<class _Node>
DWORD TQueue<_Node>::Size() const
{
	return m_size;
}

template<class _Node>
DWORD TQueue<_Node>::Push(const _Node& node)
{
	_Node* n = new _Node(node);
	DWORD size;
	if(m_head)
	{
		n->next = m_head;
		n->prev = NULL;
		m_head->prev = n;
		m_head = n;
	}
	else
	{
		m_head = m_tail = n;
	}
	size = ++m_size;
	return size;
}

template<class _Node>
DWORD TQueue<_Node>::Pop(_Node** ppNode)
{
	DWORD size;
	_Node* n;
	if(m_tail)
	{
		n = m_tail->prev;
		*ppNode = m_tail;
		m_tail = n;
		if(m_tail) 
			m_tail->next = NULL;
		else	
			m_head = NULL;
		size = --m_size;
	}
	else
	{
		*ppNode = NULL;
		size = 0xFFFFFFFF;
	}
	return size;
}

template<class _Node>
_Node* TQueue<_Node>::Front()
{
	return m_tail;
}

#endif // !defined(AFX_TASKQUEUE_H__C9F82094_19E6_11D3_A181_00A0C9777D6E__INCLUDED_)
