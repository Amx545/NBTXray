////////////////////////////////////////////////////////////////////////////
//	Module 		: UISkillPdaWnd.cpp
//	Created 	: 01.05.2019
//	Author		: AMX545(NBreak)
//	Description : UI specifications window class
//  Rework      : 10.06.2024
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "UISkillPdaWnd.h"

#include "UIXmlInit.h"
#include "UIHelper.h"
#include "xrUICore/Windows/UIFrameLineWnd.h"
#include "xrUICore/Hint/UIHint.h"
#include "xrUICore/Buttons/UI3tButton.h"

#include "Actor.h"
#include "Actorcondition.h"
#include "ai_space.h"

#include "xrScriptEngine/script_engine.hpp"
#include "UICharacterInfo.h"
#include "xrUICore/ui_base.h"

#define  PDA_SKILL_XML		"pda_skill.xml"


CUISkillPdaWnd::CUISkillPdaWnd() : CUIWindow("CUISkillPdaWnd")
{
    m_btn_valid = true;
	m_previous_time = Device.dwTimeGlobal;
	m_delay = 10;
    m_background = nullptr;
    m_downbackground = nullptr;
    m_Sstats_ramka = nullptr;
    btn_stats_cancel = nullptr;
    btn_stats_apply = nullptr;
    btn_stats_parent = nullptr;
    m_point_store = 0;
    m_vit_store = 0;
    m_dex_store = 0;
    m_str_store = 0;
    m_int_store = 0;
}

CUISkillPdaWnd::~CUISkillPdaWnd()
{ delete_data(m_skill_stats); }

void CUISkillPdaWnd::Update()
{
	inherited::Update();
    UpdateInfo();
}

bool CUISkillPdaWnd::Init()
{
	Fvector2 pos;
	CUIXml xml;
	xml.Load( CONFIG_PATH, UI_PATH, PDA_SKILL_XML );
    CUIXmlInit::InitWindow(xml, "main_wnd", 0, this);
    XML_NODE stored_root = xml.GetLocalRoot();
	m_delay = (u32)xml.ReadAttribInt( "main_wnd", 0, "delay", 500 );

	m_background = UIHelper::CreateFrameWindow(xml, "background", this);
    m_downbackground = UIHelper::CreateFrameWindow(xml, "down_background", this);

    
    constexpr std::tuple<EStatsInfo, cpcstr, bool, bool> stats_list[] = {
        {eStatLevel,        "stat_level",       false, false},
        {eStatPoint,        "stat_point",       false, true}, 
        {eStatExperience,   "stat_experience",  false, false},
        {eStatHealth,       "stat_health",      false, true},
        {eStatStamina,      "stat_stamina",     false, true},
        {eStatPower,        "stat_power",       false, true},
        {eStatVitality,     "stat_vitality",    true,  true}, 
        {eStatStrength,     "stat_strength",    true,  true},
        {eStatIntelligence, "stat_intelligence",true,  true}, 
        {eStatDexterity,    "stat_dexterity",   true,  true}
    };

    for (auto [type, path, button, cval] : stats_list)
    {
        SSkillItem sItem;
        Fvector2 dpos;
        sItem.Item = UIHelper::CreateStatic(xml, path, this);
        Fvector2 pos = sItem.Item->GetWndPos();
        xml.SetLocalRoot(xml.NavigateToNode(path));
        sItem.type = type;
        sItem.value = UIHelper::CreateStatic(xml, "value", this);
        sItem.button_plus = nullptr;
        sItem.button_minus = nullptr;
        sItem.change = nullptr;
        sItem.stored_value = 0;
        if (cval)
        {
            sItem.change = UIHelper::CreateStatic(xml, "value2", this);
            dpos = sItem.change->GetWndPos();
            sItem.change->SetWndPos(dpos.add(pos));
            sItem.type == eStatPoint ? sItem.change->SetTextColor(getIntFromColor(255, 170, 170)) :
                sItem.change->SetTextColor(getIntFromColor(170, 255, 170));
        }
        if (button)
        {
            sItem.button_plus = UIHelper::Create3tButton(xml, "button_plus", this);
            Register(sItem.button_plus);
            sItem.button_minus = UIHelper::Create3tButton(xml, "button_minus", this);
            Register(sItem.button_minus);
            dpos = sItem.button_plus->GetWndPos();
            sItem.button_plus->SetWndPos(dpos.add(pos));
            dpos = sItem.button_minus->GetWndPos();
            sItem.button_minus->SetWndPos(dpos.add(pos));
        }
        dpos = sItem.value->GetWndPos();
        sItem.value->SetWndPos(dpos.add(pos));
        m_skill_stats.push_back(sItem);
        xml.SetLocalRoot(stored_root);
    }
    btn_stats_apply = UIHelper::Create3tButton(xml, "button_apply", this);
    Register(btn_stats_apply);
    btn_stats_cancel = UIHelper::Create3tButton(xml, "button_cancel", this);
    Register(btn_stats_cancel);
    //AddCallback(btn_stats_cancel, BUTTON_DOWN, CUIWndCallback::void_function(this, &CUISkillPdaWnd::OnBtnСancel_Push));
    //AddCallback(btn_stats_apply, BUTTON_DOWN, CUIWndCallback::void_function(this, &CUISkillPdaWnd::OnBtnApply_Push));

	m_Sstats_ramka = UIHelper::CreateFrameWindow(xml, "Sstats_ramka", this);   
    return true;
}

void CUISkillPdaWnd::UpdateInfo() 
{
    CActor* pActor = smart_cast<CActor*>(Level().CurrentViewEntity());
    float cur_val = 0.f;
    float max_val = 0.f;
    u32 cur_point = 0;
    u32 max_point = 0;
    for (auto It = m_skill_stats.begin(); It != m_skill_stats.end(); It++)
    {
        if (!It->Item)
            continue;
        UpdateButtons(*It);
        switch (It->type)
        {
        case (eStatLevel):
            cur_point = pActor->conditions().GetActorLevel();
            SetValue(It->value, "%d", cur_point);
            break;
        case (eStatPoint):
            cur_point = pActor->conditions().GetActorPointLevel();
            SetValue(It->value, "%d", cur_point);
            m_point_store != 0 ? SetValue(It->change, "-%d", m_point_store) : It->change->SetText("");
            break;
        case (eStatExperience):
            cur_point = pActor->conditions().GetActorExperience();
            max_point = pActor->conditions().GetActorRequiredExp();
            SetValue(It->value, "%d", cur_point, max_point);
            break;
        case (eStatHealth):
            cur_val = pActor->conditions().GetHealth();
            max_val = pActor->conditions().GetMaxHealth();
            SetValue(It->value, "%.0f", cur_val * 100, max_val * 100);
            break;
        case (eStatStamina):
            cur_val = pActor->conditions().GetPower();
            max_val = pActor->conditions().GetMaxPower();
            SetValue(It->value, "%.0f", cur_val * 100, max_val * 100);
            break;
        case (eStatPower):
            cur_val = pActor->inventory().GetMaxWeight();
            max_val = pActor->conditions().m_MaxWalkWeight;
            SetValue(It->value, "%.1f", cur_val, max_val);
            break;
        case (eStatVitality):
            cur_point = pActor->conditions().GetActorVitality();
            SetValue(It->value, "%d", cur_point);
            m_vit_store != 0 ? SetValue(It->change, "+%d", m_vit_store) : It->change->SetText("");
            break;
        case (eStatStrength):
            cur_point = pActor->conditions().GetActorStrength();
            SetValue(It->value, "%d", cur_point);
            m_str_store != 0 ? SetValue(It->change, "+%d", m_str_store) : It->change->SetText("");
            break;
        case (eStatIntelligence):
            cur_point = pActor->conditions().GetActorIntelligence();
            SetValue(It->value, "%d", cur_point);
            m_int_store != 0 ? SetValue(It->change, "+%d", m_int_store) : It->change->SetText("");
            break;
        case (eStatDexterity):
            cur_point = pActor->conditions().GetActorDexterity();
            SetValue(It->value, "%d", cur_point);
            m_dex_store != 0 ? SetValue(It->change, "+%d", m_dex_store) : It->change->SetText("");
            break;
        default: break;
        }
    }
}

void CUISkillPdaWnd::SetValue(CUIStatic* value, cpcstr format, float cur, float max)
{
    string32 buf;
    string32 _buf;
    LPCSTR _ultraBuff;
    xr_sprintf(buf, format, cur);
    xr_sprintf(_buf, format, max);
    STRCONCAT(_ultraBuff, buf, "/", _buf);
    value->SetText(_ultraBuff);
}

void CUISkillPdaWnd::SetValue(CUIStatic* value, cpcstr format, float cur)
{
    string32 buf;
    xr_sprintf(buf, format, cur);
    value->SetText(buf);
}

void CUISkillPdaWnd::SetValue(CUIStatic* value, cpcstr format, u32 cur)
{
    string32 buf;
    xr_sprintf(buf, format, cur);
    value->SetText(buf);
}

void CUISkillPdaWnd::SetValue(CUIStatic* value, cpcstr format, u32 cur, u32 max)
{
    string32 buf;
    string32 _buf;
    LPCSTR _ultraBuff;
    xr_sprintf(buf, format, cur);
    xr_sprintf(_buf, format, max);
    STRCONCAT(_ultraBuff, buf, "/", _buf);
    value->SetText(_ultraBuff);
}

void CUISkillPdaWnd::OnBtnApply_Push() 
{
    if (!btn_stats_apply)
        return;
    if (m_skill_stats.empty())
        return;
    if (btn_stats_apply->CursorOverWindow() && btn_stats_apply->GetButtonState() == CUIButton::BUTTON_PUSHED)
    {
        m_btn_valid = false;
        CActor* pActor = smart_cast<CActor*>(Level().CurrentViewEntity());
        if (!pActor)
            return;
        for (auto It = m_skill_stats.begin(); It != m_skill_stats.end(); It++)
        {
            switch (It->type)
            {
            case eStatVitality:
                if (m_vit_store <= 0)
                    continue;
                pActor->conditions().SetActorVitality(m_vit_store + pActor->conditions().GetActorVitality());
                pActor->conditions().SetActorPointLevel(pActor->conditions().GetActorPointLevel() - m_vit_store);
                m_point_store -= m_vit_store;
                m_vit_store = 0;
                break;
            case eStatStrength:
                if (m_str_store <= 0)
                    continue;
                pActor->conditions().SetActorStrength(m_str_store + pActor->conditions().GetActorStrength());
                pActor->conditions().SetActorPointLevel(pActor->conditions().GetActorPointLevel() - m_str_store);
                m_point_store -= m_str_store;
                m_str_store = 0;
                break;
            case eStatIntelligence:
                if (m_int_store <= 0)
                    continue;
                pActor->conditions().SetActorIntelligence(m_int_store + pActor->conditions().GetActorIntelligence());
                pActor->conditions().SetActorPointLevel(pActor->conditions().GetActorPointLevel() - m_int_store);
                m_point_store -= m_int_store;
                m_int_store = 0;
                break;
            case eStatDexterity:
                if (m_dex_store <= 0)
                    continue;
                pActor->conditions().SetActorDexterity(m_dex_store + pActor->conditions().GetActorDexterity());
                pActor->conditions().SetActorPointLevel(pActor->conditions().GetActorPointLevel() - m_dex_store);
                m_point_store -= m_dex_store;
                m_dex_store = 0;
                break;
            default: continue; break;
            }
        }
        //m_point_store = 0;
    }
}

void CUISkillPdaWnd::OnBtnСancel_Push() 
{
    if (!btn_stats_cancel)
        return;
    if (btn_stats_cancel->CursorOverWindow() && btn_stats_cancel->GetButtonState() == CUIButton::BUTTON_PUSHED)
    {
        Log("BtnСancel_Push");
        m_point_store = 0;
        m_vit_store = 0;
        m_dex_store = 0;
        m_str_store = 0;
        m_int_store = 0;
        m_btn_valid = false;
    }
}
bool CUISkillPdaWnd::IsBtnValid()
{
    if (btn_stats_cancel->GetButtonState() != CUIButton::BUTTON_PUSHED &&
        btn_stats_apply->GetButtonState() != CUIButton::BUTTON_PUSHED)
    {
        return true;
    }
    return false;
}

u32 CUISkillPdaWnd::getIntFromColor(u32 Red, u32 Green, u32 Blue)
{
    Red = (Red << 16) & 0x00FF0000;
    Green = (Green << 8) & 0x0000FF00;
    Blue = Blue & 0x000000FF;
    return 0xFF000000 | Red | Green | Blue;
}

void CUISkillPdaWnd::UpdateButtons(SSkillItem &item) 
{
    if (m_btn_valid)
    {
        OnBtnApply_Push();
        OnBtnСancel_Push();
    }
    else
    {
        m_btn_valid = IsBtnValid();
    }
    CActor* pActor = smart_cast<CActor*>(Level().CurrentViewEntity());
    if ((Device.dwTimeGlobal - m_previous_time) <= m_delay)
        return;
    if (!item.button_plus)
        return;
    if (!item.button_minus)
        return;
    if (item.button_plus->CursorOverWindow() && item.button_plus->GetButtonState() == CUIButton::BUTTON_PUSHED)
    {
        if ((pActor->conditions().GetActorPointLevel() - m_point_store) <= 0)
            return;
        switch (item.type)
        {
        case eStatVitality:
            m_vit_store += 1;
            break;
        case eStatStrength:
            m_str_store += 1;
            break;
        case eStatIntelligence:
            m_int_store += 1;
            break;
        case eStatDexterity:
            m_dex_store += 1;
            break;
        default: return; break;
        }
        m_point_store += 1;
        m_previous_time = Device.dwTimeGlobal;
    }
    else if (item.button_minus->CursorOverWindow() && item.button_minus->GetButtonState() == CUIButton::BUTTON_PUSHED)
    {
        switch (item.type)
        {
        case eStatVitality:
            if (m_vit_store <= 0)
                return;
            m_vit_store -= 1;
            break;
        case eStatStrength:
            if (m_str_store <= 0)
                return;
            m_str_store -= 1;
            break;
        case eStatIntelligence:
            if (m_int_store <= 0)
                return;
            m_int_store -= 1;
            break;
        case eStatDexterity:
            if (m_dex_store <= 0)
                return;
            m_dex_store -= 1;
            break;
        default: return; break;
        }
        m_point_store -= 1;
        m_previous_time = Device.dwTimeGlobal;
    }
}
