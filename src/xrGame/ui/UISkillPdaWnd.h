////////////////////////////////////////////////////////////////////////////
//	Module 		: UISkillPdaWnd.h
//	Created 	: 01.05.2019
//	Author		: AMX545(NBreak)
//	Description : UI specifications window class
//  Rework      : 10.06.2024
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "xrUICore/Windows/UIWindow.h"
#include "xrUICore/Callbacks/UIWndCallback.h"


class CUIStatic;
class CUIXml;
class CUIProgressBar;
class CUIFrameLineWnd;
class CUIFrameWindow;
class CUICharacterInfo;
class CUIScrollView;
class UIHint;
class UIHintWindow;
class CUI3tButton;
class CUISkillPdaItem;
enum EStatsInfo
{
    eStatLevel = 0,
    eStatPoint,
    eStatHealth,
    eStatStamina,
    eStatExperience,
    eStatPower,
    eStatVitality,
    eStatStrength,
    eStatIntelligence,
    eStatDexterity,
    eStatEnd
};
struct SSkillItem
{
    EStatsInfo type;
    CUI3tButton* button_plus;
    CUI3tButton* button_minus;
    CUIStatic* value;
    CUIStatic* change;
    CUIStatic* Item;
    u32 stored_value;
};
class CUISkillPdaWnd : public CUIWindow, public CUIWndCallback
{
public:
    CUISkillPdaWnd();
    virtual ~CUISkillPdaWnd();

    virtual void Update();
    bool Init();
    void UpdateInfo();
    void SetValue(CUIStatic* value, cpcstr format, float cur, float max);
    void SetValue(CUIStatic* value, cpcstr format, float cur);
    void SetValue(CUIStatic* value, cpcstr format, u32 cur, u32 max);
    void SetValue(CUIStatic* value, cpcstr format, u32 cur);

private:
	typedef CUIWindow	inherited;
	CUIFrameWindow*	m_Sstats_ramka;
	CUIFrameWindow*	m_background;
    CUIFrameWindow* m_downbackground;

    CUIStatic* btn_stats_parent;
    CUI3tButton* btn_stats_apply;
    CUI3tButton* btn_stats_cancel;

    xr_vector<SSkillItem> m_skill_stats;
    bool m_btn_valid;
	u32	m_delay;
    u32 m_previous_time;

    u32 m_point_store;
    u32 m_vit_store;
    u32 m_dex_store;
    u32 m_str_store;
    u32 m_int_store;

private:
    void OnBtnApply_Push();
    void OnBtnСancel_Push();
    bool IsBtnValid();
    u32 getIntFromColor(u32 Red, u32 Green, u32 Blue);
    void UpdateButtons(SSkillItem& item);


};
