#include "StdAfx.h"
#include "WeaponRAIK84.h"
#include "ExplosiveRocket.h"
#include "player_hud.h"

CWeaponRAIK84::CWeaponRAIK84(ESoundTypes eSoundType) : CWeaponMagazinedWGrenade(eSoundType) 
{
    m_model = nullptr;
    m_iPreReloadAmmo = 0; 
    m_iGPS = 1;
    m_fDeltaRPMGL = 0.f;
    m_fRPMGL = 0.f;
    m_fGLShotTime = 0.f;
    m_r_transform.identity();
}
CWeaponRAIK84::~CWeaponRAIK84()
{
    if (m_model)
    {
        IRenderVisual* v = m_model->dcast_RenderVisual();
        GEnv.Render->model_Delete(v);
    }
}

void CWeaponRAIK84::Load(LPCSTR section) 
{ 
    inherited::Load(section);
    m_iGPS = pSettings->r_u16(section, "grenade_per_shot");
    float fRpmgl = READ_IF_EXISTS(pSettings, r_float, section, "rpm_gl", 200.f);
    m_fRPMGL = 60.f / fRpmgl;

    string128 val_name;

    strconcat(sizeof(val_name), val_name, "raik_position");
    m_r_position = pSettings->r_fvector3(section, val_name);

    m_sVisualName = pSettings->r_string(section, "raik_visual");
    R_ASSERT2(!m_sVisualName.empty(), "Missing 'raik_visual'.");
    GEnv.Render->hud_loading = true;
    m_model = smart_cast<IKinematics*>(GEnv.Render->model_Create(m_sVisualName.c_str()));
    GEnv.Render->hud_loading = false;

    MotionID M2 = m_model->dcast_PKinematicsAnimated()->ID_Cycle_Safe("idle");
    m_model->dcast_PKinematicsAnimated()->PlayCycle(M2, TRUE);
    m_model->CalculateBones_Invalidate();
}

void CWeaponRAIK84::PerformSwitchGL() 
{
    m_bGrenadeMode = !m_bGrenadeMode;
    SpawnMissle(true);
    m_BriefInfo_CalcFrame = 0;
}

void CWeaponRAIK84::ReloadMagazine() 
{
    m_iPreReloadAmmo = iAmmoElapsed;
    inherited::ReloadMagazine();
    // перезарядка подствольного гранатомета
    SpawnMissle(false);
}

bool CWeaponRAIK84::Action(u16 cmd, u32 flags)
{
    if (m_bGrenadeMode && cmd == kWPN_FIRE)
    {
        if (IsPending())
            return false;

        if ((flags & CMD_START) && (m_fGLShotTime < 0))
        {
            if (iAmmoElapsed >= m_iGPS)
                LaunchGrenade();

            if (GetState() == eIdle)
                OnEmptyClick();
        }
        return true;
    }
    if (CWeaponMagazined::Action(cmd, flags))
        return true;

    switch (cmd)
    {
    case kWPN_FUNC: {
        if (flags & CMD_START && !IsPending() && CanSwitchToGL())
            SwitchState(eSwitch);
        return true;
    }
    }
    return false;
}

void CWeaponRAIK84::LaunchGrenade() 
{
    if (!getRocketCount())
        return;
    R_ASSERT(m_bGrenadeMode);
    {
        Fvector p1, d;
        p1.set(get_LastFP2());
        d.set(get_LastFD());
        CEntity* E = smart_cast<CEntity*>(H_Parent());

        if (E)
        {
            CInventoryOwner* io = smart_cast<CInventoryOwner*>(H_Parent());
            if (NULL == io->inventory().ActiveItem())
            {
                Log("current_state", GetState());
                Log("next_state", GetNextState());
                Log("item_sect", cNameSect().c_str());
                Log("H_Parent", H_Parent()->cNameSect().c_str());
            }
            E->g_fireParams(this, p1, d);
        }
        if (IsGameTypeSingle())
            p1.set(get_LastFP2());

        Fmatrix launch_matrix;
        launch_matrix.identity();
        launch_matrix.k.set(d);
        Fvector::generate_orthonormal_basis(launch_matrix.k, launch_matrix.j, launch_matrix.i);

        launch_matrix.c.set(p1);

        if (IsGameTypeSingle() && IsZoomed() && smart_cast<CActor*>(H_Parent()))
        {
            H_Parent()->setEnabled(FALSE);
            setEnabled(FALSE);

            collide::rq_result RQ;
            BOOL HasPick = Level().ObjectSpace.RayPick(p1, d, 300.0f, collide::rqtStatic, RQ, this);

            setEnabled(TRUE);
            H_Parent()->setEnabled(TRUE);

            if (HasPick)
            {
                Fvector Transference;
                Transference.mul(d, RQ.range);
                Fvector res[2];
                u8 canfire0 = TransferenceAndThrowVelToThrowDir(
                    Transference, CRocketLauncher::m_fLaunchSpeed, EffectiveGravity(), res);

                if (canfire0 != 0)
                {
                    d = res[0];
                };
            }
        };

        d.normalize();
        d.mul(CRocketLauncher::m_fLaunchSpeed);
        VERIFY2(_valid(launch_matrix), "CWeaponRAIK84::SwitchState. Invalid launch_matrix!");
        CRocketLauncher::LaunchRocket(launch_matrix, d, zero_vel);

        CExplosiveRocket* pGrenade = smart_cast<CExplosiveRocket*>(getCurrentRocket());
        VERIFY(pGrenade);
        pGrenade->SetInitiator(H_Parent()->ID());

        if (Local() && OnServer())
        {
            VERIFY(m_magazine.size());
            for (u8 j = 0; j < m_iGPS; j++)
            {
                m_magazine.pop_back();
                --iAmmoElapsed;
            }
            VERIFY((u32)iAmmoElapsed == m_magazine.size());
            m_fGLShotTime = m_fRPMGL;

            NET_Packet P;
            u_EventGen(P, GE_LAUNCH_ROCKET, ID());
            P.w_u16(getCurrentRocket()->ID());
            u_EventSend(P);
        };
    }
}

void CWeaponRAIK84::UpdateCL() 
{ 
    inherited::UpdateCL(); 
    m_fDeltaRPMGL = Device.fTimeDelta;
    m_fGLShotTime -= m_fDeltaRPMGL;
    clamp(m_fGLShotTime, -0.01f, flt_max);
    UpdateVis();
}

void CWeaponRAIK84::UpdateVis() 
{
    if (IKinematicsAnimated* ka = m_model->dcast_PKinematicsAnimated())
    {
        ka->UpdateTracks();
        ka->dcast_PKinematics()->CalculateBones_Invalidate();
        ka->dcast_PKinematics()->CalculateBones(TRUE);
    }
}

void CWeaponRAIK84::RenderVis(u32 context_id, IRenderable* root) 
{
    m_r_transform.set(g_player_hud->attached_item(0)->m_item_transform);
    Fmatrix c_mat;
    Fmatrix l_mat;
    c_mat.setHPB(.0f, .0f, .0f);
    c_mat.translate_over(m_r_position);

    l_mat.mul(m_r_transform, c_mat);
    VERIFY(!fis_zero(DET(l_mat)));
    GEnv.Render->add_Visual(context_id, root, m_model->dcast_RenderVisual(), l_mat);
}

void CWeaponRAIK84::SpawnMissle(bool bSwitch)
{
    // перезарядка подствольного гранатомета
    if (m_bGrenadeMode && iAmmoElapsed)
    {
        shared_str fake_grenade_name = pSettings->r_string(m_ammoTypes[m_ammoType].c_str(), "fake_grenade_name");
        int ammo_need = (iAmmoElapsed / m_iGPS) - (m_iPreReloadAmmo / m_iGPS);
        int ammo_limit = (m_iPreReloadAmmo / m_iGPS) - getRocketCount();
        int rocket_to_spawn = ammo_need + ammo_limit;
        if (bSwitch)
            rocket_to_spawn = (iAmmoElapsed / m_iGPS) - getRocketCount();
        //Msg("Mag size:%d, Ammo current: %d, Rocket current: %d, Spawned: %d", iMagazineSize, iAmmoElapsed,getRocketCount(), rocket_to_spawn);
        while (rocket_to_spawn > 0)
        {
            rocket_to_spawn--;
            SpawnRocket(*fake_grenade_name, this);
        }
    }
    //Msg("Rocket current post reload: %d", getRocketCount());
}
