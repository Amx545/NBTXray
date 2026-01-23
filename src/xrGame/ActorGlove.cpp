#include "StdAfx.h"
#include "ActorGlove.h"
#include "Actor.h"
#include "player_hud.h"
#include "Inventory.h"

CActorGlove::CActorGlove()
{
    m_flags.set(FUsingCondition, TRUE);
    m_HitTypeProtection.resize(ALife::eHitTypeMax);
    for (u32 i = 0; i < ALife::eHitTypeMax; i++)
        m_HitTypeProtection[i] = 1.0f;
}

void CActorGlove::Load(LPCSTR section) { 
    inherited::Load(section); 
    m_HitTypeProtection[ALife::eHitTypeBurn] = pSettings->r_float(section, "burn_protection");
    m_HitTypeProtection[ALife::eHitTypeStrike] = pSettings->r_float(section, "strike_protection");
    m_HitTypeProtection[ALife::eHitTypeShock] = pSettings->r_float(section, "shock_protection");
    m_HitTypeProtection[ALife::eHitTypeWound] = pSettings->r_float(section, "wound_protection");
    m_HitTypeProtection[ALife::eHitTypeRadiation] = pSettings->r_float(section, "radiation_protection");
    m_HitTypeProtection[ALife::eHitTypeTelepatic] = pSettings->r_float(section, "telepatic_protection");
    m_HitTypeProtection[ALife::eHitTypeChemicalBurn] = pSettings->r_float(section, "chemical_burn_protection");
    m_HitTypeProtection[ALife::eHitTypeExplosion] = pSettings->r_float(section, "explosion_protection");
    m_HitTypeProtection[ALife::eHitTypeFireWound] = pSettings->read_if_exists<float>(section, "fire_wound_protection", 0.f);
    m_HitTypeProtection[ALife::eHitTypePhysicStrike] = pSettings->read_if_exists<float>(section, "physic_strike_protection", m_HitTypeProtection[ALife::eHitTypeStrike]);
    m_HitTypeProtection[ALife::eHitTypeLightBurn] = m_HitTypeProtection[ALife::eHitTypeBurn];
    m_fHealthRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "health_restore_speed", 0.0f);
    m_fRadiationRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "radiation_restore_speed", 0.0f);
    m_fSatietyRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "satiety_restore_speed", 0.0f);
    m_fPowerRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "power_restore_speed", 0.0f);
    m_fBleedingRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "bleeding_restore_speed", 0.0f);
    m_fPsyHealthRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "psy_health_restore_speed", 0.0f);
    m_fPowerLoss = READ_IF_EXISTS(pSettings, r_float, section, "power_loss", 1.0f);
    clamp(m_fPowerLoss, 0.0f, 1.0f);
    m_fReloadSpeed = READ_IF_EXISTS(pSettings, r_float, section, "reload_speed", 0.0f);
    m_fHandling = READ_IF_EXISTS(pSettings, r_float, section, "handling", 0.0f);
    m_fRecoil = READ_IF_EXISTS(pSettings, r_float, section, "recoil", 0.0f);
    m_flags.set(FUsingCondition, READ_IF_EXISTS(pSettings, r_bool, section, "use_condition", true));
}

void CActorGlove::Hit(float hit_power, ALife::EHitType hit_type)
{
    hit_power *= GetHitImmunity(hit_type);
    float cound = hit_power;
    float ampl = 0.f;
    for (int i = 0; i < RarityItem(); i++)
    {
        ampl += 0.1 + 0.02 * (i + 1);
    }
    cound /= (1.f + ampl);
    ChangeCondition(-cound);
}

void CActorGlove::OnMoveToSlot(const SInvItemPlace& previous_place)
{
    if (m_pInventory)
    {
        CActor* pActor = smart_cast<CActor*>(H_Parent());
        if (pActor)
        {
            if (pActor == Level().CurrentViewEntity())
            {
                if (pSettings->line_exist(cNameSect(), "player_hand_section"))
                    g_player_hud->load_hand(pSettings->r_string(cNameSect(), "player_hand_section"));
                else
                    g_player_hud->load_default_hand();
            }
        }
    }
}

void CActorGlove::OnMoveToRuck(const SInvItemPlace& previous_place)
{
    if (m_pInventory && previous_place.type == eItemPlaceSlot)
    {
        CActor* pActor = smart_cast<CActor*>(H_Parent());
        if (pActor)
        {
            if (pActor == Level().CurrentViewEntity())
            {
                g_player_hud->load_default_hand();
            }
        }
    }
}

bool CActorGlove::net_Spawn(CSE_Abstract* DC)
{
    BOOL res = inherited::net_Spawn(DC);
    return (res);
}

void CActorGlove::net_Export(NET_Packet& P)
{
    inherited::net_Export(P);
    P.w_float_q8(GetCondition(), 0.0f, 1.0f);
}

void CActorGlove::net_Import(NET_Packet& P)
{
    inherited::net_Import(P);
    float _cond;
    P.r_float_q8(_cond, 0.0f, 1.0f);
    SetCondition(_cond);
}

float CActorGlove::GetDefHitTypeProtection(ALife::EHitType hit_type)
{
    return m_HitTypeProtection[hit_type] * GetCondition();
}

float CActorGlove::HitThroughArmor(float hit_power, s16 element, float ap, bool& add_wound, ALife::EHitType hit_type)
{
    float protect = GetDefHitTypeProtection(hit_type);
    Hit(hit_power, hit_type);

    return protect;
}

bool CActorGlove::install_upgrade_impl(LPCSTR section, bool test)
{
    return false;
}
