#include "StdAfx.h"
#include "UIAmmoInfo.h"
#include "xrUICore/Static/UIStatic.h"
#include "Actor.h"
#include "ActorCondition.h"
#include "Common/object_broker.h"
#include "UIXmlInit.h"
#include "WeaponAmmo.h"
#include "UIHelper.h"

u32 const red_clr = color_argb(255, 210, 50, 50);
u32 const green_clr = color_argb(255, 170, 170, 170);

CUIAmmoParams::~CUIAmmoParams()
{
    xr_delete(m_disp);
    xr_delete(m_damg);
    xr_delete(m_dist);
    xr_delete(m_pirc);
    xr_delete(m_airr);
    xr_delete(m_buck);
    xr_delete(m_impr);
    xr_delete(m_Prop_line);
}

constexpr pcstr af_params = "ammo_params";

bool CUIAmmoParams::InitFromXml(CUIXml& xml)
{
    XML_NODE stored_root = xml.GetLocalRoot();
    XML_NODE base_node = xml.NavigateToNode(af_params, 0);
    if (!base_node)
    {
        return false;
    }
    CUIXmlInit::InitWindow(xml, af_params, 0, this);
    xml.SetLocalRoot(base_node);

    if ((m_Prop_line = UIHelper::CreateStatic(xml, "prop_line", this, false)))
        m_Prop_line->SetAutoDelete(false);


    m_disp = CreateItem(xml, "disp", "ui_inv_accuracy");
    m_damg = CreateItem(xml, "damg", "st_prop_hit");
    m_dist = CreateItem(xml, "dist", "st_prop_bullet_speed");
    m_pirc = CreateItem(xml, "pirc", "st_prop_pierce");
    m_airr = CreateItem(xml, "airr", "st_prop_air_resist");
    m_buck = CreateItem(xml, "buck", "st_prop_bucks");
    m_impr = CreateItem(xml, "impr", "st_prop_impair");

    xml.SetLocalRoot(stored_root);
    return true;
}

CUIAmmoParamItem* CUIAmmoParams::CreateItem(CUIXml& uiXml, pcstr section, float magnitude, bool isSignInverse,
    const shared_str& unit, const shared_str& translationId, const shared_str& translationId2 /*= nullptr*/)
{
    CUIAmmoParamItem* item = xr_new<CUIAmmoParamItem>();

    const CUIAmmoParamItem::InitResult result = item->Init(uiXml, section);
    switch (result)
    {
    case CUIAmmoParamItem::InitResult::Failed: xr_delete(item); return nullptr;
    case CUIAmmoParamItem::InitResult::Plain: item->SetDefaultValuesPlain(magnitude, isSignInverse, unit); break;
    }

    // use either translationId or translationId2
    // but set translationId if both unavailable
    shared_str name = StringTable().translate(translationId);
    shared_str name2 = translationId2 != nullptr ? StringTable().translate(translationId2) : nullptr;

    if (name != translationId && name2 != translationId2)
        item->SetCaption(name2.c_str());
    else
        item->SetCaption(name.c_str());

    item->SetAutoDelete(false);
    return item;
}

CUIAmmoParamItem* CUIAmmoParams::CreateItem(
    CUIXml& uiXml, pcstr section, const shared_str& translationId, const shared_str& translationId2 /*= nullptr*/)
{
    return CreateItem(uiXml, section, 1.0f, false, nullptr, translationId, translationId2);
}

void CUIAmmoParams::SetInfo(CWeaponAmmo* cur_ammo)
{
    DetachAll();
    if (m_Prop_line)
        AttachChild(m_Prop_line);

    CActor* actor = smart_cast<CActor*>(Level().CurrentViewEntity());
    if (!actor)
    {
        return;
    }

    float val = 0.0f, max_val = 1.0f, h = 0.0f;
    if (m_Prop_line)
        h = m_Prop_line->GetWndPos().y + m_Prop_line->GetWndSize().y;

    const auto setValue = [&](CUIAmmoParamItem* item) {
        item->SetValue(val);

        Fvector2 pos = item->GetWndPos();
        pos.y = h;
        item->SetWndPos(pos);

        h += item->GetWndSize().y;
        AttachChild(item);
    };

    if (m_disp)
    {
        val = cur_ammo->cartridge_param.kDisp;
        val = (val - 1.f) * -100;
        if (!fis_zero(val))
        {
            setValue(m_disp);
        }
    }

    if (m_damg)
    {
        val = cur_ammo->cartridge_param.kHit;
        val = (val - 1.f) * 100;
        if (!fis_zero(val))
        {
            setValue(m_damg);
        }
    }

    if (m_dist)
    {
        val = cur_ammo->cartridge_param.kDist;
        val = (val - 1.f) * 100;
        if (!fis_zero(val))
        {
            setValue(m_dist);
        }
    }

    if (m_pirc)
    {
        val = cur_ammo->cartridge_param.kAP;
        val *= 100;
        if (!fis_zero(val))
        {
            setValue(m_pirc);
        }
    }

    if (m_airr)
    {
        val = cur_ammo->cartridge_param.kAirRes;
        val = (val - 1.f) * 100;
        if (!fis_zero(val))
        {
            setValue(m_airr);
        }
    }

    if (m_buck)
    {
        val = (float)cur_ammo->cartridge_param.buckShot;
        if (cur_ammo->cartridge_param.buckShot > 1)
        {
            setValue(m_buck);
        }
    }

    if (m_impr)
    {
        val = cur_ammo->cartridge_param.impair;
        val = (val - 1.f) * 100;
        if (!fis_zero(val))
        {
            setValue(m_impr);
        }
    }

    SetHeight(h);
}

/// ----------------------------------------------------------------

CUIAmmoParamItem::CUIAmmoParamItem()
    : CUIStatic("Ammo Param Item"), m_magnitude(1.0f), m_sign_inverse(false), m_unit_str(""), m_texture_minus(""),
      m_texture_plus("")
{
}

CUIAmmoParamItem::InitResult CUIAmmoParamItem::Init(CUIXml& xml, pcstr section)
{
    if (!CUIXmlInit::InitStatic(xml, section, 0, this, false))
        return InitPlain(xml, section);

    const XML_NODE base_node = xml.GetLocalRoot();
    xml.SetLocalRoot(xml.NavigateToNode(section));

    m_caption = UIHelper::CreateStatic(xml, "caption", this);
    m_value = UIHelper::CreateStatic(xml, "value", this);
    m_magnitude = xml.ReadAttribFlt("value", 0, "magnitude", 1.0f);
    m_sign_inverse = (xml.ReadAttribInt("value", 0, "sign_inverse", 0) == 1);

    LPCSTR unit_str = xml.ReadAttrib("value", 0, "unit_str", "");
    m_unit_str._set(StringTable().translate(unit_str));

    LPCSTR texture_minus = xml.Read("texture_minus", 0, "");
    if (texture_minus && xr_strlen(texture_minus))
    {
        m_texture_minus._set(texture_minus);

        LPCSTR texture_plus = xml.Read("caption:texture", 0, "");
        m_texture_plus._set(texture_plus);
        VERIFY(m_texture_plus.size());
    }
    xml.SetLocalRoot(base_node);
    Log(" ui_ammo_LOAD_Normal ");
    return InitResult::Normal;
}

CUIAmmoParamItem::InitResult CUIAmmoParamItem::InitPlain(CUIXml& xml, pcstr section)
{
    string256 buf;
    strconcat(sizeof(buf), buf, af_params, ":static_", section);
    if (!CUIXmlInit::InitStatic(xml, buf, 0, this, false))
        return InitResult::Failed;

    m_caption = xr_new<CUIStatic>("Caption");
    m_caption->SetAutoDelete(true);
    AttachChild(m_caption);
    m_caption->Show(false); // hack

    m_value = xr_new<CUIStatic>("Value");
    m_value->SetAutoDelete(true);
    AttachChild(m_value);
    m_value->Show(false); // hack

    Log(" ui_ammo_LOAD_Plain ");
    return InitResult::Plain;
}

void CUIAmmoParamItem::SetDefaultValuesPlain(float magnitude, bool isSignInverse, const shared_str& unit)
{
    m_magnitude = magnitude;
    m_sign_inverse = isSignInverse;
    m_unit_str = unit;
}

void CUIAmmoParamItem::SetCaption(LPCSTR name) { m_caption->SetText(name); }

void CUIAmmoParamItem::SetValue(float value)
{
    string32 buf;
    xr_sprintf(buf, "%+.0f", value);

    pstr str;
    if (m_unit_str.size())
    {
        STRCONCAT(str, buf, " ", m_unit_str.c_str());
    }
    else // = ""
    {
        STRCONCAT(str, buf);
    }
    m_value->SetText(str);

    bool positive = (value >= 0.0f);
    positive = (m_sign_inverse) ? !positive : positive;
    u32 color = (positive) ? green_clr : red_clr;
    m_value->SetTextColor(color);

    if (m_texture_minus.size())
    {
        if (positive)
        {
            m_caption->InitTexture(m_texture_plus.c_str());
        }
        else
        {
            m_caption->InitTexture(m_texture_minus.c_str());
        }
    }

    // hack
    if (!m_caption->IsShown() && !m_value->IsShown())
    {
        xr_sprintf(buf, "%s %s", m_caption->GetText(), m_value->GetText());
        SetText(buf);
    }
}
