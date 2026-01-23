#pragma once

#include "WeaponMagazinedWGrenade.h"

class CWeaponRAIK84 : public CWeaponMagazinedWGrenade
{
private:
    typedef CWeaponMagazinedWGrenade inherited;

public:
    CWeaponRAIK84(ESoundTypes eSoundType = SOUND_TYPE_WEAPON_SUBMACHINEGUN);
    virtual ~CWeaponRAIK84();
    virtual void Load(LPCSTR section);
    virtual void PerformSwitchGL();
    virtual void ReloadMagazine();
    virtual bool Action(u16 cmd, u32 flags);
    virtual void LaunchGrenade();
    virtual void UpdateCL();
    void UpdateVis();
    void RenderVis(u32 context_id, IRenderable* root);

private:
    void SpawnMissle(bool bSwitch);

private:
    int m_iPreReloadAmmo;
    u16 m_iGPS;
    float m_fDeltaRPMGL;
    float m_fRPMGL;
    float m_fGLShotTime;
    IKinematics* m_model{};
    shared_str m_sVisualName;
    Fmatrix m_r_transform;
    Fvector m_r_position;
};
