#include "pch_script.h"
#include "UIWpnParams.h"
#include "UIXmlInit.h"
#include "Level.h"
#include "game_base_space.h"
#include "xrScriptEngine/script_engine.hpp"
#include "inventory_item_object.h"
#include "UIInventoryUtilities.h"
#include "Weapon.h"
#include "Actor.h"
#include "ActorCondition.h"
#include "UIHelper.h"

struct SLuaWpnParams
{
    luabind::functor<float> m_functorAccuracy;
    luabind::functor<float> m_functorDamageMP;
    luabind::functor<float> m_functorHandling;
    luabind::functor<float> m_functorRecoil;
    luabind::functor<float> m_functorReliability;
    luabind::functor<u32> m_functorRPM;
    luabind::functor<float> m_functorDamage;
    luabind::functor<float> m_functorDamageBurn;
    luabind::functor<float> m_functorDamageShock;
    luabind::functor<float> m_functorDamageAcid;
    luabind::functor<float> m_functorDamageWound;
    luabind::functor<float> m_functorDamageStrike;
    luabind::functor<float> m_functorDamageExplosion;
    luabind::functor<u32> m_functorBSpd;

    SLuaWpnParams();
    ~SLuaWpnParams();
};

static SLuaWpnParams* g_lua_wpn_params = nullptr;

SLuaWpnParams::SLuaWpnParams()
{
    [[maybe_unused]] bool functor_exists;
    functor_exists = GEnv.ScriptEngine->functor("ui_wpn_params.GetRPM", m_functorRPM);
    VERIFY(functor_exists);
    functor_exists = GEnv.ScriptEngine->functor("ui_wpn_params.GetDamage", m_functorDamage);
    VERIFY(functor_exists);
    functor_exists = GEnv.ScriptEngine->functor("ui_wpn_params.GetDamageBurn", m_functorDamageBurn);
    VERIFY(functor_exists);
    functor_exists = GEnv.ScriptEngine->functor("ui_wpn_params.GetDamageShock", m_functorDamageShock);
    VERIFY(functor_exists);
    functor_exists = GEnv.ScriptEngine->functor("ui_wpn_params.GetDamageAcid", m_functorDamageAcid);
    VERIFY(functor_exists);
    functor_exists = GEnv.ScriptEngine->functor("ui_wpn_params.GetDamageWound", m_functorDamageWound);
    VERIFY(functor_exists);
    functor_exists = GEnv.ScriptEngine->functor("ui_wpn_params.GetDamageStrike", m_functorDamageStrike);
    VERIFY(functor_exists);
    functor_exists = GEnv.ScriptEngine->functor("ui_wpn_params.GetDamageExplosion", m_functorDamageExplosion);
    VERIFY(functor_exists);
    functor_exists = GEnv.ScriptEngine->functor("ui_wpn_params.GetDamageMP", m_functorDamageMP);
    VERIFY(functor_exists);
    functor_exists = GEnv.ScriptEngine->functor("ui_wpn_params.GetHandling", m_functorHandling);
    VERIFY(functor_exists);
    functor_exists = GEnv.ScriptEngine->functor("ui_wpn_params.GetAccuracy", m_functorAccuracy);
    VERIFY(functor_exists);
    functor_exists = GEnv.ScriptEngine->functor("ui_wpn_params.GetBSpd", m_functorBSpd);
    VERIFY(functor_exists);
    functor_exists = GEnv.ScriptEngine->functor("ui_wpn_params.GetRecoil", m_functorRecoil);
    VERIFY(functor_exists);
    functor_exists = GEnv.ScriptEngine->functor("ui_wpn_params.GetReliability", m_functorReliability);
    VERIFY(functor_exists);
}

SLuaWpnParams::~SLuaWpnParams() {}

// =====================================================================

CUIWpnParams::CUIWpnParams() : CUIWindow("Weapon Params")
{
    AttachChild(&m_textAccuracy);
    AttachChild(&m_textDamage);
    AttachChild(&m_textDamageBurn);
    AttachChild(&m_textDamageShock);
    AttachChild(&m_textDamageAcid);
    AttachChild(&m_textDamageWound);
    AttachChild(&m_textDamageStrike);
    AttachChild(&m_textDamageExplosion);
    AttachChild(&m_textHandling);
    AttachChild(&m_textRPM);
    AttachChild(&m_textBSpd);
    AttachChild(&m_textRecoil);
    AttachChild(&m_textReliability);

    AttachChild(&m_progressAccuracy);
    AttachChild(&m_progressRecoil);
    AttachChild(&m_progressHandling);
    AttachChild(&m_progressReliability);
}

bool CUIWpnParams::InitFromXml(CUIXml& xml_doc)
{
    if (!xml_doc.NavigateToNode("wpn_params", 0))
        return false;

    CUIXmlInit::InitWindow(xml_doc, "wpn_params", 0, this);

    m_Prop_line = UIHelper::CreateStatic(xml_doc, "wpn_params:prop_line", this, false);

    m_icon_acc = UIHelper::CreateStatic(xml_doc, "wpn_params:static_accuracy", this, false);
    m_icon_dam = UIHelper::CreateStatic(xml_doc, "wpn_params:static_damage", this, false);
    m_icon_dbr = UIHelper::CreateStatic(xml_doc, "wpn_params:static_damage_burn", this, false);
    m_icon_dsc = UIHelper::CreateStatic(xml_doc, "wpn_params:static_damage_shock", this, false);
    m_icon_dac = UIHelper::CreateStatic(xml_doc, "wpn_params:static_damage_acid", this, false);
    m_icon_dwn = UIHelper::CreateStatic(xml_doc, "wpn_params:static_damage_wound", this, false);
    m_icon_dst = UIHelper::CreateStatic(xml_doc, "wpn_params:static_damage_strike", this, false);
    m_icon_dex = UIHelper::CreateStatic(xml_doc, "wpn_params:static_damage_explosion", this, false);
    m_icon_han = UIHelper::CreateStatic(xml_doc, "wpn_params:static_handling", this, false);
    m_icon_rpm = UIHelper::CreateStatic(xml_doc, "wpn_params:static_rpm", this, false);
    m_icon_rpm = UIHelper::CreateStatic(xml_doc, "wpn_params:static_bspd", this, false);
    m_icon_han = UIHelper::CreateStatic(xml_doc, "wpn_params:static_recoil", this, false);
    m_icon_rpm = UIHelper::CreateStatic(xml_doc, "wpn_params:static_reliability", this, false);

    CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_accuracy", 0, &m_textAccuracy);
    CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_damage", 0, &m_textDamage);
    CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_damage_burn", 0, &m_textDamageBurn);
    CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_damage_shock", 0, &m_textDamageShock);
    CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_damage_acid", 0, &m_textDamageAcid);
    CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_damage_wound", 0, &m_textDamageWound);
    CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_damage_strike", 0, &m_textDamageStrike);
    CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_damage_explosion", 0, &m_textDamageExplosion);
    CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_handling", 0, &m_textHandling);
    CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_rpm", 0, &m_textRPM);
    CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_bspd", 0, &m_textBSpd);
    CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_recoil", 0, &m_textRecoil);
    CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_reliability", 0, &m_textReliability);

    m_progressAccuracy.InitFromXml(xml_doc, "wpn_params:progress_accuracy");
    m_progressRecoil.InitFromXml(xml_doc, "wpn_params:progress_recoil");
    m_progressHandling.InitFromXml(xml_doc, "wpn_params:progress_handling");
    m_progressReliability.InitFromXml(xml_doc, "wpn_params:progress_reliability");

    m_textBSpdN = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_bspdn", this, false);
    m_textDamageN = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_damagen", this, false);
    m_textDamageBurnN = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_damagen_burn", this, false);
    m_textDamageShockN = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_damagen_shock", this, false);
    m_textDamageAcidN = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_damagen_acid", this, false);
    m_textDamageWoundN = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_damagen_wound", this, false);
    m_textDamageStrikeN = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_damagen_strike", this, false);
    m_textDamageExplosionN = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_damagen_explosion", this, false);
    m_textRPMN = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_rpmn", this, false);

    m_textDamageScale = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_scale_dmg", this, false);
    m_textDamageBurnScale = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_scale_burn", this, false);
    m_textDamageShockScale = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_scale_shock", this, false);
    m_textDamageAcidScale = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_scale_acid", this, false);
    m_textDamageWoundScale = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_scale_wound", this, false);
    m_textDamageStrikeScale = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_scale_strike", this, false);
    m_textDamageExplosionScale = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_scale_explosion", this, false);

    m_textBSpdSlot = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_bspdslot", this, false);
    m_textDamageSlot = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_damageslot", this, false);
    m_textDamageBurnSlot = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_damageslot_burn", this, false);
    m_textDamageShockSlot = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_damageslot_shock", this, false);
    m_textDamageAcidSlot = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_damageslot_acid", this, false);
    m_textDamageWoundSlot = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_damageslot_wound", this, false);
    m_textDamageStrikeSlot = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_damageslot_strike", this, false);
    m_textDamageExplosionSlot = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_damageslot_explosion", this, false);
    m_textRPMSlot = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_rpmslot", this, false);

    m_textDamageSlotScale = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_scaleslot_dmg", this, false);
    m_textDamageBurnSlotScale = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_scaleslot_burn", this, false);
    m_textDamageShockSlotScale = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_scaleslot_shock", this, false);
    m_textDamageAcidSlotScale = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_scaleslot_acid", this, false);
    m_textDamageWoundSlotScale = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_scaleslot_wound", this, false);
    m_textDamageStrikeSlotScale = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_scaleslot_strike", this, false);
    m_textDamageExplosionSlotScale = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_scaleslot_explosion", this, false);

    if (IsGameTypeSingle())
    {
        m_stAmmo = UIHelper::CreateStatic(xml_doc, "wpn_params:static_ammo", this, false);
        m_textAmmoCount = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_ammo_count", this, false);
        m_textAmmoCount2 = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_ammo_count2", this, false);
        m_textAmmoCountSlot = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_ammo_countslot", this, false);
        m_textAmmoTypes = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_ammo_types", this, false);
        m_textAmmoUsedType = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_ammo_used_type", this, false);
        m_stAmmoType1 = UIHelper::CreateStatic(xml_doc, "wpn_params:static_ammo_type1", this, false);
        m_stAmmoType2 = UIHelper::CreateStatic(xml_doc, "wpn_params:static_ammo_type2", this, false);
        m_stAmmoType3 = UIHelper::CreateStatic(xml_doc, "wpn_params:static_ammo_type3", this, false);
        m_stAmmoType4 = UIHelper::CreateStatic(xml_doc, "wpn_params:static_ammo_type4", this, false);
        m_stStatStr = UIHelper::CreateStatic(xml_doc, "wpn_params:static_str", this, false);
        m_scStatStr = UIHelper::CreateStatic(xml_doc, "wpn_params:scale_str", this, false);
        m_textStatStr = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_str", this, false);
        m_stStatDex = UIHelper::CreateStatic(xml_doc, "wpn_params:static_dex", this, false);
        m_scStatDex = UIHelper::CreateStatic(xml_doc, "wpn_params:scale_dex", this, false);
        m_textStatDex = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_dex", this, false);
        m_stStatInt = UIHelper::CreateStatic(xml_doc, "wpn_params:static_int", this, false);
        m_scStatInt = UIHelper::CreateStatic(xml_doc, "wpn_params:scale_int", this, false);
        m_textStatInt = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_int", this, false);
    }
    return true;
}

void CUIWpnParams::SetInfo(CInventoryItem* slot_wpn, CInventoryItem& cur_wpn)
{
    if (!g_lua_wpn_params)
    {
        class CResetEventCb : public CEventNotifierCallbackWithCid
        {
        public:
            CResetEventCb(CID cid) : CEventNotifierCallbackWithCid(cid) {}
            void ProcessEvent() override
            {
                xr_delete(g_lua_wpn_params);
                ai().Unsubscribe(GetCid(), CAI_Space::EVENT_SCRIPT_ENGINE_RESET);
            }
        };

        g_lua_wpn_params = xr_new<SLuaWpnParams>();
        ai().template Subscribe<CResetEventCb>(CAI_Space::EVENT_SCRIPT_ENGINE_RESET);
    }

    LPCSTR cur_section = cur_wpn.object().cNameSect().c_str();
    string2048 str_upgrades;
    str_upgrades[0] = 0;
    cur_wpn.get_upgrades_str(str_upgrades);

    u32 cur_rpm = g_lua_wpn_params->m_functorRPM(cur_section, str_upgrades);
    float cur_accur = iFloor(g_lua_wpn_params->m_functorAccuracy(cur_section, str_upgrades) * 53.0f) / 53.0f;
    float cur_hand = iFloor(g_lua_wpn_params->m_functorHandling(cur_section, str_upgrades) * 53.0f) / 53.0f;
    float cur_damage = g_lua_wpn_params->m_functorDamage(cur_section, str_upgrades);
    float cur_burn = g_lua_wpn_params->m_functorDamageBurn(cur_section, str_upgrades);
    float cur_shock = g_lua_wpn_params->m_functorDamageShock(cur_section, str_upgrades);
    float cur_acid = g_lua_wpn_params->m_functorDamageAcid(cur_section, str_upgrades);
    float cur_wound = g_lua_wpn_params->m_functorDamageWound(cur_section, str_upgrades);
    float cur_strike = g_lua_wpn_params->m_functorDamageStrike(cur_section, str_upgrades);
    float cur_explosion = g_lua_wpn_params->m_functorDamageExplosion(cur_section, str_upgrades);
    u32 cur_bspd = g_lua_wpn_params->m_functorBSpd(cur_section, str_upgrades);
    float cur_recoil = iFloor(g_lua_wpn_params->m_functorRecoil(cur_section, str_upgrades) * 53.0f) / 53.0f;
    float cur_rlb = iFloor(g_lua_wpn_params->m_functorReliability(cur_section, str_upgrades) * 53.0f) / 53.0f;

    float slot_accur = cur_accur;
    float slot_hand = cur_hand;
    float slot_recoil = cur_recoil;
    float slot_rlb = cur_rlb;
    u32 slot_rpm = cur_rpm;
    float slot_damage = cur_damage;
    float slot_burn = cur_burn;
    float slot_shock = cur_shock;
    float slot_acid = cur_acid;
    float slot_wound = cur_wound;
    float slot_strike = cur_strike;
    float slot_explosion = cur_explosion;
    u32 slot_bspd = cur_bspd;

    if (slot_wpn && (slot_wpn != &cur_wpn))
    {
        LPCSTR slot_section = slot_wpn->object().cNameSect().c_str();
        str_upgrades[0] = 0;
        slot_wpn->get_upgrades_str(str_upgrades);

        slot_rpm = g_lua_wpn_params->m_functorRPM(slot_section, str_upgrades);
        slot_accur = iFloor(g_lua_wpn_params->m_functorAccuracy(slot_section, str_upgrades) * 53.0f) / 53.0f;
        slot_hand = iFloor(g_lua_wpn_params->m_functorHandling(slot_section, str_upgrades) * 53.0f) / 53.0f;
        slot_damage = g_lua_wpn_params->m_functorDamage(slot_section, str_upgrades);
        slot_burn = g_lua_wpn_params->m_functorDamageBurn(slot_section, str_upgrades);
        slot_shock = g_lua_wpn_params->m_functorDamageShock(slot_section, str_upgrades);
        slot_acid = g_lua_wpn_params->m_functorDamageAcid(slot_section, str_upgrades);
        slot_wound = g_lua_wpn_params->m_functorDamageWound(slot_section, str_upgrades);
        slot_strike = g_lua_wpn_params->m_functorDamageStrike(slot_section, str_upgrades);
        slot_explosion = g_lua_wpn_params->m_functorDamageExplosion(slot_section, str_upgrades);
        slot_bspd = g_lua_wpn_params->m_functorBSpd(slot_section, str_upgrades);
        slot_recoil = iFloor(g_lua_wpn_params->m_functorRecoil(slot_section, str_upgrades) * 53.0f) / 53.0f;
        slot_rlb = iFloor(g_lua_wpn_params->m_functorReliability(slot_section, str_upgrades) * 53.0f) / 53.0f;
    }

    m_progressAccuracy.SetTwoPos(cur_accur, slot_accur);
    m_progressRecoil.SetTwoPos(cur_recoil, slot_recoil);
    m_progressHandling.SetTwoPos(cur_hand, slot_hand);
    m_progressReliability.SetTwoPos(cur_rlb, slot_rlb);

    if (m_textBSpdN && m_textBSpdSlot)
    {
        m_textBSpdSlot->SetTextColor(color_rgba(170, 170, 170, 255));
        if (cur_bspd == slot_bspd)
        {
            m_textBSpdN->SetTextColor(color_rgba(170, 170, 170, 255));
            m_textBSpdSlot->SetTextColor(color_rgba(255, 255, 255, 0));
        }
        else if (cur_bspd < slot_bspd)
        {
            m_textBSpdN->SetTextColor(color_rgba(255, 0, 0, 255));
        }
        else
        {
            m_textBSpdN->SetTextColor(color_rgba(0, 255, 0, 255));
        }

        string128 str;
        xr_sprintf(str, sizeof(str), "%d", cur_bspd);
        m_textBSpdN->SetText(str);
        xr_sprintf(str, sizeof(str), "%d", slot_bspd);
        m_textBSpdSlot->SetText(str);
    }

    float dmg = 0.f, slot_dmg = 0.f, scale = 0.f, slot_scale = 0.f;
    if (m_textDamageN && m_textDamageSlot && m_textDamageScale && m_textDamageSlotScale)
    {
        dmg = cur_damage + 0.f;
        slot_dmg = slot_damage + 0.f;
        scale = 0.f;
        slot_scale = 0.f;
        calc_weapon_damage(slot_wpn, cur_wpn, dmg, slot_dmg, scale, slot_scale, ALife::eHitTypeFireWound);

        m_textDamageSlot->SetTextColor(color_rgba(170, 170, 170, 255));
        m_textDamageSlotScale->SetTextColor(color_rgba(170, 170, 170, 255));
        if (slot_wpn == &cur_wpn || !slot_wpn)
        {
            m_textDamageSlot->SetTextColor(color_rgba(255, 255, 255, 0));
            m_textDamageSlotScale->SetTextColor(color_rgba(255, 255, 255, 0));
        }
        if (dmg == slot_dmg)
            m_textDamageN->SetTextColor(color_rgba(170, 170, 170, 255));
        else if (dmg < slot_dmg)
            m_textDamageN->SetTextColor(color_rgba(255, 0, 0, 255));
        else
            m_textDamageN->SetTextColor(color_rgba(0, 255, 0, 255));

        if (scale == slot_scale)
            m_textDamageScale->SetTextColor(color_rgba(170, 170, 170, 255));
        else if (scale < slot_scale)
            m_textDamageScale->SetTextColor(color_rgba(255, 0, 0, 255));
        else
            m_textDamageScale->SetTextColor(color_rgba(0, 255, 0, 255));

        string128 str;
        xr_sprintf(str, sizeof(str), "%.0f", dmg);
        m_textDamageN->SetText(str);
        xr_sprintf(str, sizeof(str), "%.0f", slot_dmg);
        m_textDamageSlot->SetText(str);
        xr_sprintf(str, sizeof(str), "%+.0f", scale);
        m_textDamageScale->SetText(str);
        xr_sprintf(str, sizeof(str), "%+.0f", slot_scale);
        m_textDamageSlotScale->SetText(str);
    }

    if (m_textDamageBurnN && m_textDamageBurnSlot && m_textDamageBurnScale && m_textDamageBurnSlotScale)
    {
        dmg = cur_burn + 0.f;
        slot_dmg = slot_burn + 0.f;
        scale = 0.f;
        slot_scale = 0.f;
        calc_weapon_damage(slot_wpn, cur_wpn, dmg, slot_dmg, scale, slot_scale, ALife::eHitTypeBurn);

        m_textDamageBurnSlot->SetTextColor(color_rgba(170, 170, 170, 255));
        m_textDamageBurnSlotScale->SetTextColor(color_rgba(170, 170, 170, 255));
        if (slot_wpn == &cur_wpn || !slot_wpn)
        {
            m_textDamageBurnSlot->SetTextColor(color_rgba(255, 255, 255, 0));
            m_textDamageBurnSlotScale->SetTextColor(color_rgba(255, 255, 255, 0));
        }
        if (dmg == slot_dmg)
            m_textDamageBurnN->SetTextColor(color_rgba(170, 170, 170, 255));
        else if (dmg < slot_dmg)
            m_textDamageBurnN->SetTextColor(color_rgba(255, 0, 0, 255));
        else
            m_textDamageBurnN->SetTextColor(color_rgba(0, 255, 0, 255));

        if (scale == slot_scale)
            m_textDamageBurnScale->SetTextColor(color_rgba(170, 170, 170, 255));
        else if (scale < slot_scale)
            m_textDamageBurnScale->SetTextColor(color_rgba(255, 0, 0, 255));
        else
            m_textDamageBurnScale->SetTextColor(color_rgba(0, 255, 0, 255));

        string128 str;
        xr_sprintf(str, sizeof(str), "%.0f", dmg);
        m_textDamageBurnN->SetText(str);
        xr_sprintf(str, sizeof(str), "%.0f", slot_dmg);
        m_textDamageBurnSlot->SetText(str);
        xr_sprintf(str, sizeof(str), "%+.0f", scale);
        m_textDamageBurnScale->SetText(str);
        xr_sprintf(str, sizeof(str), "%+.0f", slot_scale);
        m_textDamageBurnSlotScale->SetText(str);
    }

    if (m_textDamageShockN && m_textDamageShockSlot && m_textDamageShockScale && m_textDamageShockSlotScale)
    {
        dmg = cur_shock + 0.f;
        slot_dmg = slot_shock + 0.f;
        scale = 0.f;
        slot_scale = 0.f;
        calc_weapon_damage(slot_wpn, cur_wpn, dmg, slot_dmg, scale, slot_scale, ALife::eHitTypeShock);

        m_textDamageShockSlot->SetTextColor(color_rgba(170, 170, 170, 255));
        m_textDamageShockSlotScale->SetTextColor(color_rgba(170, 170, 170, 255));
        if (slot_wpn == &cur_wpn || !slot_wpn)
        {
            m_textDamageShockSlot->SetTextColor(color_rgba(255, 255, 255, 0));
            m_textDamageShockSlotScale->SetTextColor(color_rgba(255, 255, 255, 0));
        }
        if (dmg == slot_dmg)
            m_textDamageShockN->SetTextColor(color_rgba(170, 170, 170, 255));
        else if (dmg < slot_dmg)
            m_textDamageShockN->SetTextColor(color_rgba(255, 0, 0, 255));
        else
            m_textDamageShockN->SetTextColor(color_rgba(0, 255, 0, 255));

        if (scale == slot_scale)
            m_textDamageShockScale->SetTextColor(color_rgba(170, 170, 170, 255));
        else if (scale < slot_scale)
            m_textDamageShockScale->SetTextColor(color_rgba(255, 0, 0, 255));
        else
            m_textDamageShockScale->SetTextColor(color_rgba(0, 255, 0, 255));

        string128 str;
        xr_sprintf(str, sizeof(str), "%.0f", dmg);
        m_textDamageShockN->SetText(str);
        xr_sprintf(str, sizeof(str), "%.0f", slot_dmg);
        m_textDamageShockSlot->SetText(str);
        xr_sprintf(str, sizeof(str), "%+.0f", scale);
        m_textDamageShockScale->SetText(str);
        xr_sprintf(str, sizeof(str), "%+.0f", slot_scale);
        m_textDamageShockSlotScale->SetText(str);
    }

    if (m_textDamageAcidN && m_textDamageAcidSlot && m_textDamageAcidScale && m_textDamageAcidSlotScale)
    {
        dmg = cur_acid + 0.f;
        slot_dmg = slot_acid + 0.f;
        scale = 0.f;
        slot_scale = 0.f;
        calc_weapon_damage(slot_wpn, cur_wpn, dmg, slot_dmg, scale, slot_scale, ALife::eHitTypeChemicalBurn);

        m_textDamageAcidSlot->SetTextColor(color_rgba(170, 170, 170, 255));
        m_textDamageAcidSlotScale->SetTextColor(color_rgba(170, 170, 170, 255));
        if (slot_wpn == &cur_wpn || !slot_wpn)
        {
            m_textDamageAcidSlot->SetTextColor(color_rgba(255, 255, 255, 0));
            m_textDamageAcidSlotScale->SetTextColor(color_rgba(255, 255, 255, 0));
        }
        if (dmg == slot_dmg)
            m_textDamageAcidN->SetTextColor(color_rgba(170, 170, 170, 255));
        else if (dmg < slot_dmg)
            m_textDamageAcidN->SetTextColor(color_rgba(255, 0, 0, 255));
        else
            m_textDamageAcidN->SetTextColor(color_rgba(0, 255, 0, 255));

        if (scale == slot_scale)
            m_textDamageAcidScale->SetTextColor(color_rgba(170, 170, 170, 255));
        else if (scale < slot_scale)
            m_textDamageAcidScale->SetTextColor(color_rgba(255, 0, 0, 255));
        else
            m_textDamageAcidScale->SetTextColor(color_rgba(0, 255, 0, 255));

        string128 str;
        xr_sprintf(str, sizeof(str), "%.0f", dmg);
        m_textDamageAcidN->SetText(str);
        xr_sprintf(str, sizeof(str), "%.0f", slot_dmg);
        m_textDamageAcidSlot->SetText(str);
        xr_sprintf(str, sizeof(str), "%+.0f", scale);
        m_textDamageAcidScale->SetText(str);
        xr_sprintf(str, sizeof(str), "%+.0f", slot_scale);
        m_textDamageAcidSlotScale->SetText(str);
    }

    if (m_textDamageWoundN && m_textDamageWoundSlot && m_textDamageWoundScale && m_textDamageWoundSlotScale)
    {
        dmg = cur_wound + 0.f;
        slot_dmg = slot_wound + 0.f;
        scale = 0.f;
        slot_scale = 0.f;
        calc_weapon_damage(slot_wpn, cur_wpn, dmg, slot_dmg, scale, slot_scale, ALife::eHitTypeWound);

        m_textDamageWoundSlot->SetTextColor(color_rgba(170, 170, 170, 255));
        m_textDamageWoundSlotScale->SetTextColor(color_rgba(170, 170, 170, 255));
        if (slot_wpn == &cur_wpn || !slot_wpn)
        {
            m_textDamageWoundSlot->SetTextColor(color_rgba(255, 255, 255, 0));
            m_textDamageWoundSlotScale->SetTextColor(color_rgba(255, 255, 255, 0));
        }
        if (dmg == slot_dmg)
            m_textDamageWoundN->SetTextColor(color_rgba(170, 170, 170, 255));
        else if (dmg < slot_dmg)
            m_textDamageWoundN->SetTextColor(color_rgba(255, 0, 0, 255));
        else
            m_textDamageWoundN->SetTextColor(color_rgba(0, 255, 0, 255));

        if (scale == slot_scale)
            m_textDamageWoundScale->SetTextColor(color_rgba(170, 170, 170, 255));
        else if (scale < slot_scale)
            m_textDamageWoundScale->SetTextColor(color_rgba(255, 0, 0, 255));
        else
            m_textDamageWoundScale->SetTextColor(color_rgba(0, 255, 0, 255));

        string128 str;
        xr_sprintf(str, sizeof(str), "%.0f", dmg);
        m_textDamageWoundN->SetText(str);
        xr_sprintf(str, sizeof(str), "%.0f", slot_dmg);
        m_textDamageWoundSlot->SetText(str);
        xr_sprintf(str, sizeof(str), "%+.0f", scale);
        m_textDamageWoundScale->SetText(str);
        xr_sprintf(str, sizeof(str), "%+.0f", slot_scale);
        m_textDamageWoundSlotScale->SetText(str);
    }

    if (m_textDamageStrikeN && m_textDamageStrikeSlot && m_textDamageStrikeScale && m_textDamageStrikeSlotScale)
    {
        dmg = cur_strike + 0.f;
        slot_dmg = slot_strike + 0.f;
        scale = 0.f;
        slot_scale = 0.f;
        calc_weapon_damage(slot_wpn, cur_wpn, dmg, slot_dmg, scale, slot_scale, ALife::eHitTypeStrike);

        m_textDamageStrikeSlot->SetTextColor(color_rgba(170, 170, 170, 255));
        m_textDamageStrikeSlotScale->SetTextColor(color_rgba(170, 170, 170, 255));
        if (slot_wpn == &cur_wpn || !slot_wpn)
        {
            m_textDamageStrikeSlot->SetTextColor(color_rgba(255, 255, 255, 0));
            m_textDamageStrikeSlotScale->SetTextColor(color_rgba(255, 255, 255, 0));
        }
        if (dmg == slot_dmg)
            m_textDamageStrikeN->SetTextColor(color_rgba(170, 170, 170, 255));
        else if (dmg < slot_dmg)
            m_textDamageStrikeN->SetTextColor(color_rgba(255, 0, 0, 255));
        else
            m_textDamageStrikeN->SetTextColor(color_rgba(0, 255, 0, 255));

        if (scale == slot_scale)
            m_textDamageStrikeScale->SetTextColor(color_rgba(170, 170, 170, 255));
        else if (scale < slot_scale)
            m_textDamageStrikeScale->SetTextColor(color_rgba(255, 0, 0, 255));
        else
            m_textDamageStrikeScale->SetTextColor(color_rgba(0, 255, 0, 255));

        string128 str;
        xr_sprintf(str, sizeof(str), "%.0f", dmg);
        m_textDamageStrikeN->SetText(str);
        xr_sprintf(str, sizeof(str), "%.0f", slot_dmg);
        m_textDamageStrikeSlot->SetText(str);
        xr_sprintf(str, sizeof(str), "%+.0f", scale);
        m_textDamageStrikeScale->SetText(str);
        xr_sprintf(str, sizeof(str), "%+.0f", slot_scale);
        m_textDamageStrikeSlotScale->SetText(str);
    }

    if (m_textDamageExplosionN && m_textDamageExplosionSlot && m_textDamageExplosionScale &&
        m_textDamageExplosionSlotScale)
    {
        dmg = cur_explosion + 0.f;
        slot_dmg = slot_explosion + 0.f;
        scale = 0.f;
        slot_scale = 0.f;
        calc_weapon_damage(slot_wpn, cur_wpn, dmg, slot_dmg, scale, slot_scale, ALife::eHitTypeExplosion);

        m_textDamageExplosionSlot->SetTextColor(color_rgba(170, 170, 170, 255));
        m_textDamageExplosionSlotScale->SetTextColor(color_rgba(170, 170, 170, 255));
        if (slot_wpn == &cur_wpn || !slot_wpn)
        {
            m_textDamageExplosionSlot->SetTextColor(color_rgba(255, 255, 255, 0));
            m_textDamageExplosionSlotScale->SetTextColor(color_rgba(255, 255, 255, 0));
        }
        if (dmg == slot_dmg)
            m_textDamageExplosionN->SetTextColor(color_rgba(170, 170, 170, 255));
        else if (dmg < slot_dmg)
            m_textDamageExplosionN->SetTextColor(color_rgba(255, 0, 0, 255));
        else
            m_textDamageExplosionN->SetTextColor(color_rgba(0, 255, 0, 255));

        if (scale == slot_scale)
            m_textDamageExplosionScale->SetTextColor(color_rgba(170, 170, 170, 255));
        else if (scale < slot_scale)
            m_textDamageExplosionScale->SetTextColor(color_rgba(255, 0, 0, 255));
        else
            m_textDamageExplosionScale->SetTextColor(color_rgba(0, 255, 0, 255));

        string128 str;
        xr_sprintf(str, sizeof(str), "%.0f", dmg);
        m_textDamageExplosionN->SetText(str);
        xr_sprintf(str, sizeof(str), "%.0f", slot_dmg);
        m_textDamageExplosionSlot->SetText(str);
        xr_sprintf(str, sizeof(str), "%+.0f", scale);
        m_textDamageExplosionScale->SetText(str);
        xr_sprintf(str, sizeof(str), "%+.0f", slot_scale);
        m_textDamageExplosionSlotScale->SetText(str);
    }

    if (m_textRPMN && m_textRPMSlot)
    {
        m_textRPMSlot->SetTextColor(color_rgba(170, 170, 170, 255));
        if (cur_rpm == slot_rpm)
        {
            m_textRPMN->SetTextColor(color_rgba(170, 170, 170, 255));
            m_textRPMSlot->SetTextColor(color_rgba(255, 255, 255, 0));
        }
        else if (cur_rpm < slot_rpm)
        {
            m_textRPMN->SetTextColor(color_rgba(255, 0, 0, 255));
        }
        else
        {
            m_textRPMN->SetTextColor(color_rgba(0, 255, 0, 255));
        }

        string128 str;
        xr_sprintf(str, sizeof(str), "%d", cur_rpm);
        m_textRPMN->SetText(str);
        xr_sprintf(str, sizeof(str), "%d", slot_rpm);
        m_textRPMSlot->SetText(str);
    }

    if (IsGameTypeSingle())
    {
        CWeapon* weapon = cur_wpn.cast_weapon();
        if (!weapon)
            return;

        int ammo_count = weapon->GetAmmoMagSize();
        int ammo_count2 = ammo_count;

        if (slot_wpn)
        {
            CWeapon* slot_weapon = slot_wpn->cast_weapon();
            if (slot_weapon)
                ammo_count2 = slot_weapon->GetAmmoMagSize();
        }

        if (m_textAmmoCount2 && m_textAmmoCountSlot)
        {
            m_textAmmoCountSlot->SetTextColor(color_rgba(170, 170, 170, 255));
            if (ammo_count == ammo_count2)
            {
                m_textAmmoCount2->SetTextColor(color_rgba(170, 170, 170, 255));
                m_textAmmoCountSlot->SetTextColor(color_rgba(255, 255, 255, 0));
            }
            else if (ammo_count < ammo_count2)
            {
                m_textAmmoCount2->SetTextColor(color_rgba(255, 0, 0, 255));
            }
            else
            {
                m_textAmmoCount2->SetTextColor(color_rgba(0, 255, 0, 255));
            }

            string128 str;
            xr_sprintf(str, sizeof(str), "%d", ammo_count);
            m_textAmmoCount2->SetText(str);
            xr_sprintf(str, sizeof(str), "%d", ammo_count2);
            m_textAmmoCountSlot->SetText(str);
        }

        const auto& ammo_types = weapon->m_ammoTypes;
        if (ammo_types.empty())
            return;

        if (m_textAmmoUsedType)
        {
            string128 str;
            xr_sprintf(str, sizeof(str), "%s", pSettings->r_string(ammo_types[0].c_str(), "inv_name_short"));
            m_textAmmoUsedType->SetTextST(str);
        }

        Frect tex_rect;
        if (m_stAmmoType1)
        {
            m_stAmmoType1->SetShader(InventoryUtilities::GetEquipmentIconsShader());
            tex_rect.x1 = float(pSettings->r_u32(ammo_types[0].c_str(), "inv_grid_x") * INV_GRID_WIDTH);
            tex_rect.y1 = float(pSettings->r_u32(ammo_types[0].c_str(), "inv_grid_y") * INV_GRID_HEIGHT);
            tex_rect.x2 = float(pSettings->r_u32(ammo_types[0].c_str(), "inv_grid_width") * INV_GRID_WIDTH);
            tex_rect.y2 = float(pSettings->r_u32(ammo_types[0].c_str(), "inv_grid_height") * INV_GRID_HEIGHT);
            tex_rect.rb.add(tex_rect.lt);
            m_stAmmoType1->SetTextureRect(tex_rect);
            m_stAmmoType1->TextureOn();
            m_stAmmoType1->SetStretchTexture(true);
            m_stAmmoType1->SetWndSize(
                Fvector2().set((tex_rect.x2 - tex_rect.x1) * UI().get_current_kx(), tex_rect.y2 - tex_rect.y1));
        }

        if (m_stAmmoType2)
        {
            m_stAmmoType2->SetShader(InventoryUtilities::GetEquipmentIconsShader());
            if (ammo_types.size() == 1 && m_stAmmoType1)
            {
                tex_rect.set(0, 0, 1, 1);
            }
            else
            {
                tex_rect.x1 = float(pSettings->r_u32(ammo_types[1].c_str(), "inv_grid_x") * INV_GRID_WIDTH);
                tex_rect.y1 = float(pSettings->r_u32(ammo_types[1].c_str(), "inv_grid_y") * INV_GRID_HEIGHT);
                tex_rect.x2 = float(pSettings->r_u32(ammo_types[1].c_str(), "inv_grid_width") * INV_GRID_WIDTH);
                tex_rect.y2 = float(pSettings->r_u32(ammo_types[1].c_str(), "inv_grid_height") * INV_GRID_HEIGHT);
                tex_rect.rb.add(tex_rect.lt);
            }
            m_stAmmoType2->SetTextureRect(tex_rect);
            m_stAmmoType2->TextureOn();
            m_stAmmoType2->SetStretchTexture(true);
            m_stAmmoType2->SetWndSize(
                Fvector2().set((tex_rect.x2 - tex_rect.x1) * UI().get_current_kx(), tex_rect.y2 - tex_rect.y1));
        }
        if (m_stAmmoType3)
        {
            m_stAmmoType3->SetShader(InventoryUtilities::GetEquipmentIconsShader());
            if (ammo_types.size() <= 2 && m_stAmmoType1 && m_stAmmoType2)
            {
                tex_rect.set(0, 0, 1, 1);
            }
            else
            {
                tex_rect.x1 = float(pSettings->r_u32(ammo_types[2].c_str(), "inv_grid_x") * INV_GRID_WIDTH);
                tex_rect.y1 = float(pSettings->r_u32(ammo_types[2].c_str(), "inv_grid_y") * INV_GRID_HEIGHT);
                tex_rect.x2 = float(pSettings->r_u32(ammo_types[2].c_str(), "inv_grid_width") * INV_GRID_WIDTH);
                tex_rect.y2 = float(pSettings->r_u32(ammo_types[2].c_str(), "inv_grid_height") * INV_GRID_HEIGHT);
                tex_rect.rb.add(tex_rect.lt);
            }
            m_stAmmoType3->SetTextureRect(tex_rect);
            m_stAmmoType3->TextureOn();
            m_stAmmoType3->SetStretchTexture(true);
            m_stAmmoType3->SetWndSize(
                Fvector2().set((tex_rect.x2 - tex_rect.x1) * UI().get_current_kx(), tex_rect.y2 - tex_rect.y1));
        }
        if (m_stAmmoType4)
        {
            m_stAmmoType4->SetShader(InventoryUtilities::GetEquipmentIconsShader());
            if (ammo_types.size() <= 3 && m_stAmmoType1 && m_stAmmoType2 && m_stAmmoType3)
            {
                tex_rect.set(0, 0, 1, 1);
            }
            else
            {
                tex_rect.x1 = float(pSettings->r_u32(ammo_types[3].c_str(), "inv_grid_x") * INV_GRID_WIDTH);
                tex_rect.y1 = float(pSettings->r_u32(ammo_types[3].c_str(), "inv_grid_y") * INV_GRID_HEIGHT);
                tex_rect.x2 = float(pSettings->r_u32(ammo_types[3].c_str(), "inv_grid_width") * INV_GRID_WIDTH);
                tex_rect.y2 = float(pSettings->r_u32(ammo_types[3].c_str(), "inv_grid_height") * INV_GRID_HEIGHT);
                tex_rect.rb.add(tex_rect.lt);
            }
            m_stAmmoType4->SetTextureRect(tex_rect);
            m_stAmmoType4->TextureOn();
            m_stAmmoType4->SetStretchTexture(true);
            m_stAmmoType4->SetWndSize(
                Fvector2().set((tex_rect.x2 - tex_rect.x1) * UI().get_current_kx(), tex_rect.y2 - tex_rect.y1));
        }
        if (m_textStatStr)
        {
            
            u8 cur_val = weapon->GetWeaponStr();
            if (Actor()->conditions().GetActorStrength() < cur_val)
                m_textStatStr->SetTextColor(color_rgba(255, 0, 0, 255));
            else
                m_textStatStr->SetTextColor(color_rgba(170, 170, 170, 255));
            string32 str;
            xr_sprintf(str, sizeof(str), "%d", cur_val);
            m_textStatStr->SetText(str);
        }
        if (m_textStatDex)
        {
            u8 cur_val = weapon->GetWeaponDex();
            if (Actor()->conditions().GetActorDexterity() < cur_val)
                m_textStatDex->SetTextColor(color_rgba(255, 0, 0, 255));
            else
                m_textStatDex->SetTextColor(color_rgba(170, 170, 170, 255));
            string32 str;
            xr_sprintf(str, sizeof(str), "%d", cur_val);
            m_textStatDex->SetText(str);
        }
        if (m_textStatInt)
        {
            u8 cur_val = weapon->GetWeaponInt();
            if (Actor()->conditions().GetActorIntelligence() < cur_val)
                m_textStatInt->SetTextColor(color_rgba(255, 0, 0, 255));
            else
                m_textStatInt->SetTextColor(color_rgba(170, 170, 170, 255));
            string32 str;
            xr_sprintf(str, sizeof(str), "%d", cur_val);
            m_textStatInt->SetText(str);
        }
        if (m_scStatStr)
        {
            float sc_val = weapon->GetWeaponStrScale();
            float slot_scale = sc_val;
            if (slot_wpn && (slot_wpn != &cur_wpn))
            {
                CWeapon* slot_weapon = slot_wpn->cast_weapon();
                if (slot_weapon)
                    slot_scale = slot_weapon->GetWeaponStrScale();
            }
            if (slot_scale > sc_val)
                m_scStatStr->SetTextColor(color_rgba(255, 0, 0, 255));
            else if (slot_scale < sc_val)
                m_scStatStr->SetTextColor(color_rgba(0, 255, 0, 255));
            else
                m_scStatStr->SetTextColor(color_rgba(170, 170, 170, 255));
            string16 str;
            xr_sprintf(str, sizeof(str), "%c", GetScaleRating(sc_val));
            m_scStatStr->SetText(str);
        }
        if (m_scStatDex)
        {
            float sc_val = weapon->GetWeaponDexScale();
            float slot_scale = sc_val;
            if (slot_wpn && (slot_wpn != &cur_wpn))
            {
                CWeapon* slot_weapon = slot_wpn->cast_weapon();
                if (slot_weapon)
                    slot_scale = slot_weapon->GetWeaponDexScale();
            }
            if (slot_scale > sc_val)
                m_scStatDex->SetTextColor(color_rgba(255, 0, 0, 255));
            else if (slot_scale < sc_val)
                m_scStatDex->SetTextColor(color_rgba(0, 255, 0, 255));
            else
                m_scStatDex->SetTextColor(color_rgba(170, 170, 170, 255));
            string16 str;
            xr_sprintf(str, sizeof(str), "%c", GetScaleRating(sc_val));
            m_scStatDex->SetText(str);
        }
        if (m_scStatInt)
        {
            float sc_val = weapon->GetWeaponIntScale();
            float slot_scale = sc_val;
            if (slot_wpn && (slot_wpn != &cur_wpn))
            {
                CWeapon* slot_weapon = slot_wpn->cast_weapon();
                if (slot_weapon)
                    slot_scale = slot_weapon->GetWeaponIntScale();
            }
            if (slot_scale > sc_val)
                m_scStatInt->SetTextColor(color_rgba(255, 0, 0, 255));
            else if (slot_scale < sc_val)
                m_scStatInt->SetTextColor(color_rgba(0, 255, 0, 255));
            else
                m_scStatInt->SetTextColor(color_rgba(170, 170, 170, 255));
            string16 str;
            xr_sprintf(str, sizeof(str), "%c", GetScaleRating(sc_val));
            m_scStatInt->SetText(str);
        }
    }
}

bool CUIWpnParams::Check(const shared_str& wpn_section)
{
    if (pSettings->line_exist(wpn_section, "fire_dispersion_base"))
    {
        if (0 == xr_strcmp(wpn_section, "wpn_addon_silencer"))
            return false;
        if (0 == xr_strcmp(wpn_section, "wpn_binoc"))
            return false;
        if (0 == xr_strcmp(wpn_section, "mp_wpn_binoc"))
            return false;

        return true;
    }
    return false;
}

char CUIWpnParams::GetScaleRating(float val) 
{ 
    if (val < .01f)
        return '-';
    else if (val < .3f)
        return 'E';
    else if (val < .5f)
        return 'D';
    else if (val < .8f)
        return 'C';
    else if (val < 1.f)
        return 'B';
    else if (val < 1.4f)
        return 'A';
    else
        return 'S';
}

void CUIWpnParams::calc_weapon_damage(CInventoryItem* slot_wpn, CInventoryItem& cur_wpn, float& dmg, float& dmg_slot,
    float& scale, float& scale_slot, ALife::EHitType type)
{
    float ampl = 0;
    u8 rare = cur_wpn.RarityItem();
    for (int i = 0; i < rare; i++)
    {
        ampl += 0.1 + 0.16 * (i + 1);
    }
    dmg += dmg * ampl;
    if (slot_wpn)
    {
        ampl = 0;
        rare = slot_wpn->RarityItem();
        for (int i = 0; i < rare; i++)
        {
            ampl += 0.1 + 0.16 * (i + 1);
        }
        dmg_slot += dmg_slot * ampl;
    }
    else
        dmg_slot = dmg;

    CWeapon* weapon = cur_wpn.cast_weapon();
    if (weapon)
    {
        switch (type)
        {
        case ALife::eHitTypeBurn:
        case ALife::eHitTypeShock:
        case ALife::eHitTypeChemicalBurn: {
            scale = dmg * weapon->GetWeaponIntScale() *
                (Actor()->conditions().GetActorItemIntScale() + Actor()->conditions().GetActorIntScaleLevel());
            if (weapon->GetWeaponInt() > Actor()->conditions().GetActorIntelligence())
            {
                scale -= dmg * 0.8f;
            }
            break;
        }
        case ALife::eHitTypeWound: {
            scale = dmg * weapon->GetWeaponDexScale() *
                (Actor()->conditions().GetActorItemDexScale() + Actor()->conditions().GetActorDexScaleLevel());
            if (weapon->GetWeaponDex() > Actor()->conditions().GetActorDexterity())
            {
                scale -= dmg * 0.8f;
            }
            break;
        }
        case ALife::eHitTypeStrike:
        case ALife::eHitTypeExplosion: {
            scale = dmg * weapon->GetWeaponStrScale() *
                (Actor()->conditions().GetActorItemStrScale() + Actor()->conditions().GetActorStrScaleLevel());
            if (weapon->GetWeaponStr() > Actor()->conditions().GetActorStrength())
            {
                scale -= dmg * 0.8f;
            }
            break;
        }
        case ALife::eHitTypeFireWound: {
            float percent = 0.f;
            scale = dmg * weapon->GetWeaponStrScale() *
                (Actor()->conditions().GetActorItemStrScale() + Actor()->conditions().GetActorStrScaleLevel());
            scale += dmg * weapon->GetWeaponDexScale() *
                (Actor()->conditions().GetActorItemDexScale() + Actor()->conditions().GetActorDexScaleLevel());
            if (weapon->GetWeaponStr() > Actor()->conditions().GetActorStrength())
            {
                percent = (weapon->GetWeaponStr() + 0.f)/ (weapon->GetWeaponStr() + weapon->GetWeaponDex());
                scale -= dmg * 0.8f * percent;
            }
            if (weapon->GetWeaponDex() > Actor()->conditions().GetActorDexterity())
            {
                percent = (weapon->GetWeaponDex() + 0.f) / (weapon->GetWeaponStr() + weapon->GetWeaponDex());
                scale -= dmg * 0.8f * percent;
            }
            break;
        }
        default: break;
        }
    }
    scale_slot = scale;
    if (slot_wpn)
    {
        CWeapon* weapon_slot = slot_wpn->cast_weapon();
        if (weapon_slot)
        {
            switch (type)
            {
            case ALife::eHitTypeBurn:
            case ALife::eHitTypeShock:
            case ALife::eHitTypeChemicalBurn: {
                scale_slot = dmg_slot * weapon_slot->GetWeaponIntScale() *
                    (Actor()->conditions().GetActorItemIntScale() + Actor()->conditions().GetActorIntScaleLevel());
                if (weapon_slot->GetWeaponInt() > Actor()->conditions().GetActorIntelligence())
                {
                    scale_slot -= dmg_slot * 0.8f;
                }
                break;
            }
            case ALife::eHitTypeWound: {
                scale_slot = dmg_slot * weapon_slot->GetWeaponDexScale() *
                    (Actor()->conditions().GetActorItemDexScale() + Actor()->conditions().GetActorDexScaleLevel());
                if (weapon_slot->GetWeaponDex() > Actor()->conditions().GetActorDexterity())
                {
                    scale_slot -= dmg_slot * 0.8f;
                }
                break;
            }
            case ALife::eHitTypeStrike:
            case ALife::eHitTypeExplosion: {
                scale_slot = dmg_slot * weapon_slot->GetWeaponStrScale() *
                    (Actor()->conditions().GetActorItemStrScale() + Actor()->conditions().GetActorStrScaleLevel());
                if (weapon_slot->GetWeaponStr() > Actor()->conditions().GetActorStrength())
                {
                    scale_slot -= dmg_slot * 0.8f;
                }
                break;
            }
            case ALife::eHitTypeFireWound: {
                float percent = 0.f;
                scale_slot = dmg_slot * weapon_slot->GetWeaponStrScale() *
                    (Actor()->conditions().GetActorItemStrScale() + Actor()->conditions().GetActorStrScaleLevel());
                scale_slot += dmg_slot * weapon_slot->GetWeaponDexScale() *
                    (Actor()->conditions().GetActorItemDexScale() + Actor()->conditions().GetActorDexScaleLevel());
                if (weapon_slot->GetWeaponStr() > Actor()->conditions().GetActorStrength())
                {
                    percent = (weapon_slot->GetWeaponStr() + 0.f) / (weapon_slot->GetWeaponStr() + weapon_slot->GetWeaponDex());
                    scale_slot -= dmg_slot * 0.8f * percent;
                }
                if (weapon_slot->GetWeaponDex() > Actor()->conditions().GetActorDexterity())
                {
                    percent = (weapon_slot->GetWeaponDex() + 0.f) / (weapon_slot->GetWeaponStr() + weapon_slot->GetWeaponDex());
                    scale_slot -= dmg_slot * 0.8f * percent;
                }
                break;
            }
            default: break;
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------

CUIConditionParams::CUIConditionParams()
    : CUIWindow("Condition Params"), m_text("Text")
{
    AttachChild(&m_progress);
    AttachChild(&m_text);
}

bool CUIConditionParams::InitFromXml(CUIXml& xml_doc)
{
    if (!xml_doc.NavigateToNode("condition_params", 0))
        return false;
    CUIXmlInit::InitWindow(xml_doc, "condition_params", 0, this);
    CUIXmlInit::InitStatic(xml_doc, "condition_params:caption", 0, &m_text);
    m_progress.InitFromXml(xml_doc, "condition_params:progress_state");
    return true;
}

void CUIConditionParams::SetInfo(CInventoryItem const* slot_item, CInventoryItem const& cur_item)
{
    float cur_value = cur_item.GetConditionToShow() * 100.0f + 1.0f - EPS;
    float slot_value = cur_value;

    if (slot_item &&
        (slot_item !=
            &cur_item) /*&& (cur_item.object().cNameSect()._get() == slot_item->object().cNameSect()._get())*/)
    {
        slot_value = slot_item->GetConditionToShow() * 100.0f + 1.0f - EPS;
    }
    m_progress.SetTwoPos(cur_value, slot_value);
}
