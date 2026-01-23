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
    char GetScaleRating(float val);
    void calc_weapon_damage(CInventoryItem* slot_wpn, CInventoryItem& cur_wpn, float& dmg, float& dmg_slot,
        float& scale, float& scale_slot, ALife::EHitType type);

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
    CUIStatic* m_icon_dbr;
    CUIStatic* m_icon_dsc;
    CUIStatic* m_icon_dac;
    CUIStatic* m_icon_dwn;
    CUIStatic* m_icon_dst;
    CUIStatic* m_icon_dex;
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
    CUIStatic m_textDamage{"Damage"};
    CUIStatic m_textDamageBurn{"DBrn"};
    CUIStatic m_textDamageShock{"DSck"};
    CUIStatic m_textDamageAcid{"DAcd"};
    CUIStatic m_textDamageWound{"DWnd"};
    CUIStatic m_textDamageStrike{"DStr"};
    CUIStatic m_textDamageExplosion{"DExp"};
    CUIStatic m_textRPM{"RPM"};
    CUIStatic m_textBSpd{"BSpd"};

    CUIStatic* m_textBSpdN;
    CUIStatic* m_textDamageN;
    CUIStatic* m_textDamageBurnN;
    CUIStatic* m_textDamageShockN;
    CUIStatic* m_textDamageAcidN;
    CUIStatic* m_textDamageWoundN;
    CUIStatic* m_textDamageStrikeN;
    CUIStatic* m_textDamageExplosionN;
    CUIStatic* m_textRPMN;

    CUIStatic* m_textDamageScale;
    CUIStatic* m_textDamageBurnScale;
    CUIStatic* m_textDamageShockScale;
    CUIStatic* m_textDamageAcidScale;
    CUIStatic* m_textDamageWoundScale;
    CUIStatic* m_textDamageStrikeScale;
    CUIStatic* m_textDamageExplosionScale;

    CUIStatic* m_textBSpdSlot;
    CUIStatic* m_textDamageSlot;
    CUIStatic* m_textDamageBurnSlot;
    CUIStatic* m_textDamageShockSlot;
    CUIStatic* m_textDamageAcidSlot;
    CUIStatic* m_textDamageWoundSlot;
    CUIStatic* m_textDamageStrikeSlot;
    CUIStatic* m_textDamageExplosionSlot;
    CUIStatic* m_textRPMSlot;
    CUIStatic* m_textAmmoCountSlot;

    CUIStatic* m_textDamageSlotScale;
    CUIStatic* m_textDamageBurnSlotScale;
    CUIStatic* m_textDamageShockSlotScale;
    CUIStatic* m_textDamageAcidSlotScale;
    CUIStatic* m_textDamageWoundSlotScale;
    CUIStatic* m_textDamageStrikeSlotScale;
    CUIStatic* m_textDamageExplosionSlotScale;

    CUIStatic* m_textAmmoTypes;
    CUIStatic* m_textAmmoUsedType;
    CUIStatic* m_textAmmoCount;
    CUIStatic* m_textAmmoCount2;
    CUIStatic* m_stAmmoType1;
    CUIStatic* m_stAmmoType2;
    CUIStatic* m_stAmmoType3;
    CUIStatic* m_stAmmoType4;
    CUIStatic* m_Prop_line;

    CUIStatic* m_stStatStr;
    CUIStatic* m_stStatDex;
    CUIStatic* m_stStatInt;
    CUIStatic* m_scStatStr;
    CUIStatic* m_scStatDex;
    CUIStatic* m_scStatInt;
    CUIStatic* m_textStatStr;
    CUIStatic* m_textStatDex;
    CUIStatic* m_textStatInt;
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
