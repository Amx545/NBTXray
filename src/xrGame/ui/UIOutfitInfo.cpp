#include "StdAfx.h"
#include "UIOutfitInfo.h"
#include "UIXmlInit.h"
#include "xrUICore/Static/UIStatic.h"
#include "xrUICore/ProgressBar/UIDoubleProgressBar.h"
#include "CustomOutfit.h"
#include "ActorHelmet.h"
#include "ActorGlove.h"
#include "Actor.h"
#include "ActorCondition.h"
#include "player_hud.h"
#include "UIHelper.h"

constexpr std::tuple<cpcstr, cpcstr, const u8, LPCSTR> outfit_info_list[] = {
    {"burn_immunity", "ui_inv_outfit_burn_protection", 0, "burn"}, 
    {"shock_immunity", "ui_inv_outfit_shock_protection", 1, "shock"},
    {"chemical_burn_immunity", "ui_inv_outfit_chemical_burn_protection", 2, "chemical_burn"}, 
    {"radiation_immunity", "ui_inv_outfit_radiation_protection", 3, "radiation"},
    {"telepatic_immunity", "ui_inv_outfit_telepatic_protection", 4, "telepatic"}, 
    {"wound_immunity", "ui_inv_outfit_wound_protection", 5, "wound"},
    {"fire_wound_immunity", "ui_inv_outfit_fire_wound_protection", 6, "fire_wound"},
    {"fire_wound_head_immunity", "ui_inv_outfit_fire_wound_head_protection", 7, "fire_wound"}, 
    {"strike_immunity", "ui_inv_outfit_strike_protection", 8, "strike"},
    {"explosion_immunity", "ui_inv_outfit_explosion_protection", 9, "explosion"},
    {"artefact_count", "ui_inv_outfit_artefact_count", 10, nullptr},
    {"additional_weight", "ui_inv_outfit_additional_weight", 11, nullptr},
    {"power_loss", "ui_inv_outfit_power_loss", 12, nullptr},
    {"health_restore_speed", "ui_inv_outfit_health_restore_speed", 13, nullptr},
    {"radiation_restore_speed", "ui_inv_outfit_radiation_restore_speed", 14, nullptr},
    {"satiety_restore_speed", "ui_inv_outfit_satiety_restore_speed", 15, nullptr},
    {"power_restore_speed", "ui_inv_outfit_power_restore_speed", 16, nullptr},
    {"bleeding_restore_speed", "ui_inv_outfit_bleeding_restore_speed", 17, nullptr}
};

CUIOutfitImmunity::CUIOutfitImmunity()
    : CUIWindow("CUIOutfitImmunity"), m_name("Name"), m_value("Value"), m_value2("Value2")
{
    AttachChild(&m_name);
    AttachChild(&m_value);
    AttachChild(&m_value2);
    m_magnitude = 1000.0f;

}

bool CUIOutfitImmunity::InitFromXml(CUIXml& xml_doc, LPCSTR base_str, cpcstr name_str, cpcstr desc_str)
{
    CUIXmlInit::InitWindow(xml_doc, base_str, 0, this);

    string256 buf;

    strconcat(sizeof(buf), buf, base_str, ":", name_str);
    if (!CUIXmlInit::InitWindow(xml_doc, buf, 0, this, false))
        return false;

    CUIXmlInit::InitStatic(xml_doc, buf, 0, &m_name);
    m_name.TextItemControl()->SetTextST(desc_str);

    strconcat(sizeof(buf), buf, base_str, ":", name_str, ":static_value");
    if (xml_doc.NavigateToNode(buf, 0))
    {
        CUIXmlInit::InitStatic(xml_doc, buf, 0, &m_value);
        m_value.Show(true);
    }
    else
    {
        m_value.Show(false);
    }
    strconcat(sizeof(buf), buf, base_str, ":", name_str, ":static_value2");
    if (xml_doc.NavigateToNode(buf, 0))
    {
        CUIXmlInit::InitStatic(xml_doc, buf, 0, &m_value2);
        m_value2.Show(true);
    }
    else
    {
        m_value2.Show(false);
    }

    m_magnitude = xml_doc.ReadAttribFlt(buf, 0, "magnitude", 1.0f);
    return true;
}

void CUIOutfitImmunity::SetValue(float cur, float comp, bool items_equal)
{
    cur *= m_magnitude;
    comp *= m_magnitude;
    string32 buf;
    xr_sprintf(buf, sizeof(buf), "%.0f", cur);
    m_value.SetText(buf);
    xr_sprintf(buf, sizeof(buf), "%.0f", comp);
    if (items_equal)
    {
        m_value2.SetText(buf);
        m_value2.Show(true);
    }
    else
    {
        m_value2.Show(false);
    }
    if (cur > comp)
    {
        m_value2.SetTextColor(color_argb(255, 255, 170, 170));
        m_value.SetTextColor(color_argb(255, 170, 255, 170));
    }
    else if (cur < comp)
    {
        m_value2.SetTextColor(color_argb(255, 170, 255, 170));
        m_value.SetTextColor(color_argb(255, 255, 170, 170));
    }
    else
    {
        m_value2.SetTextColor(color_argb(255, 170, 170, 170));
        m_value.SetTextColor(color_argb(255, 170, 170, 170));
    }
}

// ===========================================================================================
void CUIOutfitInfo::InitFromXml(CUIXml& xml_doc)
{
    LPCSTR base_str = "outfit_info";

    CUIXmlInit::InitWindow(xml_doc, base_str, 0, this);

    string128 buf;

    strconcat(sizeof(buf), buf, base_str, ":caption");
    m_caption = UIHelper::CreateStatic(xml_doc, buf, this, false);

    strconcat(sizeof(buf), buf, base_str, ":", "prop_line");
    m_Prop_line = UIHelper::CreateStatic(xml_doc, buf, this, false);

    Fvector2 pos;
    pos.set(0.0f, 0.0f);

    if (m_Prop_line)
        pos.set(0.0f, m_Prop_line->GetWndPos().y + m_Prop_line->GetWndSize().y);
    else if (m_caption)
        pos.set(0.0f, m_caption->GetWndSize().y);

    for (auto [name, descr, idx, htype] : outfit_info_list)
    {
        auto immunity = xr_new<CUIOutfitImmunity>();
        if (!immunity->InitFromXml(xml_doc, base_str, name, descr))
        {
            xr_delete(immunity);
            continue;
        }
        immunity->SetAutoDelete(true);
        AttachChild(immunity);
        immunity->SetWndPos(pos);
        pos.y += immunity->GetWndSize().y;

        m_items[idx] = immunity;
    }

    pos.x = GetWndSize().x;
    SetWndSize(pos);
}

void CUIOutfitInfo::UpdateInfo(CCustomOutfit* cur_outfit, CCustomOutfit* slot_outfit)
{
    CActor* actor = smart_cast<CActor*>(Level().CurrentViewEntity());
    if (!actor || !cur_outfit)
    {
        return;
    }
    Fvector2 pos;
    pos.set(0.0f, 0.0f);
    if (m_Prop_line)
        pos.set(0.0f, m_Prop_line->GetWndPos().y + m_Prop_line->GetWndSize().y);

    for (auto [name, descr, idx, htype] : outfit_info_list)
    {
        if (!m_items[idx])
            continue;
        if (ITEMS_ARRAY_MAX <= idx)
            break;
        float cur = 0.f;
        float slot = 0.f;
        //Для хит тайпов
        if (htype)
        {
            ALife::EHitType hit_type = ALife::g_tfString2HitType(htype);
            cur = cur_outfit->GetDefHitTypeProtection(hit_type);
            if (idx == 7)
            {
                if (!cur_outfit->bIsHelmetAvaliable)
                    cur = cur_outfit->GetHeadDefProtection();
                else
                    cur = 0.f;
            }
            slot = cur;
            if (slot_outfit)
            {
                slot = slot_outfit->GetDefHitTypeProtection(hit_type);
                if (idx == 7)
                {
                    if (!slot_outfit->bIsHelmetAvaliable)
                        slot = slot_outfit->GetHeadDefProtection();
                    else
                        slot = 0.f;
                }
            }
        }
        else
        {
            if (idx == 10)
            {
                cur = float(cur_outfit->get_artefact_count());
                slot = cur;
                if (slot_outfit)
                {
                    slot = float(slot_outfit->get_artefact_count());
                }
            }
            if (idx == 11)
            {
                cur = cur_outfit->m_additional_weight;
                slot = cur;
                if (slot_outfit)
                {
                    slot = slot_outfit->m_additional_weight;
                }
            }
            if (idx == 12)
            {
                cur = 1.f - cur_outfit->m_fPowerLoss;
                slot = cur;
                if (slot_outfit)
                {
                    slot = 1.f - slot_outfit->m_fPowerLoss;
                }
            }
            if (idx == 13)
            {
                cur = cur_outfit->m_fHealthRestoreSpeed;
                slot = cur;
                if (slot_outfit)
                {
                    slot = slot_outfit->m_fHealthRestoreSpeed;
                }
            }
            if (idx == 14)
            {
                cur = cur_outfit->m_fRadiationRestoreSpeed;
                slot = cur;
                if (slot_outfit)
                {
                    slot = slot_outfit->m_fRadiationRestoreSpeed;
                }
            }
            if (idx == 15)
            {
                cur = cur_outfit->m_fSatietyRestoreSpeed;
                slot = cur;
                if (slot_outfit)
                {
                    slot = slot_outfit->m_fSatietyRestoreSpeed;
                }
            }
            if (idx == 16)
            {
                cur = cur_outfit->m_fPowerRestoreSpeed;
                slot = cur;
                if (slot_outfit)
                {
                    slot = slot_outfit->m_fPowerRestoreSpeed;
                }
            }
            if (idx == 17)
            {
                cur = cur_outfit->m_fBleedingRestoreSpeed;
                slot = cur;
                if (slot_outfit)
                {
                    slot = slot_outfit->m_fBleedingRestoreSpeed;
                }
            }
        }
        bool l_equal = slot_outfit && (cur_outfit != slot_outfit);
        m_items[idx]->SetValue(cur, slot,l_equal);
        //Убираем нулёвые показатели
        if (fis_zero(cur) && fis_zero(slot))
        {
            m_items[idx]->Show(false);
            continue;
        }
        else if(!m_items[idx]->IsShown())
        {
            m_items[idx]->Show(true);
        }
        // Сдвиг
        m_items[idx]->SetWndPos(pos);
        pos.y += m_items[idx]->GetWndSize().y;
    }
    pos.x = GetWndSize().x;
    SetWndSize(pos);
}

void CUIOutfitInfo::UpdateInfo(CHelmet* cur_helmet, CHelmet* slot_helmet)
{
    CActor* actor = smart_cast<CActor*>(Level().CurrentViewEntity());
    if (!actor || !cur_helmet)
    {
        return;
    }
    Fvector2 pos;
    pos.set(0.0f, 0.0f);
    if (m_Prop_line)
        pos.set(0.0f, m_Prop_line->GetWndPos().y + m_Prop_line->GetWndSize().y);

    for (auto [name, descr, idx, htype] : outfit_info_list)
    {
        if (!m_items[idx])
            continue;
        if (ITEMS_ARRAY_MAX <= idx)
            break;
        float cur = 0.f;
        float slot = 0.f;
        // Для хит тайпов
        if (htype)
        {
            ALife::EHitType hit_type = ALife::g_tfString2HitType(htype);
            cur = cur_helmet->GetDefHitTypeProtection(hit_type);
            if (idx == 6)
            {
                    cur = 0.f;
            }
            slot = cur;
            if (slot_helmet)
            {
                slot = slot_helmet->GetDefHitTypeProtection(hit_type);
                if (idx == 6)
                {
                        slot = 0.f;
                }
            }
        }
        else
        {
            if (idx == 13)
            {
                cur = cur_helmet->m_fHealthRestoreSpeed;
                slot = cur;
                if (slot_helmet)
                {
                        slot = slot_helmet->m_fHealthRestoreSpeed;
                }
            }
            if (idx == 14)
            {
                cur = cur_helmet->m_fRadiationRestoreSpeed;
                slot = cur;
                if (slot_helmet)
                {
                        slot = slot_helmet->m_fRadiationRestoreSpeed;
                }
            }
            if (idx == 15)
            {
                cur = cur_helmet->m_fSatietyRestoreSpeed;
                slot = cur;
                if (slot_helmet)
                {
                        slot = slot_helmet->m_fSatietyRestoreSpeed;
                }
            }
            if (idx == 16)
            {
                cur = cur_helmet->m_fPowerRestoreSpeed;
                slot = cur;
                if (slot_helmet)
                {
                        slot = slot_helmet->m_fPowerRestoreSpeed;
                }
            }
            if (idx == 17)
            {
                cur = cur_helmet->m_fBleedingRestoreSpeed;
                slot = cur;
                if (slot_helmet)
                {
                        slot = slot_helmet->m_fBleedingRestoreSpeed;
                }
            }
        }
        bool l_equal = slot_helmet && (cur_helmet != slot_helmet);
        m_items[idx]->SetValue(cur, slot, l_equal);
        // Убираем нулёвые показатели
        if (fis_zero(cur) && fis_zero(slot))
        {
            m_items[idx]->Show(false);
            continue;
        }
        else if (!m_items[idx]->IsShown())
        {
            m_items[idx]->Show(true);
        }
        // Сдвиг
        m_items[idx]->SetWndPos(pos);
        pos.y += m_items[idx]->GetWndSize().y;
    }
    pos.x = GetWndSize().x;
    SetWndSize(pos);
}

void CUIOutfitInfo::UpdateInfo(CActorGlove* cur_glove, CActorGlove* slot_glove)
{
    CActor* actor = smart_cast<CActor*>(Level().CurrentViewEntity());
    if (!actor || !cur_glove)
    {
        return;
    }
    Fvector2 pos;
    pos.set(0.0f, 0.0f);
    if (m_Prop_line)
        pos.set(0.0f, m_Prop_line->GetWndPos().y + m_Prop_line->GetWndSize().y);

    for (auto [name, descr, idx, htype] : outfit_info_list)
    {
        if (!m_items[idx])
            continue;
        if (ITEMS_ARRAY_MAX <= idx)
            break;
        float cur = 0.f;
        float slot = 0.f;
        // Для хит тайпов
        if (htype)
        {
            ALife::EHitType hit_type = ALife::g_tfString2HitType(htype);
            cur = cur_glove->GetDefHitTypeProtection(hit_type);
            if (idx == 7)
            {
                cur = 0.f;
            }
            slot = cur;
            if (slot_glove)
            {
                slot = slot_glove->GetDefHitTypeProtection(hit_type);
                if (idx == 7)
                {
                        slot = 0.f;
                }
            }
        }
        else
        {
            if (idx == 13)
            {
                cur = cur_glove->m_fHealthRestoreSpeed;
                slot = cur;
                if (slot_glove)
                {
                        slot = slot_glove->m_fHealthRestoreSpeed;
                }
            }
            if (idx == 14)
            {
                cur = cur_glove->m_fRadiationRestoreSpeed;
                slot = cur;
                if (slot_glove)
                {
                        slot = slot_glove->m_fRadiationRestoreSpeed;
                }
            }
            if (idx == 15)
            {
                cur = cur_glove->m_fSatietyRestoreSpeed;
                slot = cur;
                if (slot_glove)
                {
                        slot = slot_glove->m_fSatietyRestoreSpeed;
                }
            }
            if (idx == 16)
            {
                cur = cur_glove->m_fPowerRestoreSpeed;
                slot = cur;
                if (slot_glove)
                {
                        slot = slot_glove->m_fPowerRestoreSpeed;
                }
            }
            if (idx == 17)
            {
                cur = cur_glove->m_fBleedingRestoreSpeed;
                slot = cur;
                if (slot_glove)
                {
                        slot = slot_glove->m_fBleedingRestoreSpeed;
                }
            }
        }
        bool l_equal = slot_glove && (cur_glove != slot_glove);
        m_items[idx]->SetValue(cur, slot, l_equal);
        // Убираем нулёвые показатели
        if (fis_zero(cur) && fis_zero(slot))
        {
            m_items[idx]->Show(false);
            continue;
        }
        else if (!m_items[idx]->IsShown())
        {
            m_items[idx]->Show(true);
        }
        // Сдвиг
        m_items[idx]->SetWndPos(pos);
        pos.y += m_items[idx]->GetWndSize().y;
    }
    pos.x = GetWndSize().x;
    SetWndSize(pos);
}
