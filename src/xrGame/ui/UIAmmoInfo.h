#pragma once
#include "xrUICore/Windows/UIWindow.h"
#include "xrServerEntities/alife_space.h"

class CUIXml;
class CUIStatic;
class CUIAmmoParamItem;
class CWeaponAmmo;

class CUIAmmoParams final : public CUIWindow
{
public:
    CUIAmmoParams() : CUIWindow("Ammo Params") {}
    ~CUIAmmoParams() override;
    bool InitFromXml(CUIXml& xml);
    void SetInfo(CWeaponAmmo* cur_ammo);
    pcstr GetDebugType() override { return "CUIAmmoParams"; }

protected:
    CUIAmmoParamItem* CreateItem(
        CUIXml& uiXml, pcstr section, const shared_str& translationId, const shared_str& translationId2 = nullptr);

    CUIAmmoParamItem* CreateItem(CUIXml& uiXml, pcstr section, float magnitude, bool isSignInverse,
        const shared_str& unit, const shared_str& translationId, const shared_str& translationId2 = nullptr);

protected:
    CUIAmmoParamItem* m_disp{};
    CUIAmmoParamItem* m_damg{};
    CUIAmmoParamItem* m_dist{};
    CUIAmmoParamItem* m_pirc{};
    CUIAmmoParamItem* m_airr{};
    CUIAmmoParamItem* m_buck{};
    CUIAmmoParamItem* m_impr{};

    CUIStatic* m_Prop_line{};

}; // class CUIAmmoParams

// -----------------------------------

class CUIAmmoParamItem final : public CUIStatic
{
public:
    CUIAmmoParamItem();

    enum class InitResult
    {
        Failed,
        Normal,
        Plain
    };

    InitResult Init(CUIXml& xml, pcstr section);

    void SetDefaultValuesPlain(float magnitude, bool isSignInverse, const shared_str& unit);
    void SetCaption(LPCSTR name);
    void SetValue(float value);

    pcstr GetDebugType() override { return "CUIAmmoParamItem"; }

protected:
    InitResult InitPlain(CUIXml& xml, pcstr section);

private:
    CUIStatic* m_caption{};
    CUIStatic* m_value{};
    float m_magnitude;
    bool m_sign_inverse;
    shared_str m_unit_str;
    shared_str m_texture_minus;
    shared_str m_texture_plus;

}; // class CUIAmmoParamItem
