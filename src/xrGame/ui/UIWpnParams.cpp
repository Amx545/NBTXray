#include "pch_script.h"
#include "UIWpnParams.h"
#include "UIXmlInit.h"
#include "Level.h"
#include "game_base_space.h"
#include "xrScriptEngine/script_engine.hpp"
#include "inventory_item_object.h"
#include "UIInventoryUtilities.h"
#include "Weapon.h"
#include "UIHelper.h"

struct SLuaWpnParams
{
    luabind::functor<float> m_functorAccuracy;
    luabind::functor<float> m_functorDamageMP;
    luabind::functor<float> m_functorHandling;
    luabind::functor<float> m_functorRecoil;
    luabind::functor<float> m_functorReliability;
    luabind::functor<u32> m_functorRPM;
    luabind::functor<u32> m_functorDamage;
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
    m_icon_han = UIHelper::CreateStatic(xml_doc, "wpn_params:static_handling", this, false);
    m_icon_rpm = UIHelper::CreateStatic(xml_doc, "wpn_params:static_rpm", this, false);
    m_icon_rpm = UIHelper::CreateStatic(xml_doc, "wpn_params:static_bspd", this, false);
    m_icon_han = UIHelper::CreateStatic(xml_doc, "wpn_params:static_recoil", this, false);
    m_icon_rpm = UIHelper::CreateStatic(xml_doc, "wpn_params:static_reliability", this, false);

    CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_accuracy", 0, &m_textAccuracy);
    CUIXmlInit::InitStatic(xml_doc, "wpn_params:cap_damage", 0, &m_textDamage);
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
    m_textRPMN = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_rpmn", this, false);

    m_textBSpdSlot = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_bspdslot", this, false);
    m_textDamageSlot = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_damageslot", this, false);
    m_textRPMSlot = UIHelper::CreateStatic(xml_doc, "wpn_params:cap_rpmslot", this, false);

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
    u32 cur_damage = g_lua_wpn_params->m_functorDamage(cur_section, str_upgrades);
    u32 cur_bspd = g_lua_wpn_params->m_functorBSpd(cur_section, str_upgrades);
    float cur_recoil = iFloor(g_lua_wpn_params->m_functorRecoil(cur_section, str_upgrades) * 53.0f) / 53.0f;
    float cur_rlb = iFloor(g_lua_wpn_params->m_functorReliability(cur_section, str_upgrades) * 53.0f) / 53.0f;

    float slot_accur = cur_accur;
    float slot_hand = cur_hand;
    float slot_recoil = cur_recoil;
    float slot_rlb = cur_rlb;
    u32 slot_rpm = cur_rpm;
    u32 slot_damage = cur_damage;
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

    if (m_textDamageN && m_textDamageSlot)
    {
        m_textDamageSlot->SetTextColor(color_rgba(170, 170, 170, 255));
        if (cur_damage == slot_damage)
        {
            m_textDamageN->SetTextColor(color_rgba(170, 170, 170, 255));
            m_textDamageSlot->SetTextColor(color_rgba(255, 255, 255, 0));
        }
        else if (cur_damage < slot_damage)
        {
            m_textDamageN->SetTextColor(color_rgba(255, 0, 0, 255));
        }
        else
        {
            m_textDamageN->SetTextColor(color_rgba(0, 255, 0, 255));
        }

        string128 str;
        xr_sprintf(str, sizeof(str), "%d", cur_damage);
        m_textDamageN->SetText(str);
        xr_sprintf(str, sizeof(str), "%d", slot_damage);
        m_textDamageSlot->SetText(str);
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
