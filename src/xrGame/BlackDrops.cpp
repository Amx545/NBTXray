///////////////////////////////////////////////////////////////
// BlackDrops.cpp
// BlackDrops - черные капли
///////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "BlackDrops.h"
#include "Actor.h"
#include "ActorCondition.h"
#include "xrPhysics/PhysicsShell.h"

CBlackDrops::CBlackDrops(void) : m_iAbilityPoint(1) {}
CBlackDrops::~CBlackDrops(void) {}
void CBlackDrops::Load(LPCSTR section)
{
    m_iAbilityPoint = pSettings->read_if_exists<u8>(section, "ability_point", 1);
    inherited::Load(section);
}
void CBlackDrops::UpdateCLChild()
{
    inherited::UpdateCLChild();

    if (H_Parent())
        XFORM().set(H_Parent()->XFORM());
}

void CBlackDrops::UseShard(CActor* actor) 
{
    u32 points = actor->conditions().GetAbilityPoint();
    actor->conditions().SetAbilityPoint(points + m_iAbilityPoint);
    DestroyObject();
}
