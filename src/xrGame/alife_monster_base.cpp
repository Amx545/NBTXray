////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_monster_base.cpp
//	Created 	: 07.02.2007
//  Modified 	: 07.02.2007
//	Author		: Dmitriy Iassenev
//	Description : ALife mnster base class
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "alife_simulator.h"
#include "xrServer.h"
#include "alife_monster_brain.h"

void CSE_ALifeMonsterBase::on_spawn()
{
    inherited1::on_spawn();

    if (!pSettings->line_exist(s_name, "Spawn_Inventory_Item_Section"))
        return;
    // LPCSTR item_section = pSettings->r_string(s_name, "Spawn_Inventory_Item_Section");
    // float spawn_probability = pSettings->r_float(s_name, "Spawn_Inventory_Item_Probability");
    string128 buffer;
    float temp_sp;
    pstr temp_is;
    shared_str item_section = pSettings->r_string_wb(s_name, "Spawn_Inventory_Item_Section");
    shared_str spawn_probability = pSettings->r_string_wb(s_name, "Spawn_Inventory_Item_Probability");
    u8 num_item_sections = _GetItemCount(*item_section);
    u8 num_spawn_probability = _GetItemCount(*item_section);
    for (u8 i = 0; i < num_item_sections; i++)
    //float probability = randF();
    {
        if (num_spawn_probability > i)
            temp_sp = (float)atof(_GetItem(*spawn_probability, i, buffer));
        else
            temp_sp = (float)atof(_GetItem(*spawn_probability, 0, buffer));
        temp_is = _GetItem(*item_section, i, buffer);
        if ((randF() >= temp_sp) && !fsimilar(temp_sp, 1.f))
            continue;
        alife().spawn_item(temp_is, o_Position, m_tNodeID, m_tGraphID, ID)->ID_Parent = ID;
    }
}

extern void add_online_impl(CSE_ALifeDynamicObject* object, const bool& update_registries);

void CSE_ALifeMonsterBase::add_online(const bool& update_registries)
{
    add_online_impl(this, update_registries);
    brain().on_switch_online();
}

extern void add_offline_impl(
    CSE_ALifeDynamicObject* object, const xr_vector<ALife::_OBJECT_ID>& saved_children, const bool& update_registries);

void CSE_ALifeMonsterBase::add_offline(
    const xr_vector<ALife::_OBJECT_ID>& saved_children, const bool& update_registries)
{
    add_offline_impl(this, saved_children, update_registries);
    brain().on_switch_offline();
}
