#include "StdAfx.h"
#include "WeaponGauss.h"
#include "ActorCondition.h"
#include "ui/UIWeaponGauss.h"

CWeaponGauss::CWeaponGauss() { m_ui = nullptr; }

CWeaponGauss::~CWeaponGauss() { xr_delete(m_ui); }

void CWeaponGauss::Load(LPCSTR section)
{
    inherited::Load(section);
    m_bCompMode = false;
    m_bTimeReload = true;
    m_sounds.LoadSound(section, "snd_switch_off", "sndSwitchOff", true, m_eSoundReload);
    m_sounds.LoadSound(section, "snd_switch_on", "sndSwitchOn", true, m_eSoundReload);
    if (pSettings->line_exist(section, "use_comp_mode"))
    {
        m_bUseSwitchMode = pSettings->r_bool(section, "use_comp_mode") ? true : false;
    }
    else
        m_bUseSwitchMode = false;
    if (m_ui == nullptr && m_bUseSwitchMode)
    {
        m_ui = xr_new<CUIWeaponGauss>();
        ui().Init(this);
    }
}

CUIWeaponGauss& CWeaponGauss::ui() { return *((CUIWeaponGauss*)m_ui); }

void CWeaponGauss::OnH_B_Independent(bool just_before_destroy)
{
    inherited::OnH_B_Independent(just_before_destroy);

    SetPending(FALSE);
    if (m_bCompMode)
    {
        SetState(eIdle);
        SetPending(FALSE);
    }
}

void CWeaponGauss::save(NET_Packet& output_packet)
{
    inherited::save(output_packet);
    save_data(m_bCompMode, output_packet);
}

void CWeaponGauss::load(IReader& input_packet)
{
    inherited::load(input_packet);
    bool b = false;
    load_data(b, input_packet);
    if (b != m_bCompMode)
        SwitchMode();
    m_bCompOpen = b;
}

void CWeaponGauss::shedule_Update(u32 dt) 
{ 
    inherited::shedule_Update(dt); 
    if(m_bUseSwitchMode)
        ui().Update();
    switch (eGaussState)
    {
    case eGaussReload: 
    {
        float speed = 1.f;
        CActor* pActor = smart_cast<CActor*>(this->H_Parent());
        if (pActor)
        {
            speed *= pActor->conditions().GetSpeedReloadPerk();
        }
        u32 reload_start = m_dwMotionStartTm + (u32)((float)1000 / speed);
        u32 reload_end = m_dwMotionStartTm + (u32)((float)3300 / speed);
        if (m_bUseSwitchMode)
        {
            if (m_dwMotionCurrTm > reload_start && m_dwMotionCurrTm < reload_end)
            {
                ui().m_reload = true;
            }
            else
            {
                ui().m_reload = false;
            }
        }
        if (m_dwMotionCurrTm > reload_end)
        {
            eGaussState = eGaussWait;
            ReloadBattery();
        }
        break;
    }
    case eGaussRevivel: 
    {
        u32 revivel_end = m_dwMotionStartTm + 2000;

        if (m_dwMotionCurrTm > revivel_end)
        {
            eGaussState = eGaussWait;
            bMisfire = false;
        }
        break;
    }
    case eGaussFireStart: 
    {
        float shoot_max_time = fOneShotTime * 1000;
        m_shoot_cur_time += dt;
        if (m_bUseSwitchMode)
            ui().m_fire = (float)m_shoot_cur_time / shoot_max_time;
        if (m_shoot_cur_time >= shoot_max_time)
        {
            eGaussState = eGaussWait;
        }
        break;
    }
    case eGaussCompOpen:
    {
        u32 comp_timer_end = m_dwMotionStartTm + 630;

        if (m_dwMotionCurrTm > comp_timer_end)
        {
            m_bCompOpen = true;
            eGaussState = eGaussWait;
        }
        break;
    }
    case eGaussCompClose: {
        u32 comp_timer_end = m_dwMotionStartTm + 340;

        if (m_dwMotionCurrTm > comp_timer_end)
        {
            m_bCompOpen = false;
            eGaussState = eGaussWait;
        }
        break;
    }
    case eGaussWait:
    {
        if ((iAmmoElapsed > 0) && m_bUseSwitchMode)
        {
            ui().m_fire = 1.f;
            ui().m_reload = false;
        }
        break;
    }
    default: break;
    }
}

bool CWeaponGauss::Action(u16 cmd, u32 flags)
{
    if (inherited::Action(cmd, flags))
        return true;

    switch (cmd)
    {
    case kWPN_FUNC: {
        if (flags & CMD_START && !IsPending() && CanSwitchMode())
            SwitchState(eSwitch);
        return true;
    }
    }
    return false;
}

void CWeaponGauss::UpdateSounds()
{
    inherited::UpdateSounds();
    Fvector P = get_LastFP();
    m_sounds.SetPosition("sndSwitchOn", P);
    m_sounds.SetPosition("sndSwitchOff", P);
}

bool CWeaponGauss::SwitchMode()
{
    if (!CanSwitchMode())
        return false;

    OnZoomOut();

    SetPending(TRUE);

    m_bCompMode = !m_bCompMode;
    if (m_bCompMode)
    {
        eGaussState = eGaussCompOpen;
        PlaySound("sndSwitchOff", get_LastFP());
    }
    else
    {
        eGaussState = eGaussCompClose;
        PlaySound("sndSwitchOn", get_LastFP());
    }

    PlayAnimModeSwitch();

    return true;
}

bool CWeaponGauss::CanSwitchMode() 
{
    return ((eIdle == GetState()) || (eHidden == GetState()) || (eMisfire == GetState()) || (eMagEmpty == GetState())) && (!IsPending() && m_bUseSwitchMode);
}

void CWeaponGauss::OnAnimationEnd(u32 state)
{
    switch (state)
    {
    case eSwitch: {
        SwitchState(eIdle);
    }
    break;
    }
    inherited::OnAnimationEnd(state);
}

void CWeaponGauss::OnStateSwitch(u32 S, u32 oldState)
{
    switch (S)
    {
    case eSwitch: {
        if (!SwitchMode())
        {
            SwitchState(eIdle);
            return;
        }
    }
    break;
    }

    inherited::OnStateSwitch(S, oldState);
}

void CWeaponGauss::PlayAnimModeSwitch()
{
    if (m_bCompMode)
    {
        PlayHUDMotion("anm_switch_off", TRUE, this, eSwitch);
    }
    else
    {
        PlayHUDMotion("anm_switch_on", TRUE, this, eSwitch);
    }
}

void CWeaponGauss::PlayAnimShow()
{
    VERIFY(GetState() == eShowing);
    if (m_bCompMode)
    {
        PlayHUDMotion("anm_show_on", FALSE, this, GetState());
    }
    else
        PlayHUDMotion("anm_show", FALSE, this, GetState());
}

void CWeaponGauss::PlayAnimHide()
{
    VERIFY(GetState() == eHiding);
    eGaussState = eGaussWait;
    if (m_bCompMode)
    {
        PlayHUDMotion("anm_hide_on", true, this, GetState());
    }
    else
        PlayHUDMotion("anm_hide", true, this, GetState());
}

void CWeaponGauss::PlayAnimReload()
{
    auto state = GetState();
    VERIFY(state == eReload);
    float speed = 1.f;
    CActor* pActor = smart_cast<CActor*>(this->H_Parent());
    if (pActor)
    {
        speed *= pActor->conditions().GetSpeedReloadPerk();
    }
    if (m_bCompMode)
    {
        if (IsMisfire())
        {
            eGaussState = eGaussRevivel;
            PlayHUDMotion("anm_reload_misfire_on", true, this, state, speed);
        }
        else
        {
            eGaussState = eGaussReload;
            PlayHUDMotion("anm_reload_on", true, this, state, speed);
        }
    }
    else
    inherited::PlayAnimReload();
}

void CWeaponGauss::PlayAnimIdle()
{
    if (m_bCompMode)
    {
        if (IsZoomed())
        {
            PlayHUDMotion("anm_aim_on", TRUE, nullptr, GetState());
        }
        else
        {
            enum class state
            {
                idle,
                sprint,
                moving,
                crouch
            };
            state act_state = state::idle;
            if (const auto actor = smart_cast<CActor*>(H_Parent()))
            {
                CEntity::SEntityState st;
                actor->g_State(st);
                if (st.bSprint)
                    act_state = state::sprint;
                else if (actor->AnyMove())
                {
                    if (st.bCrouch)
                        act_state = state::crouch;
                    else
                        act_state = state::moving;
                }
            }
            switch (act_state)
            {
            case state::idle: 
            {
                PlayHUDMotion("anm_idle_on", TRUE, nullptr, GetState());
                break;
            }
            case state::sprint: {
                PlayHUDMotion("anm_sprint_on", TRUE, nullptr, GetState());
                break;
            }
            case state::crouch: {
                PlayHUDMotion("anm_crouch_on", TRUE, nullptr, GetState());
                break;
                [[fallthrough]];
            }
            case state::moving: {
                PlayHUDMotion("anm_moving_on", TRUE, nullptr, GetState());
                break;

            }
            }
        }
    }
    else
        inherited::PlayAnimIdle();
}

void CWeaponGauss::PlayAnimShoot()
{
    if (m_bCompMode)
    {
        if (iAmmoElapsed > 1)
        {
            eGaussState = eGaussFireStart;
            m_shoot_cur_time = 0;
        }
        if (m_bUseSwitchMode)
            ui().m_fire = 0.f;
        PlayHUDMotion("anm_shot_on", FALSE, this, eFire);
    }
    else
        inherited::PlayAnimShoot();
}

void CWeaponGauss::PlayAnimBore()
{
    if (m_bCompMode)
    {
        PlayHUDMotion("anm_bore_on", FALSE, this, GetState());
    }
    else
        inherited::PlayAnimBore();
}

void CWeaponGauss::ReloadMagazine()
{
    if (!m_bCompMode)
        inherited::ReloadMagazine();
}
void CWeaponGauss::ReloadBattery()
{ 
    m_BriefInfo_CalcFrame = 0;

    // устранить осечку при перезарядке
    if (IsMisfire())
    {
        bMisfire = false;
        if (iAmmoElapsed)
            return;
    }

    if (!m_bLockType)
    {
        m_pCurrentAmmo = nullptr;
    }

    if (!m_pInventory)
        return;

    if (m_set_next_ammoType_on_reload != undefined_ammo_type)
    {
        m_ammoType = m_set_next_ammoType_on_reload;
        m_set_next_ammoType_on_reload = undefined_ammo_type;
    }

    if (!unlimited_ammo())
    {
        if (m_ammoTypes.size() <= m_ammoType)
            return;

        LPCSTR tmp_sect_name = m_ammoTypes[m_ammoType].c_str();

        if (!tmp_sect_name)
            return;

        // попытаться найти в инвентаре патроны текущего типа
        m_pCurrentAmmo = smart_cast<CWeaponAmmo*>(m_pInventory->GetAny(tmp_sect_name));

        if (!m_pCurrentAmmo && !m_bLockType)
        {
            for (u8 i = 0; i < u8(m_ammoTypes.size()); ++i)
            {
                // проверить патроны всех подходящих типов
                m_pCurrentAmmo = smart_cast<CWeaponAmmo*>(m_pInventory->GetAny(m_ammoTypes[i].c_str()));
                if (m_pCurrentAmmo)
                {
                    m_ammoType = i;
                    break;
                }
            }
        }
    }

    // нет патронов для перезарядки
    if (!m_pCurrentAmmo && !unlimited_ammo())
        return;

    // разрядить магазин, если загружаем патронами другого типа
    if (!m_bLockType && !m_magazine.empty() &&
        (!m_pCurrentAmmo || xr_strcmp(m_pCurrentAmmo->cNameSect(), *m_magazine.back().m_ammoSect)))
        UnloadMagazine();

    VERIFY((u32)iAmmoElapsed == m_magazine.size());

    if (m_DefaultCartridge.m_LocalAmmoType != m_ammoType)
        m_DefaultCartridge.Load(m_ammoTypes[m_ammoType].c_str(), m_ammoType);
    CCartridge l_cartridge = m_DefaultCartridge;
    while (iAmmoElapsed < iMagazineSize)
    {
        if (!unlimited_ammo())
        {
            if (!m_pCurrentAmmo->Get(l_cartridge))
                break;
        }
        ++iAmmoElapsed;
        l_cartridge.m_LocalAmmoType = m_ammoType;
        m_magazine.push_back(l_cartridge);
    }

    VERIFY((u32)iAmmoElapsed == m_magazine.size());

    // выкинуть коробку патронов, если она пустая
    if (m_pCurrentAmmo && !m_pCurrentAmmo->m_boxCurr && OnServer())
        m_pCurrentAmmo->SetDropManual(true);

    if (iMagazineSize > iAmmoElapsed)
    {
        m_bLockType = true;
        ReloadBattery();
        m_bLockType = false;
    }

    VERIFY((u32)iAmmoElapsed == m_magazine.size());
}

void CWeaponGauss::render_item_3d_ui()
{
    R_ASSERT(HudItemData());
    inherited::render_item_3d_ui();
    if (m_bUseSwitchMode)
        ui().Draw();
    GEnv.UIRender->CacheSetCullMode(IUIRender::cmCCW);
}

bool CWeaponGauss::render_item_3d_ui_query()
{
    return m_bCompOpen && H_Parent() && H_Parent() == Level().CurrentViewEntity();
}
