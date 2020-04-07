// MiddleMan.h: interface for the CMiddleMan class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIDDLEMAN_H__6AA8A786_CC25_11D2_86E5_00A0C9777D43__INCLUDED_)
#define AFX_MIDDLEMAN_H__6AA8A786_CC25_11D2_86E5_00A0C9777D43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define MP3_DEBUGQUEUE  //Toggles Debug Output for DebugQueue
#include "TaskQueue.h"
#include "MediaPlayer.h"

#define CMD_NONE 0
#define CMD_STOP 1
#define CMD_PAUSE 2
#define CMD_PLAY 3
#define CMD_SETFILE 4
#define CMD_SETFRAME 5

class CMediaPlayer;

class CMMNode : public TQueue_BaseNode<CMMNode>
{
public:
	CMMNode() : Command(CMD_NONE), Param1(0), m_pPlayer(NULL) {};
	CMMNode(CMediaPlayer* pPlayer,int c,DWORD Param = 0) : m_pPlayer(pPlayer), Command(c), Param1(Param) {};
	CMMNode(const CMMNode& node) : m_pPlayer(node.m_pPlayer), Command(node.Command), Param1(node.Param1) {};
	~CMMNode() {};
	int Command;
	DWORD Param1;
	CMediaPlayer* m_pPlayer;
};

typedef TaskQueue<CMMNode> CMiddleMan;

#endif // !defined(AFX_MIDDLEMAN_H__6AA8A786_CC25_11D2_86E5_00A0C9777D43__INCLUDED_)
