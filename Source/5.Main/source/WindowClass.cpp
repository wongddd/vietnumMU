#include "stdafx.h"
#include "WindowClass.h"
#include "NewUISystem.h"
#include "CBInterface.h"
#include "CUIController.h"
#include "CharacterManager.h"
#include "Util.h"
#include "Protocol.h"
#include "NewUIBase.h"
#include "Other.h"

bool StatusCachePhoto = false;
CUIPhotoViewer m_PhotoChangeClass;
CNewUITextBox* m_pTextBoxChangeClass[7];
cWindowClass WindowClass;

cWindowClass::cWindowClass()
{
	this->Init();
	this->WCoinChangeClass = 0;

}

cWindowClass::~cWindowClass()
{
	for (int i = 0; i < 7; i++)
	{
		if (m_pTextBoxChangeClass[i])
			m_pTextBoxChangeClass[i]->Release();

		SAFE_DELETE(m_pTextBoxChangeClass[i]);
	}
}

void cWindowClass::Init()
{
	this->SetVisible(false);
	this->SetRect(120, 70, 400, 255);
	this->SetCurTab(0);
}

void cWindowClass::RecvData(PMSG_CHANGECLASS_DATA* Data)
{
	this->WCoinChangeClass = Data->m_WCoinC;
}

void cWindowClass::SendChangeClass(int Type)
{
	CG_CHANGECLASS_SEND pRequest;
	pRequest.Head.set(0xFB, 0x08, sizeof(pRequest));

	pRequest.Type = Type;

	DataSend((BYTE*)& pRequest, pRequest.Head.size);
}
void cWindowClass::SetRect(float a1, float a2, float a3, float a4)
{
	this->x = a1;
	this->y = a2;
	this->w = a3;
	this->h = a4;

}

void cWindowClass::Draw()
{
	if (!this->GetVisible())
	{
		return;
	}

	if(!StatusCachePhoto) this->CachePhoto();
	glAlphaFunc(GL_GREATER, 0.0f);

	TextDraw((HFONT)g_hFontBold, x, y, 0x0, 0x373b45ff, w, h, 1, " ");


	int i = 0;


	g_pBCustomMenuInfo->gDrawWindowCustom(&x, &y, w, h, eWindowChangeClass, "ĐỔI GIỚI TÍNH NHÂN VẬT");

	for (int i = 0; i < 7; i++) // Hiển thị button
	{
		if (this->CurTab == 7)
		{
			const BYTE state[3] = { 1, 1, 2 };
			UIController.Button(CNewUIInGameShop::IMAGE_IGS_CATEGORY_BTN, x + 20, y + 36 + 30 * i, 73, 27, 73, 27, 3, state, WindowClass.SelectTabAction, i);
			TextDraw((HFONT)g_hFontBold, x + 20, y + 45 + 30 * i, 0xffffffff, 0x0, 73, 0, 3, gOther.Text_Chung[3 + i]);
		}
		else
		{
			const BYTE state[3] = { 0, 1, 2 };
			UIController.Button(CNewUIInGameShop::IMAGE_IGS_CATEGORY_BTN, x + 20, y + 36 + 30 * i, 73, 27, 73, 27, 3, state, WindowClass.SelectTabAction, i);
			TextDraw((HFONT)g_hFont, x + 20, y + 45 + 30 * i, 0xffffffbb, 0x0, 73, 0, 3, gOther.Text_Chung[3 + i]);
		}
	}

	glAlphaFunc(GL_GREATER, 0.25f);



	switch (this->CurTab)
	{
	case 0:

		this->TabPhuThuy();
		break;
	case 1:
		this->TabChienBinh();
		break;
	case 2:
		this->TabTienNu();
		break;
	case 3:
		this->TabDauSi();
		break;
	case 4:
		this->TabChuaTe();
		break;
	case 5:
		this->TabThuatSi();
		break;
	case 6:
		this->TabThietBinh();
		break;
	default:
		break;
	}
	if (gInterface.Data[eWindowChangeClass].OnClick)
	{
		this->SetRect(x, y, w, h);
		for (int i = 0; i < 7; i++)
		{
			if (m_pTextBoxChangeClass[i])
			{
				m_pTextBoxChangeClass[i]->SetPos(x + 102, y + 170, 290, 70);
			}

		}
		if (StatusCachePhoto)
		{
			m_PhotoChangeClass.SetPosition(x + 100, y + 25);
		}
	}

}
void cWindowClass::SetClassPhoto(int Class)
{
	m_PhotoChangeClass.SetAnimation(AT_STAND1);

	if (m_PhotoChangeClass.GetPhotoChar()->Class != Class)
	{
		m_PhotoChangeClass.CopyPlayer();
		m_PhotoChangeClass.SetClass(Class);
		for (int i = 0; i < MAX_BODYPART; ++i)
		{
			m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].m_pCloth[0] = NULL;
			m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].m_pCloth[1] = NULL;
			m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].m_byNumCloth = 0;
			m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Type = -1;
			m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Level = 0;
			m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Option1 = 0;
		}
		m_PhotoChangeClass.GetPhotoChar()->BodyPart[1].Type = MODEL_BODY_HELM + gCharacterManager.GetSkinModelIndex(Class);
		m_PhotoChangeClass.GetPhotoChar()->BodyPart[2].Type = MODEL_BODY_ARMOR + gCharacterManager.GetSkinModelIndex(Class);
		m_PhotoChangeClass.GetPhotoChar()->BodyPart[3].Type = MODEL_BODY_PANTS + gCharacterManager.GetSkinModelIndex(Class);
		m_PhotoChangeClass.GetPhotoChar()->BodyPart[4].Type = MODEL_BODY_GLOVES + gCharacterManager.GetSkinModelIndex(Class);
		m_PhotoChangeClass.GetPhotoChar()->BodyPart[5].Type = MODEL_BODY_BOOTS + gCharacterManager.GetSkinModelIndex(Class);

		m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Type = -1;
		m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Level = 0;
		m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Option1 = 0;

		m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Type = -1;
		m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Level = 0;
		m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Option1 = 0;

		m_PhotoChangeClass.GetPhotoChar()->Wing.Type = -1;
		m_PhotoChangeClass.GetPhotoChar()->Wing.Level = 0;
		m_PhotoChangeClass.GetPhotoChar()->Wing.Option1 = 0;
		m_PhotoChangeClass.GetPhotoChar()->Wing.m_pCloth[0] = NULL;
		m_PhotoChangeClass.GetPhotoChar()->Wing.m_pCloth[1] = NULL;
		m_PhotoChangeClass.GetPhotoChar()->Wing.m_byNumCloth = 0;
	}
	switch (Class)
	{
		case 0 :
		{
			m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Type = MODEL_ITEM + 5 * 512 + 10;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Level = 15;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Option1 = 63;

			m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Type = MODEL_ITEM + 6 * 512 + 15;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Level = 15;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Option1 = 63;
			for (int i = 1; i < MAX_BODYPART; i++)
			{
				m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Type = MODEL_ITEM + (6 + i) * 512 + 18;
				m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Level = 15;
				m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Option1 = 63;
			}
			m_PhotoChangeClass.GetPhotoChar()->Wing.Type = MODEL_ITEM + 12 * 512 + 37;
			m_PhotoChangeClass.GetPhotoChar()->Wing.Level = 15;
			m_PhotoChangeClass.GetPhotoChar()->Wing.Option1 = 63;
		}
		break;
		case 1:
		{
			m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Type = MODEL_ITEM +19;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Level = 15;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Option1 = 63;

			m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Type = MODEL_ITEM +19;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Level = 15;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Option1 = 63;
			for (int i = 1; i < MAX_BODYPART; i++)
			{
				m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Type = (MODEL_ITEM + ((6 + i) * 512)) + 1;
				m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Level = 15;
				m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Option1 = 63;
			}
			m_PhotoChangeClass.GetPhotoChar()->Wing.Type = MODEL_ITEM + 12 * 512 + 36;
			m_PhotoChangeClass.GetPhotoChar()->Wing.Level = 15;
			m_PhotoChangeClass.GetPhotoChar()->Wing.Option1 = 63;
		}
		break;
		case 2:
		{
			m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Type = MODEL_ITEM + 4 * 512 + 33;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Level = 15;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Option1 = 63;

			m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Type = -1;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Level = 15;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Option1 = 63;
			for (int i = 1; i < MAX_BODYPART; i++)
			{
				m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Type = MODEL_ITEM + (6 + i) * 512 + 31;
				m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Level = 15;
				m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Option1 = 63;
			}
			m_PhotoChangeClass.GetPhotoChar()->Wing.Type = MODEL_ITEM + 12 * 512 + 38;
			m_PhotoChangeClass.GetPhotoChar()->Wing.Level = 15;
			m_PhotoChangeClass.GetPhotoChar()->Wing.Option1 = 63;
		}
		break;
		case 3:
		{
			m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Type = MODEL_ITEM + 19;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Level = 15;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Option1 = 63;

			m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Type = MODEL_ITEM + 19;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Level = 15;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Option1 = 63;
			for (int i = 2; i < MAX_BODYPART; i++)
			{
				m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Type = MODEL_ITEM + (6 + i) * 512 + 15;
				m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Level = 15;
				m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Option1 = 63;
			}
			m_PhotoChangeClass.GetPhotoChar()->Wing.Type = MODEL_ITEM + 12 * 512 + 39;
			m_PhotoChangeClass.GetPhotoChar()->Wing.Level = 15;
			m_PhotoChangeClass.GetPhotoChar()->Wing.Option1 = 63;
		}
		break;
		case 4:
		{
			m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Type = MODEL_ITEM + 2 * 512 + 14;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Level = 15;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Option1 = 63;

			m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Type = MODEL_ITEM + 6 * 512 + 7;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Level = 15;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Option1 = 63;
			for (int i = 1; i < MAX_BODYPART; i++)
			{
				m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Type = MODEL_ITEM + (6 + i) * 512 + 51;
				m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Level = 15;
				m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Option1 = 63;
			}
			m_PhotoChangeClass.GetPhotoChar()->Wing.Type = MODEL_ITEM + 12 * 512 + 40;
			m_PhotoChangeClass.GetPhotoChar()->Wing.Level = 15;
			m_PhotoChangeClass.GetPhotoChar()->Wing.Option1 = 63;
		}
		break;
		case 5:
		{
			m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Type = MODEL_ITEM + 5 * 512 + 32;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Level = 15;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Option1 = 63;

			m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Type = MODEL_ITEM + 5 * 512 + 29;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Level = 15;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Option1 = 63;
			for (int i = 1; i < MAX_BODYPART; i++)
			{
				m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Type = MODEL_ITEM + (6 + i) * 512 + 44;
				m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Level = 15;
				m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Option1 = 63;
			}
			m_PhotoChangeClass.GetPhotoChar()->Wing.Type = MODEL_ITEM + 12 * 512 + 43;
			m_PhotoChangeClass.GetPhotoChar()->Wing.Level = 15;
			m_PhotoChangeClass.GetPhotoChar()->Wing.Option1 = 63;
		}
		break;
		case 6:
		{
			m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Type = MODEL_ITEM + 0 * 512 + 32;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Level = 15;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[0].Option1 = 63;

			m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Type = MODEL_ITEM + 0 * 512 + 32;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Level = 15;
			m_PhotoChangeClass.GetPhotoChar()->Weapon[1].Option1 = 63;
			for (int i = 1; i < MAX_BODYPART; i++)
			{
				m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Type = MODEL_ITEM + (6 + i) * 512 + 59;
				m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Level = 15;
				m_PhotoChangeClass.GetPhotoChar()->BodyPart[i].Option1 = 63;
			}
			m_PhotoChangeClass.GetPhotoChar()->Wing.Type = MODEL_ITEM + 12 * 512 + 50;
			m_PhotoChangeClass.GetPhotoChar()->Wing.Level = 15;
			m_PhotoChangeClass.GetPhotoChar()->Wing.Option1 = 63;
		}
		break;
	default:
		break;
	}


}
void cWindowClass::CachePhoto()
{
	m_PhotoChangeClass.Init(0);
	m_PhotoChangeClass.SetSize(150, 150);
	m_PhotoChangeClass.CopyPlayer();
	m_PhotoChangeClass.SetAutoupdatePlayer(TRUE);
	m_PhotoChangeClass.SetAnimation(AT_STAND1);
	m_PhotoChangeClass.SetAngle(120);
	m_PhotoChangeClass.SetZoom(0.8);
	m_PhotoChangeClass.SetPosition(x + 100, y + 25);
	for (int i = 0; i < 7; i++)
	{
		//===Text Box
		m_pTextBoxChangeClass[i] = new CNewUITextBox();
		m_pTextBoxChangeClass[i]->Create(x + 102, y + 170, 290, 70);
		if (m_pTextBoxChangeClass[i])
		{
			m_pTextBoxChangeClass[i]->ClearText();
			
		}
	}
	cWindowClass::CreateText();
	StatusCachePhoto = true;
}
void cWindowClass::TabHuongDan()
{

}


void cWindowClass::TabPhuThuy()
{
	//character.DrawHero2(x + 90, y + 30, 150, 150);
	if (StatusCachePhoto)
	{
		if (m_PhotoChangeClass.GetPhotoChar()->Class != 0)
		{
			this->SetClassPhoto(0);
		}
		m_PhotoChangeClass.Render();
	}


	TextDraw((HFONT)g_hFont, x + 250, y + 38, 0x0, 0x87CEFA, 140, 15, 0, " ");
	TextDraw((HFONT)g_hFont, x + 250, y + 38 + 25 * 1, 0x0, 0x00F5FF, 140, 15, 0, " ");
	TextDraw((HFONT)g_hFont, x + 250, y + 38 + 25 * 2, 0x0, 0x00FA9A, 140, 15, 0, " ");
	TextDraw((HFONT)g_hFont, x + 250, y + 38 + 25 * 3, 0x0, 0xCAFF70, 140, 15, 0, " ");

	TextDraw((HFONT)g_hFont, x + 240, y + 43, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[10], gOther.Text_Chung[3]);
	//TextDraw((HFONT)g_hFont, x + 240 , y + 43 + 25 * 1, 0xffffffff, 0x0, 150, 0, 3, "CHI PHÍ : %d WcoinC",gChangeClass.m_WCoinC);
	TextDraw((HFONT)g_hFont, x + 240, y + 43 + 25 * 1, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[11], gInterface.NumberFormat(this->WCoinChangeClass));
	TextDraw((HFONT)g_hFont, x + 240, y + 43 + 25 * 2, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[12]);
	TextDraw((HFONT)g_hFont, x + 240, y + 43 + 25 * 3, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[13]);

	UIController.Button(32331, x + 270, y + 38 + 25 * 4, 73, 27, 128.0, 128.0, this->ChangeClassDW);
	//UIController.Button(32331, x + 20, y + 36 + 30 * i, 73, 27, 73, 27, 3, state, WindowClass.SelectTabAction, i);
	TextDraw((HFONT)g_hFontBold, x + 270, y + 45 + 25 * 4, 0xffffffff, 0x0, 73, 0, 3, gOther.Text_Chung[14]);

	if (m_pTextBoxChangeClass[0])
	{
		m_pTextBoxChangeClass[0]->RenderKemScroll();
	}
	
	//zText1.Draw(x + 102, y + 170, 290, 5, 15, true);
}
void cWindowClass::TabChienBinh()
{

	//character.DrawHero1(x + 90, y + 30, 150, 150);
	if (StatusCachePhoto)
	{
		if (m_PhotoChangeClass.GetPhotoChar()->Class != 1)
		{
			this->SetClassPhoto(1);
		}
		m_PhotoChangeClass.Render();
	}
	TextDraw((HFONT)g_hFont, x + 250, y + 38, 0x0, 0x87CEFA, 140, 15, 0, " ");
	TextDraw((HFONT)g_hFont, x + 250, y + 38 + 25 * 1, 0x0, 0x00F5FF, 140, 15, 0, " ");
	TextDraw((HFONT)g_hFont, x + 250, y + 38 + 25 * 2, 0x0, 0x00FA9A, 140, 15, 0, " ");
	TextDraw((HFONT)g_hFont, x + 250, y + 38 + 25 * 3, 0x0, 0xCAFF70, 140, 15, 0, " ");

	TextDraw((HFONT)g_hFont, x + 240, y + 43, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[10], gOther.Text_Chung[4]);
	TextDraw((HFONT)g_hFont, x + 240, y + 43 + 25 * 1, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[11], gInterface.NumberFormat(this->WCoinChangeClass));
	TextDraw((HFONT)g_hFont, x + 240, y + 43 + 25 * 2, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[12]);
	TextDraw((HFONT)g_hFont, x + 240, y + 43 + 25 * 3, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[13]);

	UIController.Button(32331, x + 270, y + 38 + 25 * 4, 73, 27, 128.0, 128.0, this->ChangeClassDK);
	TextDraw((HFONT)g_hFontBold, x + 270, y + 45 + 25 * 4, 0xffffffff, 0x0, 73, 0, 3, gOther.Text_Chung[14]);

	if (m_pTextBoxChangeClass[1])
	{
		m_pTextBoxChangeClass[1]->RenderKemScroll();
	}
}
void cWindowClass::TabTienNu()
{
	//character.DrawHero3(x + 90, y + 30, 150, 150);
	if (StatusCachePhoto)
	{
		if (m_PhotoChangeClass.GetPhotoChar()->Class != 2)
		{
			this->SetClassPhoto(2);
		}
		m_PhotoChangeClass.Render();
	}
	TextDraw((HFONT)g_hFont, x + 250, y + 38, 0x0, 0x87CEFA, 140, 15, 0, " ");
	TextDraw((HFONT)g_hFont, x + 250, y + 38 + 25 * 1, 0x0, 0x00F5FF, 140, 15, 0, " ");
	TextDraw((HFONT)g_hFont, x + 250, y + 38 + 25 * 2, 0x0, 0x00FA9A, 140, 15, 0, " ");
	TextDraw((HFONT)g_hFont, x + 250, y + 38 + 25 * 3, 0x0, 0xCAFF70, 140, 15, 0, " ");

	TextDraw((HFONT)g_hFont, x + 240, y + 43, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[10], gOther.Text_Chung[5]);
	TextDraw((HFONT)g_hFont, x + 240, y + 43 + 25 * 1, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[11], gInterface.NumberFormat(this->WCoinChangeClass));
	TextDraw((HFONT)g_hFont, x + 240, y + 43 + 25 * 2, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[12]);
	TextDraw((HFONT)g_hFont, x + 240, y + 43 + 25 * 3, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[13]);

	UIController.Button(32331, x + 270, y + 38 + 25 * 4, 73, 27, 128.0, 128.0, this->ChangeClassEFL);
	TextDraw((HFONT)g_hFontBold, x + 270, y + 45 + 25 * 4, 0xffffffff, 0x0, 73, 0, 3, gOther.Text_Chung[14]);
	if (m_pTextBoxChangeClass[2])
	{
		m_pTextBoxChangeClass[2]->RenderKemScroll();
	}
}
void cWindowClass::TabDauSi()
{

	//character.DrawHero4(x + 90, y + 30, 150, 150);
	if (StatusCachePhoto)
	{
		if (m_PhotoChangeClass.GetPhotoChar()->Class != 3)
		{
			this->SetClassPhoto(3);
		}
		m_PhotoChangeClass.Render();
	}
	TextDraw((HFONT)g_hFont, x + 250, y + 38, 0x0, 0x87CEFA, 140, 15, 0, " ");
	TextDraw((HFONT)g_hFont, x + 250, y + 38 + 25 * 1, 0x0, 0x00F5FF, 140, 15, 0, " ");
	TextDraw((HFONT)g_hFont, x + 250, y + 38 + 25 * 2, 0x0, 0x00FA9A, 140, 15, 0, " ");
	TextDraw((HFONT)g_hFont, x + 250, y + 38 + 25 * 3, 0x0, 0xCAFF70, 140, 15, 0, " ");

	TextDraw((HFONT)g_hFont, x + 240, y + 43, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[10], gOther.Text_Chung[6]);
	TextDraw((HFONT)g_hFont, x + 240, y + 43 + 25 * 1, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[11], gInterface.NumberFormat(this->WCoinChangeClass));
	TextDraw((HFONT)g_hFont, x + 240, y + 43 + 25 * 2, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[12]);
	TextDraw((HFONT)g_hFont, x + 240, y + 43 + 25 * 3, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[13]);

	UIController.Button(32331, x + 270, y + 38 + 25 * 4, 73, 27, 128.0, 128.0, this->ChangeClassMG);
	TextDraw((HFONT)g_hFontBold, x + 270, y + 45 + 25 * 4, 0xffffffff, 0x0, 73, 0, 3, gOther.Text_Chung[14]);

	if (m_pTextBoxChangeClass[3])
	{
		m_pTextBoxChangeClass[3]->RenderKemScroll();
	}
}
void cWindowClass::TabChuaTe()
{
	//character.DrawHero5(x + 90, y + 30, 150, 150);
	if (StatusCachePhoto)
	{
		if (m_PhotoChangeClass.GetPhotoChar()->Class != 4)
		{
			this->SetClassPhoto(4);
		}
		m_PhotoChangeClass.Render();
	}
	TextDraw((HFONT)g_hFont, x + 250, y + 38, 0x0, 0x87CEFA, 140, 15, 0, " ");
	TextDraw((HFONT)g_hFont, x + 250, y + 38 + 25 * 1, 0x0, 0x00F5FF, 140, 15, 0, " ");
	TextDraw((HFONT)g_hFont, x + 250, y + 38 + 25 * 2, 0x0, 0x00FA9A, 140, 15, 0, " ");
	TextDraw((HFONT)g_hFont, x + 250, y + 38 + 25 * 3, 0x0, 0xCAFF70, 140, 15, 0, " ");

	TextDraw((HFONT)g_hFont, x + 240, y + 43, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[10], gOther.Text_Chung[7]);
	TextDraw((HFONT)g_hFont, x + 240, y + 43 + 25 * 1, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[11], gInterface.NumberFormat(this->WCoinChangeClass));
	TextDraw((HFONT)g_hFont, x + 240, y + 43 + 25 * 2, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[12]);
	TextDraw((HFONT)g_hFont, x + 240, y + 43 + 25 * 3, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[13]);

	UIController.Button(32331, x + 270, y + 38 + 25 * 4, 73, 27, 128.0, 128.0, this->ChangeClassDL);
	TextDraw((HFONT)g_hFontBold, x + 270, y + 45 + 25 * 4, 0xffffffff, 0x0, 73, 0, 3, gOther.Text_Chung[14]);

	if (m_pTextBoxChangeClass[4])
	{
		m_pTextBoxChangeClass[4]->RenderKemScroll();
	}
}

void cWindowClass::TabThuatSi()
{
	//character.DrawHero6(x + 90, y + 30, 150, 150);
	if (StatusCachePhoto)
	{
		if (m_PhotoChangeClass.GetPhotoChar()->Class != 5)
		{
			this->SetClassPhoto(5);
		}
		m_PhotoChangeClass.Render();
	}
	TextDraw((HFONT)g_hFont, x + 250, y + 38, 0x0, 0x87CEFA, 140, 15, 0, " ");
	TextDraw((HFONT)g_hFont, x + 250, y + 38 + 25 * 1, 0x0, 0x00F5FF, 140, 15, 0, " ");
	TextDraw((HFONT)g_hFont, x + 250, y + 38 + 25 * 2, 0x0, 0x00FA9A, 140, 15, 0, " ");
	TextDraw((HFONT)g_hFont, x + 250, y + 38 + 25 * 3, 0x0, 0xCAFF70, 140, 15, 0, " ");

	TextDraw((HFONT)g_hFont, x + 240, y + 43, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[10], gOther.Text_Chung[8]);
	TextDraw((HFONT)g_hFont, x + 240, y + 43 + 25 * 1, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[11], gInterface.NumberFormat(this->WCoinChangeClass));
	TextDraw((HFONT)g_hFont, x + 240, y + 43 + 25 * 2, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[12]);
	TextDraw((HFONT)g_hFont, x + 240, y + 43 + 25 * 3, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[13]);

	UIController.Button(32331, x + 270, y + 38 + 25 * 4, 73, 27, 128.0, 128.0, this->ChangeClassSUM);
	TextDraw((HFONT)g_hFontBold, x + 270, y + 45 + 25 * 4, 0xffffffff, 0x0, 73, 0, 3, gOther.Text_Chung[14]);

	if (m_pTextBoxChangeClass[5])
	{
		m_pTextBoxChangeClass[5]->RenderKemScroll();
	}
}

void cWindowClass::TabThietBinh()
{
	//character.DrawHero7(x + 90, y + 30, 150, 150);
	if (StatusCachePhoto)
	{
		if (m_PhotoChangeClass.GetPhotoChar()->Class != 6)
		{
			this->SetClassPhoto(6);
		}
		m_PhotoChangeClass.Render();
	}
	TextDraw((HFONT)g_hFont, x + 250, y + 38, 0x0, 0x87CEFA, 140, 15, 0, " ");
	TextDraw((HFONT)g_hFont, x + 250, y + 38 + 25 * 1, 0x0, 0x00F5FF, 140, 15, 0, " ");
	TextDraw((HFONT)g_hFont, x + 250, y + 38 + 25 * 2, 0x0, 0x00FA9A, 140, 15, 0, " ");
	TextDraw((HFONT)g_hFont, x + 250, y + 38 + 25 * 3, 0x0, 0xCAFF70, 140, 15, 0, " ");

	TextDraw((HFONT)g_hFont, x + 240, y + 43, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[10], gOther.Text_Chung[9]);
	TextDraw((HFONT)g_hFont, x + 240, y + 43 + 25 * 1, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[11], gInterface.NumberFormat(this->WCoinChangeClass));
	TextDraw((HFONT)g_hFont, x + 240, y + 43 + 25 * 2, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[12]);
	TextDraw((HFONT)g_hFont, x + 240, y + 43 + 25 * 3, 0xffffffff, 0x0, 150, 0, 3, gOther.Text_Chung[13]);

	UIController.Button(32331, x + 270, y + 38 + 25 * 4, 73, 27, 128.0, 128.0, this->ChangeClassRF);
	TextDraw((HFONT)g_hFontBold, x + 270, y + 45 + 25 * 4, 0xffffffff, 0x0, 73, 0, 3, gOther.Text_Chung[14]);

	if (m_pTextBoxChangeClass[6])
	{
		m_pTextBoxChangeClass[6]->RenderKemScroll();
	}
}

bool cWindowClass::BlockMouse()
{
	if (!this->GetVisible())
	{
		return false;
	}

	return SEASON3B::CheckMouseIn(x - 20, y - 20, w + 70, h + 90);
}
bool cWindowClass::GetVisible()
{
	return gInterface.Data[eWindowChangeClass].OnShow;

}
void cWindowClass::SetVisible(bool a1)
{
	gInterface.Data[eWindowChangeClass].OnShow = a1;
	gInterface.Data[eWindowChangeClass].EventTick = GetTickCount();
}

void cWindowClass::SetCurTab(int a1)
{
	this->CurTab = a1;
	gInterface.Data[eWindowChangeClass].EventTick = GetTickCount();
}


void cWindowClass::SelectTabAction(LPVOID pClass)
{
	cUIController* This = (cUIController*)pClass;
	WindowClass.SetCurTab(This->GetCallBackValue());
}

void cWindowClass::ChangeClassDW(LPVOID pClass)
{
	if (GetTickCount() - gInterface.Data[eWindowChangeClass].EventTick < 300) return;
	cUIController* This = (cUIController*)pClass;
	WindowClass.SendChangeClass(0);
	WindowClass.SetVisible(false);
}

void cWindowClass::ChangeClassDK(LPVOID pClass)
{
	if (GetTickCount() - gInterface.Data[eWindowChangeClass].EventTick < 300) return;
	cUIController* This = (cUIController*)pClass;
	WindowClass.SendChangeClass(16);
	WindowClass.SetVisible(false);
}

void cWindowClass::ChangeClassEFL(LPVOID pClass)
{
	if (GetTickCount() - gInterface.Data[eWindowChangeClass].EventTick < 300) return;
	cUIController* This = (cUIController*)pClass;
	WindowClass.SendChangeClass(32);
	WindowClass.SetVisible(false);
}

void cWindowClass::ChangeClassMG(LPVOID pClass)
{
	if (GetTickCount() - gInterface.Data[eWindowChangeClass].EventTick < 300) return;
	cUIController* This = (cUIController*)pClass;
	WindowClass.SendChangeClass(48);
	WindowClass.SetVisible(false);
}

void cWindowClass::ChangeClassDL(LPVOID pClass)
{
	if (GetTickCount() - gInterface.Data[eWindowChangeClass].EventTick < 300) return;
	cUIController* This = (cUIController*)pClass;
	WindowClass.SendChangeClass(64);
	WindowClass.SetVisible(false);
}

void cWindowClass::ChangeClassSUM(LPVOID pClass)
{
	if (GetTickCount() - gInterface.Data[eWindowChangeClass].EventTick < 300) return;
	cUIController* This = (cUIController*)pClass;
	WindowClass.SendChangeClass(80);
	WindowClass.SetVisible(false);
}

void cWindowClass::ChangeClassRF(LPVOID pClass)
{
	if (GetTickCount() - gInterface.Data[eWindowChangeClass].EventTick < 300) return;
	cUIController* This = (cUIController*)pClass;
	WindowClass.SendChangeClass(96);
	WindowClass.SetVisible(false);
}


void cWindowClass::DrawButton(int IMGcode, float x, float y, float w, float h, float a6, float a7)
{
	if (SEASON3B::CheckMouseIn(x, y, w, h))
	{
		if (GetKeyState(VK_LBUTTON) & 0x8000)
		{
			RenderBitmap(IMGcode, x, y, w, h, 0.0, h / a7 * 2.0, w / a6, h / a7, 1, 1, 0.0);
		}
		else {
			RenderBitmap(IMGcode, x, y, w, h, 0.0, h / a7, w / a6, h / a7, 1, 1, 0.0);
		}
		return;
	}
	else
	{
		RenderBitmap(IMGcode, x, y, w, h, 0.0, 0.0, w / a6, h / a7, 1, 1, 0.0);
	}
	return;
}

void cWindowClass::CreateText()
{
	//zText1.SetFont(CreateFontA(13, 0, 0, 0, 400, 0, 0, 0, 0x1, 0, 0, 3, 0, "Tahoma"));
	//zText2.SetFont(CreateFontA(13, 0, 0, 0, 400, 0, 0, 0, 0x1, 0, 0, 3, 0, "Tahoma"));
	//zText3.SetFont(CreateFontA(13, 0, 0, 0, 400, 0, 0, 0, 0x1, 0, 0, 3, 0, "Tahoma"));
	//zText4.SetFont(CreateFontA(13, 0, 0, 0, 400, 0, 0, 0, 0x1, 0, 0, 3, 0, "Tahoma"));
	//zText5.SetFont(CreateFontA(13, 0, 0, 0, 400, 0, 0, 0, 0x1, 0, 0, 3, 0, "Tahoma"));
	//zText6.SetFont(CreateFontA(13, 0, 0, 0, 400, 0, 0, 0, 0x1, 0, 0, 3, 0, "Tahoma"));
	//zText7.SetFont(CreateFontA(13, 0, 0, 0, 400, 0, 0, 0, 0x1, 0, 0, 3, 0, "Tahoma"));
	////alpha B G R
	for (int st = 0; st < 50; st++)
	{
		if (strcmp(gOther.Text_InfoDW[st], "Null"))
		{
			//zText1.AddText(1, (st == 0) ? 3 : 1, (st == 0) ? 0xFF00FFFF : 0xFFffffff, 0x0, gOther.Text_InfoDW[st], 1);
			if(m_pTextBoxChangeClass[0])m_pTextBoxChangeClass[0]->AddText(gOther.Text_InfoDW[st]);
		}
		if (strcmp(gOther.Text_InfoDK[st], "Null"))
		{
			//zText2.AddText(1, (st == 0) ? 3 : 1, (st == 0) ? 0xFF00FFFF : 0xFFffffff, 0x0, gOther.Text_InfoDK[st], 1);
			if (m_pTextBoxChangeClass[1])m_pTextBoxChangeClass[1]->AddText(gOther.Text_InfoDK[st]);
		}
		if (strcmp(gOther.Text_InfoELF[st], "Null"))
		{
			//zText3.AddText(1, (st == 0) ? 3 : 1, (st == 0) ? 0xFF00FFFF : 0xFFffffff, 0x0, gOther.Text_InfoELF[st], 1);
			if (m_pTextBoxChangeClass[2])m_pTextBoxChangeClass[2]->AddText(gOther.Text_InfoELF[st]);
		}
		if (strcmp(gOther.Text_InfoMG[st], "Null"))
		{
			//zText4.AddText(1, (st == 0) ? 3 : 1, (st == 0) ? 0xFF00FFFF : 0xFFffffff, 0x0, gOther.Text_InfoMG[st], 1);
			if (m_pTextBoxChangeClass[3])m_pTextBoxChangeClass[3]->AddText(gOther.Text_InfoMG[st]);
		}
		if (strcmp(gOther.Text_InfoDL[st], "Null"))
		{
			//zText5.AddText(1, (st == 0) ? 3 : 1, (st == 0) ? 0xFF00FFFF : 0xFFffffff, 0x0, gOther.Text_InfoDL[st], 1);
			if (m_pTextBoxChangeClass[4])m_pTextBoxChangeClass[4]->AddText(gOther.Text_InfoDL[st]);
		}
		if (strcmp(gOther.Text_InfoSUM[st], "Null"))
		{
			//zText6.AddText(1, (st == 0) ? 3 : 1, (st == 0) ? 0xFF00FFFF : 0xFFffffff, 0x0, gOther.Text_InfoSUM[st], 1);
			if (m_pTextBoxChangeClass[5])m_pTextBoxChangeClass[5]->AddText(gOther.Text_InfoSUM[st]);
		}
		if (strcmp(gOther.Text_InfoRF[st], "Null"))
		{
			//zText7.AddText(1, (st == 0) ? 3 : 1, (st == 0) ? 0xFF00FFFF : 0xFFffffff, 0x0, gOther.Text_InfoRF[st], 1);
			if (m_pTextBoxChangeClass[6])m_pTextBoxChangeClass[6]->AddText(gOther.Text_InfoRF[st]);
		}
	}
	//zText1.AddText(1, 3, 0xFF00FFFF, 0x0, "介绍黑暗巫师",1);

	//zText1.AddText(1, 1, 0xFFffffff, 0x0, "有些人努力获得强壮的体魄", 1);
	//zText1.AddText(1, 1, 0xFFffffff, 0x0, "利用自然规律为自己服务", 1);
	//zText1.AddText(1, 1, 0xFFffffff, 0x0, "巫师可以使用魔法攻击敌人", 1);
	//zText1.AddText(1, 1, 0xFFffffff, 0x0, "此外他还可以请求来自", 1);
	//zText1.AddText(1, 1, 0xFFffffff, 0x0, "神秘灵魂的支援所有人都畏惧魔法力量", 1);
	//zText1.AddText(1, 1, 0xFFffffff, 0x0, "因此巫师通常独来独往然而", 1);
	//zText1.AddText(1, 1, 0xFFffffff, 0x0, "虚弱的外表可能会让你产生错误的想法", 1);
	//zText1.AddText(1, 1, 0xFFffffff, 0x0, "不要被他欺骗许多战士", 1);
	//zText1.AddText(1, 1, 0xFFffffff, 0x0, "在接近巫师之前就被火焰吞噬", 1);
	//zText1.AddText(1, 1, 0xFFffffff, 0x0, "巫师拥有独立的攻击魔法列表", 1);
	//zText1.AddText(1, 1, 0xFFffffff, 0x0, "尽管强大但他们的许多魔法需要", 1);
	//zText1.AddText(1, 1, 0xFFffffff, 0x0, "时间寻找和耗费巨大能量来施展", 1);

	//zText2.SetFont(CreateFontA(13, 0, 0, 0, 400, 0, 0, 0, 0x1, 0, 0, 3, 0, "Tahoma"));
	////alpha B G R
	//zText2.AddText(1, 3, 0xFF00FFFF, 0x0, "介绍黑暗战士", 1);
	//zText2.AddText(1, 1, 0xFFffffff, 0x0, "战士是力量与权力的化身", 1);
	//zText2.AddText(1, 1, 0xFFffffff, 0x0, "在激战中他们可以杀死", 1);
	//zText2.AddText(1, 1, 0xFFffffff, 0x0, "其他角色即使已受重伤", 1);
	//zText2.AddText(1, 1, 0xFFffffff, 0x0, "凭借出色的生存能力战士可以冒险", 1);
	//zText2.AddText(1, 1, 0xFFffffff, 0x0, "独自行动而不会遇到任何困难", 1);
	//zText2.AddText(1, 1, 0xFFffffff, 0x0, "他可以穿戴各种不同的盔甲", 1);
	//zText2.AddText(1, 1, 0xFFffffff, 0x0, "使用各种不同的武器如剑锤或矛", 1);
	//zText2.AddText(1, 1, 0xFFffffff, 0x0, "战士的技能消耗能量不多", 1);
	//zText2.AddText(1, 1, 0xFFffffff, 0x0, "因此战士通常是新玩家的选择", 1);
	//zText2.AddText(1, 1, 0xFFffffff, 0x0, "此外战士将获得特殊技能", 1);
	//zText2.AddText(1, 1, 0xFFffffff, 0x0, "当骑乘怪鸟时", 1);


	//zText3.SetFont(CreateFontA(13, 0, 0, 0, 400, 0, 0, 0, 0x1, 0, 0, 3, 0, "Tahoma"));
	////alpha B G R
	//zText3.AddText(1, 3, 0xFF00FFFF, 0x0, "介绍精灵", 1);
	//zText3.AddText(1, 1, 0xFFffffff, 0x0, "诺丽亚的精灵属于古老的血统", 1);
	//zText3.AddText(1, 1, 0xFFffffff, 0x0, "他们认为自己是人类之前MU最早的种族", 1);
	//zText3.AddText(1, 1, 0xFFffffff, 0x0, "诺丽亚的一位精灵流传着他们拥有神秘之美", 1);
	//zText3.AddText(1, 1, 0xFFffffff, 0x0, "虽然他们看似回避战斗但不能说", 1);
	//zText3.AddText(1, 1, 0xFFffffff, 0x0, "他们胆小精灵们教会了人类精湛的", 1);
	//zText3.AddText(1, 1, 0xFFffffff, 0x0, "弓术没有人能在弓箭战斗中战胜精灵", 1);
	//zText3.AddText(1, 1, 0xFFffffff, 0x0, "诺丽亚的精灵也与其他种族有契约", 1);
	//zText3.AddText(1, 1, 0xFFffffff, 0x0, "也与其他种族有契约", 1);
	//zText3.AddText(1, 1, 0xFFffffff, 0x0, "而那些契约将他们卷入了战争", 1);


	//zText4.SetFont(CreateFontA(13, 0, 0, 0, 400, 0, 0, 0, 0x1, 0, 0, 3, 0, "Tahoma"));
	////alpha B G R
	//zText4.AddText(1, 3, 0xFF00FFFF, 0x0, "介绍魔剑士", 1);
	//zText4.AddText(1, 1, 0xFFffffff, 0x0, "魔剑士可以使用战士和巫师的多种技能", 1);
	//zText4.AddText(1, 1, 0xFFffffff, 0x0, "尽管战斗技能和魔法不如", 1);
	//zText4.AddText(1, 1, 0xFFffffff, 0x0, "战士或巫师但这种结合使他成为一个", 1);
	//zText4.AddText(1, 1, 0xFFffffff, 0x0, "最具突破性的角色类型", 1);
	//zText4.AddText(1, 1, 0xFFffffff, 0x0, "魔剑士能够施展决定性打击", 1);
	//zText4.AddText(1, 1, 0xFFffffff, 0x0, "他们可以使用强力装备或进入", 1);
	//zText4.AddText(1, 1, 0xFFffffff, 0x0, "等级要求低于其他角色类型的区域", 1);
	//zText4.AddText(1, 1, 0xFFffffff, 0x0, "也与其他种族有契约", 1);
	//zText4.AddText(1, 1, 0xFFffffff, 0x0, "要转职魔剑士你的等级必须达到220级", 1);


	//zText5.SetFont(CreateFontA(13, 0, 0, 0, 400, 0, 0, 0, 0x1, 0, 0, 3, 0, "Tahoma"));
	////alpha B G R
	//zText5.AddText(1, 3, 0xFF00FFFF, 0x0, "介绍领主", 1);
	//zText5.AddText(1, 1, 0xFFffffff, 0x0, "领主挥剑示意展现出领袖的威严", 1);
	//zText5.AddText(1, 1, 0xFFffffff, 0x0, "在战斗中他们拥有召唤暗影之鸦和战马的力量", 1);
	//zText5.AddText(1, 1, 0xFFffffff, 0x0, "助他们镇压敌人像魔剑士一样他们可以进入", 1);
	//zText5.AddText(1, 1, 0xFFffffff, 0x0, "那些区域并穿戴高级装备", 1);
	//zText5.AddText(1, 1, 0xFFffffff, 0x0, "要求低于其他角色类型此外", 1);
	//zText5.AddText(1, 1, 0xFFffffff, 0x0, "他们拥有全新的魔法和技术", 1);
	//zText5.AddText(1, 1, 0xFFffffff, 0x0, "在MU大陆上从未见过的最后要获得领主职业", 1);
	//zText5.AddText(1, 1, 0xFFffffff, 0x0, "需要达到等级250", 1);

	//zText6.SetFont(CreateFontA(13, 0, 0, 0, 400, 0, 0, 0, 0x1, 0, 0, 3, 0, "Tahoma"));
	////alpha B G R
	//zText6.AddText(1, 3, 0xFF00FFFF, 0x0, "介绍召唤术士", 1);
	//zText6.AddText(1, 1, 0xFFffffff, 0x0, "召唤术士诞生于艾尔贝兰隐者之村", 1);
	//zText6.AddText(1, 1, 0xFFffffff, 0x0, "位于MU大陆的西南方", 1);
	//zText6.AddText(1, 1, 0xFFffffff, 0x0, "据古老传说这里是拥有", 1);
	//zText6.AddText(1, 1, 0xFFffffff, 0x0, "超自然能力之人的居所他们非常敏感且易于", 1);
	//zText6.AddText(1, 1, 0xFFffffff, 0x0, "与未超度的灵魂交流由于这一起源特点", 1);
	//zText6.AddText(1, 1, 0xFFffffff, 0x0, "召唤术士继承了先祖所有的魔法", 1);
	//zText6.AddText(1, 1, 0xFFffffff, 0x0, "此外他们还拥有吸血和召唤灵魂的技能", 1);
	//zText6.AddText(1, 1, 0xFFffffff, 0x0, "来支援自身被灵魂击中的对手", 1);
	//zText6.AddText(1, 1, 0xFFffffff, 0x0, "将中诅咒并损失大量生命值", 1);

	//zText7.SetFont(CreateFontA(13, 0, 0, 0, 400, 0, 0, 0, 0x1, 0, 0, 3, 0, "Tahoma"));
	////alpha B G R
	//zText7.AddText(1, 3, 0xFF00FFFF, 0x0, "介绍铁血战士", 1);
	//zText7.AddText(1, 1, 0xFFffffff, 0x0, "铁血战士是皇家骑士军团的后裔", 1);
	//zText7.AddText(1, 1, 0xFFffffff, 0x0, "在卡尔鲁坦王国铁血战士成功", 1);
	//zText7.AddText(1, 1, 0xFFffffff, 0x0, "运用以身体力量为优势的战术", 1);
	//zText7.AddText(1, 1, 0xFFffffff, 0x0, "凭借敏捷的移动和出其不意的攻击", 1);
	//zText7.AddText(1, 1, 0xFFffffff, 0x0, "铁血战士总能令对手陷入被动", 1);
	//zText7.AddText(1, 1, 0xFFffffff, 0x0, "措手不及并迅速击倒对手", 1);
	//zText7.AddText(1, 1, 0xFFffffff, 0x0, "凭借其庞大的身躯此外这个角色也", 1);
	//zText7.AddText(1, 1, 0xFFffffff, 0x0, "具有远程消灭敌人的能力造成巨大伤害", 1);
	//zText7.AddText(1, 1, 0xFFffffff, 0x0, "源自自身的能量", 1);
}
