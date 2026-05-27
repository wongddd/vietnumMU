#include "StdAfx.h"
#include "CB_DangKyInGame.h"
#include "NewUISystem.h"
#include "CBInterface.h"
#include "CUIController.h"
#include "CharacterManager.h"
#include "Util.h"
#include "Protocol.h"
#include "NewUIBase.h"
#include "Other.h"
#include "ZzzInterface.h"
#include "UIMng.h"

CB_DangKyInGame* gCB_DangKyInGame;


CB_DangKyInGame::CB_DangKyInGame()
{

	this->Clear();
}


CB_DangKyInGame::~CB_DangKyInGame()
{
	for (int i = 0; i < TYPE_INPUT_DKTK::eMaxINPUT; i++)
	{
		SAFE_DELETE(CInputData[i]);
	}

	SAFE_DELETE(CInputCaptCha);
}

void CB_DangKyInGame::Clear()
{
	for (int i = 0; i < TYPE_INPUT_DKTK::eMaxINPUT; i++)
	{
		CInputData[i] = NULL;
	}
	CInputCaptCha = NULL;
	this->TimeSendRegTK = GetTickCount();
	this->OpenDKTK = false;
}

void CB_DangKyInGame::OpenOnOff()
{
	if (GetTickCount() - gInterface.Data[eWindow_DangKyInGame].EventTick > 300)
	{
		gInterface.Data[eWindow_DangKyInGame].EventTick = GetTickCount();
		if (gInterface.Data[eWindow_DangKyInGame].OnShow)
		{
			gInterface.Data[eWindow_DangKyInGame].OnShow = 0;
			return;
		}
		//==Show hoac send packet open
		gInterface.Data[eWindow_DangKyInGame].OnShow = 1;


	}
}

bool CB_DangKyInGame::RenderWindow(int X, int Y)
{
	if (!gInterface.Data[eWindow_DangKyInGame].OnShow || !gCB_DangKyInGame)
	{
		if (this->OpenDKTK)
		{
			this->Clear();
		}
		return 0;
	}

	this->OpenDKTK = true;
	float WindowW = 262;
	float WindowH = 250;

	static char* mTextStatus[] = { "❂ 账号 :", "❂ 密码 :", "❂ 7位密保数字 :", "❂ 手机号码 :" , "验证码 :" };
	static int mInputType[] = { UIOPTION_NOLOCALIZEDCHARACTERS,
								UIOPTION_NOLOCALIZEDCHARACTERS,
								UIOPTION_NUMBERONLY,
								UIOPTION_NUMBERONLY };
	static int mMaxInput[] = { 10,
							   11,
							   7,
							   11};


	float StartX = (MAX_WIN_WIDTH / 2) - (WindowW / 2);;
	float StartY = 150;
	gInterface.Data[eWindow_DangKyInGame].AllowMove = false;

	g_pBCustomMenuInfo->gDrawWindowCustom(&StartX, &StartY, WindowW, WindowH, eWindow_DangKyInGame, "注册账号");

	TextDraw(g_hFontBold, StartX, StartY + 50, 0xFFA200B8, 0x0, WindowW, 0, 3, "账号只能使用 #字符 0~9, a~z"); //Notice

	float InputW = 110;
	float InputKhoangCach = 20;
	StartY += 30;

	for (int i = 0; i < TYPE_INPUT_DKTK::eMaxINPUT; i++)
	{
		TextDraw(g_hFontBold, StartX + 30, StartY + 50, 0xFFFFFFFF, 0x0, 100, 0, 1, mTextStatus[i]);
		gInterface.DrawBarForm((StartX + 120 ) - 3, (StartY + 50) - 3, InputW, 16, 0.0, 0.0, 0.0, 1.0);
		if (g_pBCustomMenuInfo->RenderInputBox(StartX + 120, StartY + 50 , InputW, 14, "", this->CInputData[i],(UIOPTIONS)mInputType[i], mMaxInput[i], (i == 1 ? TRUE: FALSE)))
		{
			this->CInputData[i]->SetTextColor(255, 255, 255, 255);
			this->CInputData[i]->SetBackColor(255, 0, 0, 0);
			
			if (this->CInputData[i+1]) this->CInputData[i]->SetTabTarget(this->CInputData[i+1]);
		}
		StartY += InputKhoangCach;
	}

	//===Captcha
	StartY += 20;
	float NTW = 70;
	TextDraw((HFONT)g_hFontBold, StartX + 40, StartY + 50, 0x62FF00FF, 0x0, 100, 0, 1, gOther.TextVN_NAPGAME[13]);
	float CaptChaX = StartX + 105;
	float CaptChaY = StartY + 45;
	if (!CInputCaptCha)
	{
		CInputCaptCha = new CUITextInputBox;
		CInputCaptCha->Init(pGameWindow, CaptChaX, CaptChaY, 4);
		CInputCaptCha->SetBackColor(0, 0, 0, 0);
		CInputCaptCha->SetTextColor(255, 255, 157, 0);
		CInputCaptCha->SetFont((HFONT)g_hFont);
		CInputCaptCha->SetState(UISTATE_NORMAL);
		CInputCaptCha->SetOption(UIOPTION_NUMBERONLY);
		CInputCaptCha->SetPosition(CaptChaX, CaptChaY);
	}
	else
	{
		gInterface.RenderCaptchaNumber(CaptChaX, CaptChaY, CInputCaptCha, gInterface.vCaptcha.c_str());
	}
	StartY += 30;
	if (g_pBCustomMenuInfo->DrawButton(StartX + 100, StartY + 45, 100, 12, gOther.TextVN_NAPGAME[14], 80, true))
	{
		char CGetTextCaptCha[5] = { 0, };
		this->CInputCaptCha->GetText(CGetTextCaptCha, MAX_ID_SIZE + 1);
		std::string CaptchaInput(CGetTextCaptCha);
		if (!gInterface.check_Captcha(gInterface.vCaptcha, CaptchaInput))
		{
			gInterface.OpenMessageBox("Error","验证码错误");
			
		}
		else
		{
			this->RequsetDKTK();
		}
		
	}
	gInterface.DrawMessageBox();
	return 1;
}

bool CB_DangKyInGame::RequsetDKTK()
{
	char szID[MAX_ID_SIZE + 1] = { 0, };
	char szPass[MAX_PASSWORD_SIZE + 1] = { 0, };
	char szSno[7 + 1] = { 0, };
	char szSDT[11 + 1] = { 0, };

	this->CInputData[Account]->GetText(szID, MAX_ID_SIZE + 1);
	this->CInputData[Pass]->GetText(szPass, MAX_PASSWORD_SIZE + 1);
	this->CInputData[Snonumber]->GetText(szSno, 7 + 1);
	this->CInputData[Phone]->GetText(szSDT, 11 + 1);

	if (this->TimeSendRegTK > GetTickCount())
	{ 
		gInterface.OpenMessageBox("Error", "操作过快，请减速 !!");
		return 0;
	}
	if (strlen(szID) < 1) 
	{
		gInterface.OpenMessageBox("Error", "请输入 账号");
		return 0;
	}
	if (strlen(szPass) < 1)
	{
		gInterface.OpenMessageBox("Error", "请输入 密码");
		return 0;
	}
	if (strlen(szSno) < 7)
	{
		gInterface.OpenMessageBox("Error", "请输入 7位安全码");
		return 0;
	}
	if (strlen(szSDT) < 10)
	{
		gInterface.OpenMessageBox("Error", "请输入 手机号码");
		return 0;
	}

	if (!CheckChuoiKyTuDacBiet(szID) || !CheckChuoiKyTuDacBiet(szPass))
	{
		gInterface.OpenMessageBox("Error", "账号不能包含特殊字符");
		return 0;
	}
	//==Send GS
	PMSG_REGISTER_MAIN_SEND pMsg;
	pMsg.header.set(0xD3, 0x05, sizeof(pMsg));
	pMsg.TypeSend = 0x01;

	memcpy(&pMsg.account, szID, sizeof(pMsg.account));
	memcpy(&pMsg.password, szPass, sizeof(pMsg.password));
	memcpy(&pMsg.numcode, szSno, sizeof(pMsg.numcode));
	memcpy(&pMsg.sodienthoai, szSDT, sizeof(pMsg.sodienthoai));

	DataSend((LPBYTE)& pMsg, pMsg.header.size);

	gInterface.vCaptcha = gInterface.generateCaptcha(4); //tao lai Captcha
	this->TimeSendRegTK = GetTickCount() + 5000;

	return 1;
}

void CB_DangKyInGame::RecvKQRegInGame(XULY_CGPACKET* lpMsg)
{
	int TypeKQNotice = lpMsg->ThaoTac;
	//gInterface.OpenMessageBox("KQ", "KQ %d", lpMsg->ThaoTac);

	char szID[MAX_ID_SIZE + 1] = { 0, };
	this->CInputData[Account]->GetText(szID, MAX_ID_SIZE + 1);

	char szPass[MAX_PASSWORD_SIZE + 1] = { 0, };
	this->CInputData[Pass]->GetText(szPass, MAX_PASSWORD_SIZE + 1);


	switch (TypeKQNotice)
	{
	case CB_DangKyInGame::eDangKyThanhCong:
		{
			gInterface.OpenMessageBox("结果", "恭喜您注册成功 ! \n ID : %s", szID);
			CUIMng& rUIMng = CUIMng::Instance();
			rUIMng.m_LoginWin.GetIDInputBox()->SetText(szID);
			rUIMng.m_LoginWin.GetPassInputBox()->SetText(szPass);
		}
		break;
	case CB_DangKyInGame::eTaiKhoanDaTonTai:
		gInterface.OpenMessageBox("结果", "ID : %s \n 已存在，请注册其他账号!", szID);
		break;
	case CB_DangKyInGame::eDuLieuNhapKhongDung:
		gInterface.OpenMessageBox("结果", "输入信息不正确，请重新检查!");
		break;
	default:
		break;
	}
}
