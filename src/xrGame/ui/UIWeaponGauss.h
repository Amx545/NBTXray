#pragma once

class CWeaponGauss;
class CUIProgressBar;
class CUIStatic;
class XR_NOVTABLE CUIWeaponGauss final : public CUIWindow
{
    typedef CUIWindow inherited;

    Fmatrix m_map_attach_offset;

    void GetUILocatorMatrix(Fmatrix& _m);

public:
    CUIWeaponGauss() : CUIWindow(CUIWeaponGauss::GetDebugType()) {}

    void Init(CWeaponGauss* p);
    void Update() override;
    void Draw() override;
    void Clear();

    pcstr GetDebugType() override { return "CUIWeaponGauss"; }

private:
    CUIStatic* m_cond;
    CUIStatic* m_charge;
    CUIStatic* m_main;
    CUIStatic* m_ammo_count;
    CUIStatic* m_status;
    CWeaponGauss* m_parent;
    CUIWindow* m_main_wnd;

public:
    bool m_reload{false};
    float m_fire{1.f};
    float m_cond_width{0.f};
    float m_charge_width{0.f};
};
