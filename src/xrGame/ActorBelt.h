#pragma once
#include "inventory_item_object.h"


class CActorBelt : public CInventoryItemObject
{
    using inherited = CInventoryItemObject;

public:
    CActorBelt();

    void Load(pcstr section) override;

    float m_additional_weight;
    float m_additional_weight2;
    float m_fHealthRestoreSpeed;
    float m_fRadiationRestoreSpeed;
    float m_fSatietyRestoreSpeed;
    float m_fPowerRestoreSpeed;
    float m_fBleedingRestoreSpeed;

    
protected:
    u32 m_artefact_count;
    bool install_upgrade_impl(pcstr section, bool test) override;

public:
    u32 get_artefact_count() const { return m_artefact_count; }
};
