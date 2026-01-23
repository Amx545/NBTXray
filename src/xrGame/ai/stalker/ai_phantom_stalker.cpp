#include "StdAfx.h"
#include "ai_phantom_stalker.h"
#include "ActorEffector.h"
#include "alife_simulator.h"

CAI_PsyStalker::CAI_PsyStalker()
{
    m_max_phantoms_count = 0;
    m_phantoms_die_time = NULL;
    m_boss_mode = FALSE;
    m_boss_mode_multiplier = 1;
}

CAI_PsyStalker::~CAI_PsyStalker() { xr_free(m_phantoms_die_time); }

void CAI_PsyStalker::Load(LPCSTR section)
{
    inherited::Load(section);
    m_section = section;
    m_max_phantoms_count = pSettings->read_if_exists<u8>(section, "Phantoms_Count", 1);
    if (pSettings->line_exist(section, "Boss_Mode"))
    {
        m_boss_mode = pSettings->r_bool(section, "Boss_Mode") ? true : false;
    }
    xr_free(m_phantoms_die_time);
    m_phantoms_die_time = xr_alloc<TTime>(m_max_phantoms_count * 3);

    for (int i = 0; i < m_max_phantoms_count * 3; ++i)
        m_phantoms_die_time[i] = s_phantom_immediate_respawn_flag;

    m_particles_appear = pSettings->r_string(*cNameSect(), "particles_appear");
    m_particles_disappear = pSettings->r_string(*cNameSect(), "particles_disappear");
    pSettings->read_if_exists(m_time_phantom_respawn, section, "Time_Phantom_Respawn", "Time_Phantom_Appear", true);
}

void CAI_PsyStalker::reinit() { inherited::reinit(); }

void CAI_PsyStalker::reload(LPCSTR section) { inherited::reload(section); }

bool CAI_PsyStalker::net_Spawn(CSE_Abstract* dc)
{
    if (!inherited::net_Spawn(dc))
        return FALSE;
    if (m_boss_mode)
    {
        float health = pSettings->read_if_exists<float>(m_section, "Boss_Health_Base", 1.f);
        SetMaxHealth(health + GetStalkerLevel() * 0.1f);
    }
    m_se_stalker = smart_cast<CSE_ALifeHumanStalker*>(dc);
    return TRUE;
}

void CAI_PsyStalker::net_Destroy()
{
    delete_all_phantoms();
    inherited::net_Destroy();
}

void CAI_PsyStalker::Die(IGameObject* who)
{
    die_effect();
    inherited::Die(who);
    delete_all_phantoms();
}

void CAI_PsyStalker::Hit(SHit* pHDS)
{
    CAI_PsyStalkerPhantom* hit_owner = smart_cast<CAI_PsyStalkerPhantom*>(pHDS->who);
    if (hit_owner)
    {
        for (auto& i : m_storage)
        {
            if (pHDS->whoID == i->ID())
                return;
        }
    }
    if (::Random.randF(1.f) < 0.2f && m_boss_mode && g_Alive())
    {
        u32 node;
        if (get_node(ai_location().level_vertex_id(), 4, 8, 5, node))
        {
            die_effect();
            Position().set(ai().level_graph().vertex_position(node));
            CParticlesPlayer::StartParticles(m_particles_appear, Fvector().set(0.0f, 0.1f, 0.0f), ID());
        }
    }
    inherited::Hit(pHDS);
}

void CAI_PsyStalker::Think()
{
    inherited::Think();
    if (!g_Alive())
        return;
    m_boss_mode_multiplier = 1;
    if (m_boss_mode)
    {
        float temp = floor((4.4f - ((GetfHealth() / GetMaxHealth()) / .25f)) * 100.f)/100.f;
        clamp(temp, 0.f, 3.f);
        m_boss_mode_multiplier = (u8)temp;
    }

    // check spawn / destroy phantoms
    if (memory().enemy().selected() && get_phantoms_count() < get_max_phantoms_count())
    {
        for (int i = 0; i < get_max_phantoms_count(); ++i)
        {
            if (m_phantoms_die_time[i] != s_phantom_alive_flag &&
                time() > m_phantoms_die_time[i] + m_time_phantom_respawn)
            {
                if (spawn_phantom())
                    m_phantoms_die_time[i] = s_phantom_alive_flag;
            }
        }
    }
    else
    {
        if (!memory().enemy().selected() && !m_storage.empty())
        {
            delete_all_phantoms();
        }
    }
}

u8 CAI_PsyStalker::get_phantoms_count() { return u8(m_storage.size()); }

u8 CAI_PsyStalker::get_max_phantoms_count() { return m_max_phantoms_count * m_boss_mode_multiplier; }

void CAI_PsyStalker::die_effect()
{
    Fvector center;
    Center(center);

    CParticlesObject* ps = CParticlesObject::Create(m_particles_disappear, true);
    Fmatrix matrix;
    matrix.identity();
    matrix.k.set(Fvector().set(0.f, 1.f, 0.f));
    Fvector::generate_orthonormal_basis_normalized(matrix.k, matrix.j, matrix.i);
    matrix.translate_over(center);
    ps->SetXFORM(matrix);
    ps->Play(false);
}

bool CAI_PsyStalker::spawn_phantom()
{
    u32 node;
    if (::Random.randF(1.f) < 0.25f && m_boss_mode)
    {
        if (!get_node(memory().enemy().selected()->ai_location().level_vertex_id(), 2, 5, 5, node))
            if (!get_node(ai_location().level_vertex_id(), 4, 8, 5, node))
                return false;
    }
    else
    {
        if (!get_node(ai_location().level_vertex_id(), 4, 8, 5, node))
            return false;
    }
    
    pcstr phantomSection = READ_IF_EXISTS(pSettings, r_string, m_section, "phantom_section", "stalker_phantom");
    CSE_Abstract* phantom =
        Level().spawn_item(phantomSection, ai().level_graph().vertex_position(node), node, 0xffff, true);

    CSE_ALifeHumanStalker* pSE_Stalker = smart_cast<CSE_ALifeHumanStalker*>(phantom);
    VERIFY(pSE_Stalker);
    
    pSE_Stalker->m_spec_object_id = ID();
    pSE_Stalker->set_character_profile(pSettings->r_string(m_section, "character_profile"));
    pSE_Stalker->set_specific_character(pSettings->r_string(m_section, "character_profile"));
    NET_Packet P;
    pSE_Stalker->Spawn_Write(P, TRUE);
    Level().Send(P, net_flags(TRUE));
    F_entity_Destroy(pSE_Stalker);
    return true;
}

void CAI_PsyStalker::register_phantom(CAI_PsyStalkerPhantom* phantom) { m_storage.push_back(phantom); }

void CAI_PsyStalker::unregister_phantom(CAI_PsyStalkerPhantom* phantom)
{
    xr_vector<CAI_PsyStalkerPhantom*>::iterator it = std::find(m_storage.begin(), m_storage.end(), phantom);

    for (int i = 0; i < get_max_phantoms_count(); ++i)
    {
        if (m_phantoms_die_time[i] == s_phantom_alive_flag)
        {
            m_phantoms_die_time[i] = time();
            break;
        }
    }

    VERIFY(it != m_storage.end());
    m_storage.erase(it);
}

void CAI_PsyStalker::delete_all_phantoms()
{
    for (xr_vector<CAI_PsyStalkerPhantom*>::iterator it = m_storage.begin(); it != m_storage.end(); ++it)
        (*it)->destroy_from_parent();

    m_storage.clear();
}
bool CAI_PsyStalker::get_node(u32 src_node, float min_radius, float max_radius, u32 attempts, u32& dest_node)
{
    Fvector vertex_position = ai().level_graph().vertex_position(src_node);

    for (u32 i = 0; i < attempts; i++)
    {
        Fvector dir;
        dir.random_dir();
        dir.normalize();

        Fvector new_pos;
        new_pos.mad(vertex_position, dir, Random.randF(min_radius, max_radius));

        movement().restrictions().add_border(vertex_position, new_pos);
        dest_node = ai().level_graph().check_position_in_direction(src_node, vertex_position, new_pos);
        movement().restrictions().remove_border();
        if (dest_node != u32(-1) && movement().restrictions().accessible(dest_node))
            return true;
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////
// Phantom Psy Stalker
//////////////////////////////////////////////////////////////////////////
#include "ai_stalker_space.h"
CAI_PsyStalkerPhantom::CAI_PsyStalkerPhantom() {}

CAI_PsyStalkerPhantom::~CAI_PsyStalkerPhantom() {}

bool CAI_PsyStalkerPhantom::net_Spawn(CSE_Abstract* dc)
{
    if (!inherited::net_Spawn(dc))
        return FALSE;
    se_stalker = smart_cast<CSE_ALifeHumanStalker*>(dc);
    m_parent_id = se_stalker->m_spec_object_id;
    CSpecificCharacter selected_char;
    selected_char.Load(se_stalker->m_SpecificCharacter);
    start_spawn(selected_char.SupplySpawn());
    m_iHitsToKill = 0;
    m_parent = 0;
    VERIFY(m_parent_id != 0xffff);

    try_to_register_to_parent();

    setVisible(FALSE);
    setEnabled(FALSE);
    m_particles_appear = pSettings->r_string(*cNameSect(), "particles_appear");
    m_particles_disappear = pSettings->r_string(*cNameSect(), "particles_disappear");
    m_iMaxHits = pSettings->r_u8(*cNameSect(), "MaxHits");

    m_time_spawned = time();

    return (TRUE);
}

void CAI_PsyStalkerPhantom::net_Destroy()
{
    Fvector center;
    Center(center);

    CParticlesObject* ps = CParticlesObject::Create(m_particles_disappear, true);
    Fmatrix matrix;
    matrix.identity();
    matrix.k.set(Fvector().set(0.f, 1.f, 0.f));
    Fvector::generate_orthonormal_basis_normalized(matrix.k, matrix.j, matrix.i);
    matrix.translate_over(center);
    ps->SetXFORM(matrix);
    ps->Play(false);

    if (m_parent && !is_wait_to_destroy_object())
    {
        m_parent->unregister_phantom(this);
        m_parent = 0;
        m_parent_id = 0xffff;
    }

    inherited::net_Destroy();
}

void CAI_PsyStalkerPhantom::Hit(SHit* pHDS)
{
    if (is_wait_to_destroy_object())
        return;
    if (pHDS->whoID == m_parent_id)
        return;
    m_iHitsToKill++;
    const CEntityAlive* entity_alive = smart_cast<const CEntityAlive*>(pHDS->initiator());
    if (entity_alive && !wounded())
    {
        if (is_relation_enemy(entity_alive))
            sound().play(StalkerSpace::eStalkerSoundInjuring);
    }
    if (m_iMaxHits > m_iHitsToKill)
        return;
    if (pHDS->who != 0)
        destroy_me();
}

void CAI_PsyStalkerPhantom::Die(IGameObject* who)
{
    inherited::Die(who);
    destroy_me();
}

void CAI_PsyStalkerPhantom::start_spawn(LPCSTR ini_str)
{
    if (!ini_str)
        return;

    if (!xr_strlen(ini_str))
        return;

#pragma warning(push)
#pragma warning(disable : 4238)
    IReader reader((void*)ini_str, xr_strlen(ini_str));
    CInifile ini(&reader, FS.get_path("$game_config$")->m_Path);
#pragma warning(pop) 

    if (!ini.section_exist("spawn"))
        return;
    pcstr N, V;
    for (u32 k = 0, j; ini.r_line("spawn", k, &N, &V); k++)
    {
        VERIFY(xr_strlen(N));

        if (pSettings->section_exist(N)) // Alundaio: verify item section exists!^^
        {
            bool bScope = false;
            bool bSilencer = false;
            bool bLauncher = false;

            j = 1;

            if (V && xr_strlen(V))
            {
                string64 buf;
                j = atoi(_GetItem(V, 0, buf));
                if (!j)
                    j = 1;

                bScope = nullptr != strstr(V, "scope");
                bSilencer = nullptr != strstr(V, "silencer");
                bLauncher = nullptr != strstr(V, "launcher");
            }
            for (u32 i = 0; i < j; ++i)
            {
                CSE_Abstract* E = Level().spawn_item(N, Position(), ai_location().level_vertex_id(), ID(), true);
                // подсоединить аддоны к оружию, если включены соответствующие флажки
                CSE_ALifeItemWeapon* W = smart_cast<CSE_ALifeItemWeapon*>(E);
                CSE_ALifeItemAmmo* Ammo = smart_cast<CSE_ALifeItemAmmo*>(E);
                if (!W && !Ammo)
                    continue;
                if (W)
                {
                    if (W->m_scope_status == ALife::eAddonAttachable)
                        W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonScope, bScope);
                    if (W->m_silencer_status == ALife::eAddonAttachable)
                        W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonSilencer, bSilencer);
                    if (W->m_grenade_launcher_status == ALife::eAddonAttachable)
                        W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher, bLauncher);
                }
                NET_Packet P;
                E->Spawn_Write(P, TRUE);
                Level().Send(P, net_flags(TRUE));
                F_entity_Destroy(E);
            }
        }
    }
}

void CAI_PsyStalkerPhantom::Think()
{
    if (is_wait_to_destroy_object())
        return;
    inherited::Think();

    try_to_register_to_parent();

    if (m_parent && m_parent->Position().distance_to(Position()) > 30)
    {
        destroy_me();
        return;
    }

    if (!m_parent)
    {
        if (m_time_spawned + 10000 > time())
            destroy_me();
        return;
    }
    if (m_ph_state != eWaitToAppear)
        return;
    memory().enemy().set_enemy(m_parent->memory().enemy().selected());

    Fvector target;
    target.mad(Position(), Direction(), 10.f);
    // нода в прямой видимости?
    movement().restrictions().add_border(Position(), target);
    u32 node = ai().level_graph().check_position_in_direction(ai_location().level_vertex_id(), Position(), target);
    movement().restrictions().remove_border();

    m_ph_state = eAttack;
    setVisible(TRUE);
    setEnabled(TRUE);
    CParticlesPlayer::StartParticles(m_particles_appear, Fvector().set(0.0f, 0.1f, 0.0f), ID());

}

void CAI_PsyStalkerPhantom::destroy_from_parent()
{
    m_parent_id = 0xffff;

    NET_Packet P;
    u_EventGen(P, GE_DESTROY, ID());
    u_EventSend(P);
}

void CAI_PsyStalkerPhantom::destroy_me()
{
    VERIFY(!is_wait_to_destroy_object());

    if (m_parent)
    {
        m_parent->unregister_phantom(this);
        m_parent = 0;
        m_parent_id = 0xffff;
    }

    NET_Packet P;
    u_EventGen(P, GE_DESTROY, ID());
    u_EventSend(P);
}

void CAI_PsyStalkerPhantom::try_to_register_to_parent()
{ // parent not ready yet
    if (m_parent)
        return;

    IGameObject* obj = Level().Objects.net_Find(m_parent_id);
    if (obj)
    {
        CAI_PsyStalker* dog = smart_cast<CAI_PsyStalker*>(obj);
        VERIFY(dog);

        m_parent = dog;
        m_parent->register_phantom(this);

        movement().restrictions().add_restrictions(m_parent->movement().restrictions().out_restrictions(),
            m_parent->movement().restrictions().in_restrictions());

        m_ph_state = eWaitToAppear;
    }
}
