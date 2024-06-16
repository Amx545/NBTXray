#include "StdAfx.h"
#include "ActorBelt.h"
#include "Actor.h"
#include "Inventory.h"

CActorBelt::CActorBelt() { m_flags.set(FUsingCondition, false); }

void CActorBelt::Load(pcstr section) 
{
    inherited::Load(section);
    m_additional_weight = READ_IF_EXISTS(pSettings, r_float, section, "additional_inventory_weight", 0.0f);
    m_additional_weight2 = READ_IF_EXISTS(pSettings, r_float, section, "additional_inventory_weight2", 0.0f);
    m_fHealthRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "health_restore_speed", 0.0f);
    m_fRadiationRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "radiation_restore_speed", 0.0f);
    m_fSatietyRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "satiety_restore_speed", 0.0f);
    m_fPowerRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "power_restore_speed", 0.0f);
    m_fBleedingRestoreSpeed = READ_IF_EXISTS(pSettings, r_float, section, "bleeding_restore_speed", 0.0f);
    m_artefact_count = READ_IF_EXISTS(pSettings, r_u32, section, "artefact_count", 0);
    clamp(m_artefact_count, (u32)0, (u32)5);
    m_flags.set(FUsingCondition, pSettings->read_if_exists<bool>(section, "use_condition", false));
}

bool CActorBelt::install_upgrade_impl(pcstr section, bool test) { return false; }
