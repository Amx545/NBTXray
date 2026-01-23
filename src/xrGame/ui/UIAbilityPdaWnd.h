////////////////////////////////////////////////////////////////////////////
//	Module 		: UIAbilityPdaWnd.h
//	Created 	: 03.07.2024
//	Author		: AMX545(NBreak)
//	Description : UI ability window class
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "xrUICore/Windows/UIWindow.h"
#include "xrUICore/Callbacks/UIWndCallback.h"

class CUIStatic;
class CUIXml;

class CUIAbilityPdaWnd : public CUIWindow, public CUIWndCallback
{
public:
    CUIAbilityPdaWnd();
    virtual ~CUIAbilityPdaWnd();

    bool Init();
    virtual void Update();

private:
    typedef CUIWindow inherited;
};
