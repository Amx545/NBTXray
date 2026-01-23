#include "StdAfx.h"
#include "UIActorClassSelector.h"
#include "UIXmlInit.h"
#include "UIHelper.h"
#include "xrUICore/Buttons/UI3tButton.h"
#include "xrUICore/ui_base.h"
#include "xrUICore/Static/UIStatic.h"
#include "xrUICore/Buttons/UIButton.h"
#include "xrUICore/EditBox/UIEditBox.h"
#include "xrUICore/Windows/UIFrameWindow.h"
#include "xrUICore/ComboBox/UIComboBox.h"
#include "ActorCondition.h"
#include "UICellItemFactory.h"
#include "UIDragDropCustomList.h"
#include "xrServer_space.h"

#define CLASS_SELECTOR_XML "ui_class_selector.xml"
#define CLASS_SELECTOR_CONFIG "creatures\\class_selector.ltx"

CUIActorClassSelector::CUIActorClassSelector() : CUIDialogWnd(CUIActorClassSelector::GetDebugType())
{
    ELastSelectedID = 1;
}

CUIActorClassSelector::~CUIActorClassSelector() 
{
    HideDialog();
    m_actor_class_info.clear();
    m_iItemListID.clear();
    if (m_pActorClassList)
        m_pActorClassList->Clear();
}

void CUIActorClassSelector::Init()
{
    m_actor_class_info.clear();
    m_iItemListID.clear();
    CUIXml xml;
    xml.Load(CONFIG_PATH, UI_PATH, CLASS_SELECTOR_XML);
    CUIXmlInit::InitWindow(xml, "main_wnd", 0, this);
    m_background = UIHelper::CreateStatic(xml, "background", this);
    m_downbackground = UIHelper::CreateFrameWindow(xml, "down_background", this);
    m_pClassOverlay = UIHelper::CreateFrameWindow(xml, "class_overlay", this);
    m_pStatsOverlay = UIHelper::CreateFrameWindow(xml, "stats_overlay", this);
    m_pDescrOverlay = UIHelper::CreateFrameWindow(xml, "descr_overlay", this);
    m_pPersonOverlay = UIHelper::CreateFrameWindow(xml, "person_overlay", this);
    m_pSuppliesOverlay = UIHelper::CreateFrameWindow(xml, "supplies_overlay", this);
    m_pActorLvl = UIHelper::CreateStatic(xml, "actor_lvl", this);
    m_pActorStr = UIHelper::CreateStatic(xml, "actor_str", this);
    m_pActorDex = UIHelper::CreateStatic(xml, "actor_dex", this);
    m_pActorInt = UIHelper::CreateStatic(xml, "actor_int", this);
    m_pActorVit = UIHelper::CreateStatic(xml, "actor_vit", this);
    m_pActorClassDescr = UIHelper::CreateScrollView(xml, "descr_list", this);
    m_pStaticPerson = UIHelper::CreateStatic(xml, "person_overlay:actor_icon", this);
    m_pClassDescr = xr_new<CUIStatic>("Description");
    m_pClassDescr->SetTextColor(m_pActorLvl->GetTextColor());
    m_pClassDescr->SetFont(GEnv.UI->Font().pFontLetterica18Russian);
    m_pClassDescr->SetWidth(m_pActorClassDescr->GetDesiredChildWidth());
    m_pClassDescr->SetTextComplexMode(true);
    m_pActorClassDescr->AddWindow(m_pClassDescr, true);
    btn_apply = UIHelper::Create3tButton(xml, "background:button_apply", this);
    Register(btn_apply);
    m_pActorClassList = UIHelper::CreateListBox(xml, "class_overlay:class_list", this);
    Register(m_pActorClassList);
    string256 path;
    strcpy(path, FS.get_path("$game_config$")->m_Path);
    strcat(path, CLASS_SELECTOR_CONFIG);
    m_ini_file = xr_new<CInifile>(path);
    pcstr N, V;
    for (u8 i = 0; m_ini_file->r_line("actor_class_list", i, &N, &V); ++i)
    {
        SActorClassInfo SInf;
        SInf.iActorClassDescription = "";
        SInf.iActorClassDexterity = 3;
        SInf.iActorClassIntelligence = 3;
        SInf.iActorClassVitality = 3;
        SInf.iActorClassStrength = 3;
        SInf.iActorClassLevel = 1;
        SInf.selected_type = i;
        pcstr class_name = "";
        if (m_ini_file->line_exist(N,"descr"))
            SInf.iActorClassDescription = m_ini_file->r_string(N, "descr");
        if (m_ini_file->line_exist(N, "vitality"))
            SInf.iActorClassVitality = m_ini_file->r_u8(N, "vitality");
        if (m_ini_file->line_exist(N, "skill"))
            SInf.iActorClassDexterity = m_ini_file->r_u8(N, "skill");
        if (m_ini_file->line_exist(N, "power"))
            SInf.iActorClassStrength = m_ini_file->r_u8(N, "power");
        if (m_ini_file->line_exist(N, "mind"))
            SInf.iActorClassIntelligence = m_ini_file->r_u8(N, "mind");
        if (m_ini_file->line_exist(N, "items"))
            SInf.cActorClassItems = m_ini_file->r_string_wb(N, "items");
        if (m_ini_file->line_exist(N, "icon"))
            SInf.cActorIcon = m_ini_file->r_string(N, "icon");
        if (m_ini_file->line_exist(N, "name"))
            class_name = m_ini_file->r_string(N, "name");

        CUIListBoxItem* l_item = m_pActorClassList->AddTextItem(class_name);
        l_item->SetWndSize({260, 42});
        l_item->GetTextItem()->SetWndPos({
            l_item->GetTextItem()->GetWndPos().x, l_item->GetTextItem()->GetWndPos().y+10});
        l_item->GetTextItem()->SetFont(GEnv.UI->Font().pFontGraffiti22Russian);
        l_item->GetTextItem()->SetColor(color_rgba(170, 170, 170, 255));
        l_item->GetTextItem()->SetEllipsis(true);
        m_actor_class_info.push_back(SInf);
    }

    if (xml.NavigateToNode("supplies_overlay:list", 0))
    {
        m_pList = xr_new<CUIDragDropCustomList>();
        if (!CUIXmlInit::InitDragDropListEx(xml, "supplies_overlay:list", 0, m_pList))
        {
            xr_delete(m_pList);
        }
        if (m_pList)
        {
            AttachChild(m_pList);
            m_pList->SetAutoDelete(true);
            m_pList->dx_pos = m_pSuppliesOverlay->GetWndRect().left;
        }
    }

    m_pActorClassList->SetSelectedIDX(0);
    OnActorClassSelected();
    m_pActorClassList->ScrollBar()->SetVisible(!m_pActorClassList->ScrollBar()->GetVisible());
}

void CUIActorClassSelector::Draw() { inherited::Draw(); }

void CUIActorClassSelector::Show(bool status) { inherited::Show(status); }

void CUIActorClassSelector::ShowDialog(bool bDoHideIndicators) 
{ 
    inherited::ShowDialog(bDoHideIndicators);
}

void CUIActorClassSelector::HideDialog() { inherited::HideDialog(); }

void CUIActorClassSelector::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
    if (pWnd == m_pActorClassList && msg == LIST_ITEM_CLICKED)
    {
        OnActorClassSelected();
    }
    else if (pWnd == btn_apply && msg == BUTTON_CLICKED)
    {
        OnActorClassApply();
    }
    inherited::SendMessage(pWnd, msg, pData);
}

void CUIActorClassSelector::Update() 
{ 
    if (!m_iItemListID.empty())
    {
        for (auto &i : m_iItemListID)
        {
            if (i.bStatus)
                continue;
            PIItem inv_item = Actor()->inventory().GetItemFromInventory(i.sName.c_str());
            Msg("STATUS_CLASS_SELECTOR %s", i.sName.c_str());
            if (!inv_item) continue;
            CUICellItem* itm = create_cell_item(inv_item);
            m_pList->SetItem(itm);
            i.bStatus = true;
        }
    }
    inherited::Update();
}


void CUIActorClassSelector::OnActorClassSelected()
{
    if (m_pActorClassList->GetSelectedIDX() != m_actor_class_info[m_pActorClassList->GetSelectedIDX()].selected_type)
        return;
    if (ELastSelectedID == m_pActorClassList->GetSelectedIDX())
        return;
    SActorClassInfo clInfo;
    string32 buffer;
    string32 numbuffer;
    // Release
    m_pList->ClearAll(true);
    m_iItemListID.clear();
    clInfo = m_actor_class_info[ELastSelectedID];
    u8 num_item_sections = _GetItemCount(*clInfo.cActorClassItems);
    for (u8 i = 0; i < num_item_sections; i++)
    {
        pstr release_item = _GetItem(*clInfo.cActorClassItems, i, buffer);
        if (!Actor()->inventory().GetItemFromInventory(release_item))
            continue;
        if (i + 1 < num_item_sections)
        {
            u16 count = atoi(_GetItem(*clInfo.cActorClassItems, i + 1, numbuffer));
            if (count && count>1)
            {
                LPCSTR itm_name = Actor()->inventory().GetItemFromInventory(release_item)->NameItem();
                TIItemContainer::iterator It = Actor()->inventory().m_all.begin();
                TIItemContainer::iterator E = Actor()->inventory().m_all.end();
                for (; It != E; ++It)
                {
                    if ((*It)->NameItem() == itm_name)
                    {
                        --count;
                        NET_Packet packet;
                        packet.w_begin(M_EVENT);
                        packet.w_u32(Level().timeServer());
                        packet.w_u16(GE_DESTROY);
                        packet.w_u16((*It)->object_id());
                        Level().Send(packet, net_flags(TRUE, TRUE));
                        if (count == 0) break;
                    }
                }
                i++;
                continue;
            }
        }
        NET_Packet packet;
        packet.w_begin(M_EVENT);
        packet.w_u32(Level().timeServer());
        packet.w_u16(GE_DESTROY);
        packet.w_u16(Actor()->inventory().GetItemFromInventory(release_item)->object_id());
        Level().Send(packet, net_flags(TRUE, TRUE));
    }
    //Create
    clInfo = m_actor_class_info[m_pActorClassList->GetSelectedIDX()];
    num_item_sections = _GetItemCount(*clInfo.cActorClassItems);
    CSE_Abstract* se_item = nullptr;
    for (u8 i = 0; i < num_item_sections; i++)
    {
        pcstr spawned_item = _GetItem(*clInfo.cActorClassItems, i, buffer);
        if (i + 1 < num_item_sections)
        {
            u16 count = atoi(_GetItem(*clInfo.cActorClassItems, i + 1, numbuffer));
            if (count)
            {
                for (u8 j = 0; j < count; j++)
                {
                    Level().spawn_item(
                        spawned_item, 
                        Actor()->Position(), 
                        Actor()->ai_location().level_vertex_id(), 
                        Actor()->ID());
                    SSpawnedItemStatus Sis;
                    Sis.sName = spawned_item;
                    Sis.bStatus = false;
                    m_iItemListID.push_back(Sis);
                }
                i++;
                continue;
            }
        }
        Level().spawn_item(spawned_item, Actor()->Position(), Actor()->ai_location().level_vertex_id(), Actor()->ID());
        SSpawnedItemStatus Sis;
        Sis.sName = spawned_item;
        Sis.bStatus = false;
        m_iItemListID.push_back(Sis);
    }

    u8 iLevelCl = clInfo.iActorClassVitality;
    sprintf(buffer, "%d", clInfo.iActorClassVitality);
    m_pActorVit->SetText(buffer);
    iLevelCl += clInfo.iActorClassIntelligence;
    sprintf(buffer, "%d", clInfo.iActorClassIntelligence);
    m_pActorInt->SetText(buffer);
    iLevelCl += clInfo.iActorClassDexterity;
    sprintf(buffer, "%d", clInfo.iActorClassDexterity);
    m_pActorDex->SetText(buffer);
    iLevelCl += clInfo.iActorClassStrength;
    sprintf(buffer, "%d", clInfo.iActorClassStrength);
    m_pActorStr->SetText(buffer);
    sprintf(buffer, "%d", iLevelCl - 11);
    m_pActorLvl->SetText(buffer);

    m_pClassDescr->SetText(StringTable().translate(clInfo.iActorClassDescription).c_str());
    m_pClassDescr->AdjustHeightToText();
    m_pStaticPerson->InitTexture(clInfo.cActorIcon.c_str(), false);
    ELastSelectedID = m_actor_class_info[m_pActorClassList->GetSelectedIDX()].selected_type;
}

void CUIActorClassSelector::OnActorClassApply() 
{
    SActorClassInfo SInfo = m_actor_class_info[m_pActorClassList->GetSelectedIDX()];
    if (m_pActorClassList->GetSelectedIDX() != SInfo.selected_type)
        return;
    if (Actor())
    {
        Actor()->conditions().ChangeStatsVitality(SInfo.iActorClassVitality, TRUE, TRUE);
        Actor()->conditions().ChangeStatsDexterity(SInfo.iActorClassDexterity, TRUE, TRUE);
        Actor()->conditions().ChangeStatsStrength(SInfo.iActorClassStrength, TRUE, TRUE);
        Actor()->conditions().ChangeStatsIntelligence(SInfo.iActorClassIntelligence, TRUE, TRUE);
        int lvl_cl = SInfo.iActorClassIntelligence + SInfo.iActorClassStrength + SInfo.iActorClassDexterity +
            SInfo.iActorClassVitality - 11;
        clamp(lvl_cl, 1, 255);
        Actor()->conditions().SetActorLevel(lvl_cl);
    }
    HideDialog();
}
