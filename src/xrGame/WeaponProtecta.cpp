#include "StdAfx.h"
#include "WeaponProtecta.h"
#include "player_hud.h"
#include "Actor.h"
#include "ActorCondition.h"

CWeaponProtecta::CWeaponProtecta() {}

CWeaponProtecta::~CWeaponProtecta() {}

void CWeaponProtecta::Load(LPCSTR section)
{
    inherited::Load(section);
    if (pSettings->line_exist(section, "protecta_bullet_scheme"))
    {
        bProtectaBulletScheme = pSettings->r_bool(section, "protecta_bullet_scheme") ? true : false;
        sProtectaBulletName = pSettings->r_string(section, "protecta_bullet_name");
        iProtectaBulletDelay = pSettings->r_u16(section, "protecta_bullet_delay");
        iProtectaBulletCount = pSettings->r_u8(section, "protecta_bullet_count");
    }
    else
        bProtectaBulletScheme = false;
}

void CWeaponProtecta::shedule_Update(u32 dt)
{
    
    if (ParentIsActor() && bProtectaBulletScheme && HudItemData())
    {
        float speed = 1.f;
        u32 delay = iProtectaBulletDelay;
        CActor* pActor = smart_cast<CActor*>(this->H_Parent());
        if (pActor)
        {
            speed *= pActor->conditions().GetSpeedReloadPerk();
            delay = (u32)((float)delay / speed);
        }
        u32 motion_delay = m_dwMotionStartTm + delay;
        if ((m_dwMotionCurrTm > motion_delay) && bAmmoSkip && iAmmoCurrent == iAmmoElapsed)
        {
            iAmmoCurrent = iAmmoElapsed + 1;
            ProtectaBoneVisibility(iAmmoCurrent, true);
        }
        if ((iAmmoCurrent != iAmmoElapsed) && !bAmmoSkip)
        {
            if (GetState() == eFire)
                ProtectaBoneVisibility(iAmmoCurrent, false);
            else
                ProtectaBoneUpdate();
            iAmmoCurrent = iAmmoElapsed;
        }
    }
    inherited::shedule_Update(dt);
}

void CWeaponProtecta::PlayAnimAddOneCartridgeWeapon()
{
    VERIFY(GetState() == eReload);
    bAmmoSkip = true;
    inherited::PlayAnimAddOneCartridgeWeapon();
}

void CWeaponProtecta::PlayAnimHide()
{
    VERIFY(GetState() == eHiding);
    bAmmoSkip = false;
    inherited::PlayAnimHide();
}

void CWeaponProtecta::PlayAnimShow()
{
    inherited::PlayAnimShow();
    ProtectaBoneUpdate();
}

void CWeaponProtecta::AddCartridgeUpdate()
{
    inherited::AddCartridgeUpdate();
    bAmmoSkip = false;
}

void CWeaponProtecta::ProtectaBoneUpdate()
{
    if (!bProtectaBulletScheme)
        return;
    if (!HudItemData())
        return;
    for (u8 i = 1; i <= iProtectaBulletCount; i++)
    {
        ProtectaBoneVisibility(i, i <= iAmmoElapsed);
    }
}

void CWeaponProtecta::ProtectaBoneVisibility(u8 index, bool add)
{
    if (!bProtectaBulletScheme)
        return;
    if (!HudItemData())
        return;
    string64 buf = "";
    string64 upbuf = "";
    xr_strcpy(upbuf, sProtectaBulletName);
    xr_sprintf(buf, "%d", index);
    xr_strcat(upbuf, buf);
    if (add)
        HudItemData()->set_bone_visible(upbuf, TRUE, TRUE);
    else
        HudItemData()->set_bone_visible(upbuf, FALSE, TRUE);
}
