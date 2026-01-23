#pragma once

#include "inventory_item_object.h"

class CActorGlove : public CInventoryItemObject
{
    friend void CActorGlove_Export(lua_State* luaState);
    using inherited = CInventoryItemObject;

public:
    CActorGlove();

    virtual void Load(LPCSTR section);

    virtual void Hit(float P, ALife::EHitType hit_type);

    virtual void OnMoveToSlot(const SInvItemPlace& previous_place);
    virtual void OnMoveToRuck(const SInvItemPlace& previous_place);

    virtual bool net_Spawn(CSE_Abstract* DC);
    virtual void net_Export(NET_Packet& P);
    virtual void net_Import(NET_Packet& P);

    float GetDefHitTypeProtection(ALife::EHitType hit_type);
    float HitThroughArmor(float hit_power, s16 element, float ap, bool& add_wound, ALife::EHitType hit_type);

    float m_fPowerLoss;
    float m_fHealthRestoreSpeed;
    float m_fRadiationRestoreSpeed;
    float m_fSatietyRestoreSpeed;
    float m_fPowerRestoreSpeed;
    float m_fBleedingRestoreSpeed;
    float m_fPsyHealthRestoreSpeed;
    float m_fReloadSpeed;
    float m_fHandling;
    float m_fRecoil;

protected:
    HitImmunity::HitTypeSVec m_HitTypeProtection;

protected:
    virtual bool install_upgrade_impl(LPCSTR section, bool test);
};
