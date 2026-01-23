////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_object.cpp
//	Created 	: 27.10.2005
//  Modified 	: 27.10.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife object class
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "xrServer_Objects_ALife.h"
#include "alife_simulator.h"
#include "xrServer_Objects_ALife_Items.h"
#include "xrCommon/xr_vector.h"

void CSE_ALifeObject::spawn_supplies() { spawn_supplies(*m_ini_string); }

void CSE_ALifeObject::spawn_supplies(LPCSTR ini_string)
{
    if (!ini_string)
        return;

    if (!xr_strlen(ini_string))
        return;

#pragma warning(push)
#pragma warning(disable : 4238)
    IReader reader((void*)ini_string, xr_strlen(ini_string));
    CInifile ini(&reader, FS.get_path("$game_config$")->m_Path);
#pragma warning(pop)

    // Alundaio: This will spawn a single random section listed in [spawn_loadout]
    // No need to spawn ammo, this will automatically spawn 1 box for weapon and if ammo_type is specified it will spawn that type
    // count is used only for ammo boxes (ie wpn_pm = 3) will spawn 3 boxes, not 3 wpn_pm
    // Usage: to create random weapon loadouts
    if (ini.section_exist("spawn_loadout"))
    {
        pcstr itmSection, V;
        xr_vector<u32> OnlyOne;

        pcstr lname = *ai().game_graph().header().level(ai().game_graph().vertex(m_tGraphID)->level_id()).name();

        for (u32 k = 0; ini.r_line("spawn_loadout", k, &itmSection, &V); k++)
        {
            // If level=<lname> then only spawn items if object on that level
            if (strstr(V, "level=") != nullptr)
            {
                if (strstr(V, lname) != nullptr)
                    OnlyOne.push_back(k);
            }
            else
            {
                OnlyOne.push_back(k);
            }
        }

        if (!OnlyOne.empty())
        {
            s32 sel = Random.randI(0, OnlyOne.size());
            if (ini.r_line("spawn_loadout", OnlyOne.at(sel), &itmSection, &V))
            {
                VERIFY(xr_strlen(itmSection));
                if (pSettings->section_exist(itmSection))
                {
                    u32 spawn_count = 1;
                    bool bScope = false;
                    bool bSilencer = false;
                    bool bLauncher = false;
                    float f_cond = 1.0f;
                    int i_ammo_type = 0, n = 0;

                    if (V && xr_strlen(V))
                    {
                        n = _GetItemCount(V);
                        if (n > 0)
                        {
                            string64 tmp;
                            spawn_count = atoi(_GetItem(V, 0, tmp)); //count
                        }

                        if (!spawn_count) spawn_count = 1;
                        if (nullptr != strstr(V, "cond="))
                            f_cond = static_cast<float>(atof(strstr(V, "cond=") + 5));
                        bScope = nullptr != strstr(V, "scope");
                        bSilencer = nullptr != strstr(V, "silencer");
                        bLauncher = nullptr != strstr(V, "launcher");
                        if (nullptr != strstr(V, "ammo_type="))
                            i_ammo_type = atoi(strstr(V, "ammo_type=") + 10);
                    }


                    CSE_Abstract* E = alife().spawn_item(itmSection, o_Position, m_tNodeID, m_tGraphID, ID);
                    CSE_ALifeItemWeapon* W = smart_cast<CSE_ALifeItemWeapon*>(E);
                    if (W)
                    {
                        if (W->m_scope_status == ALife::eAddonAttachable)
                            W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonScope, bScope);
                        if (W->m_silencer_status == ALife::eAddonAttachable)
                            W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonSilencer, bSilencer);
                        if (W->m_grenade_launcher_status == ALife::eAddonAttachable)
                            W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher, bLauncher);

                        //spawn count box(es) of the correct ammo for weapon
                        if (pSettings->line_exist(itmSection, "ammo_class"))
                        {
                            pcstr ammo_class = pSettings->r_string(itmSection, "ammo_class");
                            pcstr ammoSec = "";
                            for (int i = 0, n = _GetItemCount(ammo_class); i < n; ++i)
                            {
                                string128 tmp;
                                ammoSec = _GetItem(ammo_class, i, tmp);
                                if (i == i_ammo_type)
                                    break;
                            }
                            if (xr_strlen(ammoSec) && pSettings->section_exist(ammoSec))
                                for (u32 i = 1; i <= spawn_count; ++i)
                                    alife().spawn_item(ammoSec, o_Position, m_tNodeID, m_tGraphID, ID);
                        }
                    }
                    CSE_ALifeInventoryItem* IItem = smart_cast<CSE_ALifeInventoryItem*>(E);
                    if (IItem)
                        IItem->m_fCondition = f_cond;
                }
            }
        }
    }
    //-Alundaio

    string32 buff;
    u8 it = 1;
    u8 no_spawnCount = 0;
    xr_sprintf(buff, "spawn_only_one_%d", it);
    while (ini.section_exist(buff))
    {
        u32 total = 0, p = 0, min = 1, max = 200;
        s32 total_x = 0;
        pcstr N, V;
        xr_unordered_map<u32, u32> spawn_map;
        for (u32 k = 0; ini.r_line(buff, k, &N, &V); k++)
        {
            VERIFY(xr_strlen(N));

            if (pSettings->section_exist(N))
            {
                if (V && xr_strlen(V) && (nullptr != strstr(V, "prob=")))
                    p = atoi(strstr(V, "prob=") + 5);
                else
                    p = 10;
                clamp(p, min, max);
                total += p;
                spawn_map.emplace(k, p);
            }
        }
        total_x = randI(total);
        for (auto& [item, prob] : spawn_map)
        {
            total_x -= prob;
            if (total_x <= 0)
            {
                if (ini.r_line(buff, item, &N, &V))
                {
                    VERIFY(xr_strlen(N));
                    if (pSettings->section_exist(N))
                    {
                        CSE_Abstract* E = alife().spawn_item(N, o_Position, m_tNodeID, m_tGraphID, ID);
                        CSE_ALifeItemWeapon* W = smart_cast<CSE_ALifeItemWeapon*>(E);
                        if (W)
                        {
                            bool bScope = false;
                            bool bSilencer = false;
                            bool bLauncher = false;
                            u8 bScopeSect = 0;
                            u8 iAmmoType = 0;
                            if (V && xr_strlen(V))
                            {
                                bScope = nullptr != strstr(V, "scope");
                                bSilencer = nullptr != strstr(V, "silencer");
                                bLauncher = nullptr != strstr(V, "launcher");
                                if (nullptr != strstr(V, "scope="))
                                    bScopeSect = atoi(strstr(V, "scope=") + 6);
                                if (nullptr != strstr(V, "ammo_type="))
                                    iAmmoType = atoi(strstr(V, "ammo_type=") + 10);
                            }
                            if (W->m_scope_status == ALife::eAddonAttachable)
                            {
                                W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonScope, bScope);
                                W->m_scope_section = bScopeSect;
                            }
                            if (W->m_silencer_status == ALife::eAddonAttachable)
                                W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonSilencer, bSilencer);
                            if (W->m_grenade_launcher_status == ALife::eAddonAttachable)
                                W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher, bLauncher);
                            if (pSettings->line_exist(N, "ammo_class"))
                            {
                                pcstr ammo_class = pSettings->r_string(N, "ammo_class");
                                string128 ammoSec;
                                if (_GetItemCount(ammo_class) > iAmmoType)
                                    _GetItem(ammo_class, iAmmoType, ammoSec);
                                else
                                    _GetItem(ammo_class, 0, ammoSec);
                                if (xr_strlen(ammoSec) && pSettings->section_exist(ammoSec))
                                    alife().spawn_item(ammoSec, o_Position, m_tNodeID, m_tGraphID, ID);
                            }
                        }
                    }
                    else
                        no_spawnCount++;
                }
                break;
            }
                
        }
        total = 0;
        if (it >= 2 && no_spawnCount != it)
            break;
        it++;
        xr_sprintf(buff, "spawn_only_one_%d", it);
    }

    if (ini.section_exist("spawn"))
    {
        pcstr N, V;
        float p;
        for (u32 k = 0, j; ini.r_line("spawn", k, &N, &V); k++)
        {
            VERIFY(xr_strlen(N));

            if (pSettings->section_exist(N)) //Alundaio: verify item section exists!
            {
                float f_cond = 1.0f;
                bool l_bRandom = false;
                bool bScope = false;
                bool bSilencer = false;
                bool bLauncher = false;
                float f_min_cond = 1.0f;
                float f_max_cond = 1.0f;
                u8 bScopeSect = 0;

                j = 1;
                p = 1.f;

                if (V && xr_strlen(V))
                {
                    string64 buf;
                    j = atoi(_GetItem(V, 0, buf));
                    if (!j)
                        j = 1;

                    bScope = nullptr != strstr(V, "scope");
                    bSilencer = nullptr != strstr(V, "silencer");
                    bLauncher = nullptr != strstr(V, "launcher");
                    if (nullptr != strstr(V, "scope="))
                        bScopeSect = atoi(strstr(V, "scope=") + 6);
                    // probability
                    if (nullptr != strstr(V, "prob="))
                        p = static_cast<float>(atof(strstr(V, "prob=") + 5));
                    if (fis_zero(p))
                        p = 1.0f;
                    if (nullptr != strstr(V, "cond="))
                        f_cond = static_cast<float>(atof(strstr(V, "cond=") + 5));
                    if (nullptr != strstr(V, "cond_min=") && nullptr != strstr(V, "cond_max="))
                    {
                        f_min_cond = static_cast<float>(atof(strstr(V, "cond_min=") + 9));
                        f_max_cond = static_cast<float>(atof(strstr(V, "cond_max=") + 9));
                        l_bRandom = true;
                    }
                }
                for (u32 i = 0; i < j; ++i)
                {
                    if (randF(1.f) < p)
                    {
                        CSE_Abstract* E = alife().spawn_item(N, o_Position, m_tNodeID, m_tGraphID, ID);
                        //подсоединить аддоны к оружию, если включены соответствующие флажки
                        CSE_ALifeItemWeapon* W = smart_cast<CSE_ALifeItemWeapon*>(E);
                        if (W)
                        {
                            if (W->m_scope_status == ALife::eAddonAttachable)
                            {
                                W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonScope, bScope);
                                W->m_scope_section = bScopeSect;
                            }
                            if (W->m_silencer_status == ALife::eAddonAttachable)
                                W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonSilencer, bSilencer);
                            if (W->m_grenade_launcher_status == ALife::eAddonAttachable)
                                W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher, bLauncher);
                        }
                        CSE_ALifeInventoryItem* IItem = smart_cast<CSE_ALifeInventoryItem*>(E);
                        if (IItem)
                        {
                            if (l_bRandom)
                                f_cond = floor(randF(f_min_cond, f_max_cond) * 100) / 100;
                            IItem->m_fCondition = f_cond;
                        }
                    }
                }
            }
        }
    }
}

bool CSE_ALifeObject::keep_saved_data_anyway() const /* noexcept */ { return false; }
