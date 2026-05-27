#pragma once
#include "CBInterface.h"
#include <functional>
#include <array>
#include <set>
#include <string>
#include <thread>
#include <atomic>
class CB_MUHelper
{

public:
	enum TYPE_ACTION_MUHELPER
	{
		eBuff,
		eBuffParty,
		eBuffPartyHeal,
		eHeal,
		ePickUp,
		eMoveRegen,
		eMaxVaule,
	};

	CB_MUHelper();
	virtual ~CB_MUHelper();

	void AddTarget(int iTargetId, bool bIsAttacking);
	void DeleteTarget(int iTargetId);

	void AddItem(int iItemId);
	void DeleteItem(int iItemId);

	void Work();

	std::set<int> m_setTargets;
	std::set<int> m_setTargetsAttacking;
	std::set<int> m_setItems;
	void ClearData();

	int m_iCurrentItem;
	int m_iCurrentTarget;
	int m_iCurrentItemMove;
	bool ClearDateOn;
	bool m_bTimerActivatedBuffOngoing;
	bool CB_MUHelper::MUHelperNhatItem();
	void CB_MUHelper::GetTarget();
	int CB_MUHelper::GetMonter();
	bool MUHelperAttack();
	bool MuHelperActiveAttack();
	bool MuHelperActiveHeal();
	void CleanupTargets();
	void CleanupItems();
	int PickUpItemMuHelper();
	bool CB_MUHelper::ShouldObtainItem(int iItemId);
	int CB_MUHelper::BuffTarget(CHARACTER* pTargetChar, int iBuffSkill);
	int CB_MUHelper::SimulateSkill(int iSkill, bool bTargetRequired, int iTarget);

	BYTE CacheAction[eMaxVaule];
	int m_SelectedCharacter;
	float m_DistanceSelectCharacter;
	void RenderCircle(CHARACTER* c);
	DWORD TimeDrawCircle;
	int RangeDraw;
	void SetCircle(float Range);
	DWORD m_ActionMoveRegen;
	int CB_MUHelper::SimulateMove(POINT posMove);
	void CB_MUHelper::CheckLiveTarget();
};

extern CB_MUHelper* gCB_MUHelper;