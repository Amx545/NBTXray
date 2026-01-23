#pragma once
#include "ai_stalker.h"
class CAI_PsyStalkerPhantom;
class CAI_PsyStalker : public CAI_Stalker
{
    typedef CAI_Stalker inherited;

    friend class CAI_PsyStalkerPhantom;

    CActor* m_enemy;
    bool m_boss_mode;
    u8 m_max_phantoms_count;
    u8 m_boss_mode_multiplier;
    u32 m_time_phantom_respawn;

    static u32 const s_phantom_immediate_respawn_flag = 0;
    static u32 const s_phantom_alive_flag = 1;

    TTime* m_phantoms_die_time;

public:
    CAI_PsyStalker();
    virtual ~CAI_PsyStalker();

    virtual void Load(LPCSTR section);
    virtual void reinit();
    virtual void reload(LPCSTR section);
    virtual bool net_Spawn(CSE_Abstract* dc);
    virtual void net_Destroy();
    virtual void Die(IGameObject* who);
    virtual void Hit(SHit* pHDS);

    virtual void Think();

    u8 get_phantoms_count();
    u8 get_max_phantoms_count();
    void die_effect();

private:
    bool spawn_phantom();
    void register_phantom(CAI_PsyStalkerPhantom* phantom);
    void unregister_phantom(CAI_PsyStalkerPhantom* phantom);
    void delete_all_phantoms();
    bool get_node(u32 src_node, float min_radius, float max_radius, u32 attempts, u32& dest_node);

private:
    shared_str m_section;
    LPCSTR m_particles_appear;
    LPCSTR m_particles_disappear;
    xr_vector<CAI_PsyStalkerPhantom*> m_storage;
    CSE_ALifeHumanStalker* m_se_stalker;
};

//////////////////////////////////////////////////////////////////////////
// Phantom Psy Stalker
//////////////////////////////////////////////////////////////////////////
class CAI_PsyStalkerPhantom : public CAI_Stalker
{
    typedef CAI_Stalker inherited;
    enum
    {
        eWaitToAppear,
        eAttack
    } m_ph_state;
    CAI_PsyStalker* m_parent;

    SAttackEffector m_appear_effector;

    LPCSTR m_particles_appear;
    LPCSTR m_particles_disappear;

    u16 m_parent_id;

    u32 m_time_spawned;

public:
    CAI_PsyStalkerPhantom();
    virtual ~CAI_PsyStalkerPhantom();
    virtual bool net_Spawn(CSE_Abstract* dc);
    virtual void net_Destroy();
    virtual void Hit(SHit* pHDS);
    virtual void Die(IGameObject* who);
    void start_spawn(LPCSTR ini_str);

    virtual void Think();

    void destroy_from_parent();

private:
    void destroy_me();
    void try_to_register_to_parent();
    bool is_wait_to_destroy_object() { return (m_parent_id == 0xffff); }

private:
    CSE_ALifeHumanStalker* se_stalker;
    u8 m_iHitsToKill{0};
    u8 m_iMaxHits{0};
};
