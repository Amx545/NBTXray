#pragma once
#include "xrUICore/Windows/UIWindow.h"
#include "xrUICore/ProgressBar/UIDoubleProgressBar.h"

class CUIXml;
class CInventoryItem;

struct SLuaWpnParams;

class CUIWpnParams final : public CUIWindow
{
public:
    CUIWpnParams();

    bool InitFromXml(CUIXml& xml_doc);
    void SetInfo(CInventoryItem* slot_wpn, CInventoryItem& cur_wpn);
    bool Check(const shared_str& wpn_section);

    pcstr GetDebugType() override { return "CUIWpnParams"; }

protected:
    CUIDoubleProgressBar m_progressAccuracy; // red or green
    CUIDoubleProgressBar m_progressHandling;
    CUIDoubleProgressBar m_progressRecoil;
    CUIDoubleProgressBar m_progressReliability;
    //CUIDoubleProgressBar m_progressDamage;
    //CUIDoubleProgressBar m_progressRPM;

    CUIStatic* m_icon_acc;
    CUIStatic* m_icon_dam;
    CUIStatic* m_icon_rlb;
    CUIStatic* m_icon_rcl;
    CUIStatic* m_icon_han;
    CUIStatic* m_icon_rpm;
    CUIStatic* m_icon_bsp;

    CUIStatic* m_stAmmo;
    CUIStatic m_textAccuracy{ "Accuracy" };
    CUIStatic m_textHandling{"Handling"};
    CUIStatic m_textRecoil{"Recoil"};
    CUIStatic m_textReliability{"Reliability"};
    CUIStatic m_textDamage{ "Damage" };
    CUIStatic m_textRPM{"RPM"};
    CUIStatic m_textBSpd{"BSpd"};
    CUIStatic* m_textBSpdN;
    CUIStatic* m_textDamageN;
    CUIStatic* m_textRPMN;
    CUIStatic* m_textBSpdSlot;
    CUIStatic* m_textDamageSlot;
    CUIStatic* m_textRPMSlot;
    CUIStatic* m_textAmmoCountSlot;
    CUIStatic* m_textAmmoTypes;
    CUIStatic* m_textAmmoUsedType;
    CUIStatic* m_textAmmoCount;
    CUIStatic* m_textAmmoCount2;
    CUIStatic* m_stAmmoType1;
    CUIStatic* m_stAmmoType2;
    CUIStatic* m_stAmmoType3;
    CUIStatic* m_stAmmoType4;
    CUIStatic* m_Prop_line;
};

// -------------------------------------------------------------------------------------------------

class CUIConditionParams final : public CUIWindow
{
public:
    CUIConditionParams();

    bool InitFromXml(CUIXml& xml_doc);
    void SetInfo(CInventoryItem const* slot_wpn, CInventoryItem const& cur_wpn);

    pcstr GetDebugType() override { return "CUIConditionParams"; }

protected:
    CUIDoubleProgressBar m_progress; // red or green
    CUIStatic m_text;
};
