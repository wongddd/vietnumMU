#pragma once
#include "Protocol.h"
#if(CUSTOM_WINDOWLOCKITEM)
class CB_LockItem
{

public:
	struct CGPACKET_LOCKWINDOW
	{
		PSBMSG_HEAD header; // C3:F3:03
		DWORD Status;
		DWORD InPass;

	};
	CB_LockItem();
	~CB_LockItem();
	void CB_LockItem::DrawWindow();
	bool StatusLock;

	void CB_LockItem::OpenWindowLock();
	void CB_LockItem::SendLockUnLock(int Pass);
	void CB_LockItem::RecvGSSatusLockWindow(CGPACKET_LOCKWINDOW* lpMsg);
};

extern CB_LockItem* gCB_LockItem;
#endif