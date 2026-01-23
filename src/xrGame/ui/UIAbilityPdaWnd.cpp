////////////////////////////////////////////////////////////////////////////
//	Module 		: UIAbilityPdaWnd.h
//	Created 	: 03.07.2024
//	Author		: AMX545(NBreak)
//	Description : UI ability window class
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "UIAbilityPdaWnd.h"

#include "UIXmlInit.h"
#include "UIHelper.h"

#include "xrUICore/Windows/UIFrameLineWnd.h"
#include "xrUICore/Buttons/UI3tButton.h"

#include "xrScriptEngine/script_engine.hpp"
#include "UICharacterInfo.h"
#include "xrUICore/ui_base.h"

#define PDA_ABILITY_XML "pda_ability.xml"

CUIAbilityPdaWnd::CUIAbilityPdaWnd() : CUIWindow("CUIAbilityPdaWnd") {}

CUIAbilityPdaWnd::~CUIAbilityPdaWnd() {}

bool CUIAbilityPdaWnd::Init() 
{
	CUIXml xml;
    xml.Load(CONFIG_PATH, UI_PATH, PDA_ABILITY_XML);
    CUIXmlInit::InitWindow(xml, "main_wnd", 0, this);
    XML_NODE stored_root = xml.GetLocalRoot();

    UIHelper::CreateFrameWindow(xml, "background", this);
    UIHelper::CreateFrameWindow(xml, "down_background", this);

    return true; 
}

void CUIAbilityPdaWnd::Update() 
{ 
    inherited::Update(); 
}

