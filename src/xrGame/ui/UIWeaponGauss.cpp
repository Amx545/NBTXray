#include "StdAfx.h"
#include "ui/UIWeaponGauss.h"
#include "player_hud.h"
#include "WeaponGauss.h"
#include "UIHelper.h"
#include "Weapon.h"

void CUIWeaponGauss::GetUILocatorMatrix(Fmatrix& _m) 
{
    Fmatrix trans = m_parent->HudItemData()->m_item_transform;
    shared_str bone_name = pSettings->r_string(m_parent->cNameSect(), "comp_bone");
    u16 bid = m_parent->HudItemData()->m_model->LL_BoneID(bone_name);
    Fmatrix cover_bone = m_parent->HudItemData()->m_model->LL_GetTransform(bid);
    _m.mul(trans, cover_bone);
    _m.mulB_43(m_map_attach_offset);
}

void CUIWeaponGauss::Init(CWeaponGauss* p)
{
    m_parent = p;
    CUIXml Xml;
    Xml.Load(CONFIG_PATH, UI_PATH, UI_PATH_DEFAULT, "ui_gauss_16.xml");

    CUIXmlInit::InitWindow(Xml, "gauss_wnd", 0, this);
    m_main_wnd = xr_new<CUIWindow>("Main wnd");
    CUIXmlInit::InitWindow(Xml, "gauss_wnd:main_wnd", 0, m_main_wnd);
    m_main_wnd->SetAutoDelete(true);
    AttachChild(m_main_wnd);
    m_ammo_count = UIHelper::CreateStatic(Xml, "gauss_wnd:ammo_count", m_main_wnd);
    m_status = UIHelper::CreateStatic(Xml, "gauss_wnd:status", m_main_wnd);
    m_charge = UIHelper::CreateStatic(Xml, "gauss_wnd:charge", m_main_wnd);
    m_cond = UIHelper::CreateStatic(Xml, "gauss_wnd:cond", m_main_wnd);
    m_charge_width = m_charge->GetWidth();
    m_cond_width = m_cond->GetWidth();
    Fvector _map_attach_p = pSettings->r_fvector3(m_parent->cNameSect(), "ui_p");
    Fvector _map_attach_r = pSettings->r_fvector3(m_parent->cNameSect(), "ui_r");
    _map_attach_r.mul(PI / 180.f);
    m_map_attach_offset.setHPB(_map_attach_r.x, _map_attach_r.y, _map_attach_r.z);
    m_map_attach_offset.translate_over(_map_attach_p);
}

void CUIWeaponGauss::Update()
{
    inherited::Update();
    m_charge->SetWidth(m_charge_width * m_fire);
    m_cond->SetWidth(m_cond_width * m_parent->GetCondition());
}

void CUIWeaponGauss::Draw()
{
    Fmatrix LM;
    GetUILocatorMatrix(LM);

    IUIRender::ePointType bk = UI().m_currentPointType;

    UI().m_currentPointType = IUIRender::pttLIT;

    GEnv.UIRender->CacheSetXformWorld(LM);
    GEnv.UIRender->CacheSetCullMode(IUIRender::cmNONE);
    inherited::Draw();

    Fvector2 wrk_sz = m_main_wnd->GetWndSize();
    Fvector2 rp;
    m_main_wnd->GetAbsolutePos(rp);

    Fmatrix M, Mc;
    float h, p;
    Device.vCameraDirection.getHP(h, p);
    Mc.setHPB(h, 0, 0);
    Mc.c.set(Device.vCameraPosition);
    M.invert(Mc);

    UI().ScreenFrustumLIT().CreateFromRect(Frect().set(rp.x, rp.y, wrk_sz.x, wrk_sz.y));
    UI().m_currentPointType = bk;

    string256 texture;
    xr_sprintf(texture, "ui_InGame2_Gauss_Ammo_%d", m_parent->GetAmmoElapsed());
    m_ammo_count->InitTextureEx(texture, "hud\\p3d", FALSE);
    if (m_reload)
    {
        xr_strcpy(texture ,"ui_InGame2_Gauss_Reload");
    }
    else if (m_parent->GetAmmoElapsed() == 0)
    {
        xr_strcpy(texture, "ui_InGame2_Gauss_Empty");
    }
    else if (m_parent->IsMisfire())
    {
        xr_strcpy(texture, "ui_InGame2_Gauss_Misfire");
    }
    else if (m_fire<0.99f)
    {
        xr_strcpy(texture, "ui_InGame2_Gauss_Charging");
    }
    else
    {
        xr_strcpy(texture, "ui_InGame2_Gauss_Charged");
        m_reload = false;
    }
    m_status->InitTextureEx(texture, "hud\\p3d", FALSE);
}

void CUIWeaponGauss::Clear() { }
