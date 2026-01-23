#include "StdAfx.h"
#include "UIDragDropCustomList.h"
#include "UICellItem.h"
#include "UICellItemFactory.h"
#include "xrUICore/Static/UIStatic.h"
#include "Inventory.h"
#include "InventoryOwner.h"
#include "Actor.h"
#include "actor_defs.h"
#include "UIInventoryUtilities.h"
#include "xrEngine/xr_input.h"
#include "xrUICore/Cursor/UICursor.h"
#include "UICellItemFactory.h"
#include "UIHelper.h"
#include "UIItemInfo.h"

CUIDragDropCustomList::CUIDragDropCustomList()
{
    m_ItemInfo = xr_new<CUIItemInfo>();
    m_ItemInfo->InitItemInfo("actor_menu_item.xml");
}

CUIDragDropCustomList::~CUIDragDropCustomList() 
{ xr_delete(m_ItemInfo); }

void CUIDragDropCustomList::SetItem(CUICellItem* itm)
{
    if (GetContainer()->AddSimilar(itm))
        return;

    Ivector2 dest_cell_pos = GetContainer()->FindFreeCell(itm->GetGridSize());

    R_ASSERT(GetContainer()->IsRoomFree(dest_cell_pos, itm->GetGridSize()));

    GetContainer()->PlaceItemAtPos(itm, dest_cell_pos);

    itm->SetWindowName("cell_item");
    Register(itm);
    itm->SetOwnerList(this);
}

CUICellItem* CUIDragDropCustomList::RemoveItem(CUICellItem* itm, bool force_root)
{
    CUICellItem* i = GetContainer()->RemoveItem(itm, force_root);
    i->SetOwnerList((CUIDragDropCustomList*)NULL);
    return i;
}

void CUIDragDropCustomList::OnItemStartDragging(CUIWindow* w, void* pData) {}

void CUIDragDropCustomList::OnItemDrop(CUIWindow* w, void* pData) {}

void CUIDragDropCustomList::OnItemSelected(CUIWindow* w, void* pData) { OnItemFocusReceived(w, pData); }

void CUIDragDropCustomList::OnItemLButtonClick(CUIWindow* w, void* pData) {}

void CUIDragDropCustomList::OnItemRButtonClick(CUIWindow* w, void* pData) {}

void CUIDragDropCustomList::OnItemDBClick(CUIWindow* w, void* pData) {}

void CUIDragDropCustomList::OnItemFocusReceived(CUIWindow* w, void* pData)
{
    CUICellItem* p_cell_item = smart_cast<CUICellItem*>(w);
    if (p_cell_item)
    {
        p_cell_item->m_selected = true;
    }
}

void CUIDragDropCustomList::OnItemFocusLost(CUIWindow* w, void* pData)
{
    CUICellItem* p_cell_item = smart_cast<CUICellItem*>(w);
    if (p_cell_item)
    {
        p_cell_item->m_selected = false;
        m_ItemInfo->InitItem(nullptr);
    }
}

void CUIDragDropCustomList::OnItemFocusedUpdate(CUIWindow* w, void* pData)
{
    CUICellItem* p_cell_item = smart_cast<CUICellItem*>(w);
    if (p_cell_item && p_cell_item->m_selected)
    {
        m_ItemInfo->InitItem(p_cell_item);
        if (GetUICursor().IsVisible())
        {
            Fvector2 temp = GetUICursor().GetCursorPosition();
            clamp(temp.y, 0.f, UI_BASE_HEIGHT - m_ItemInfo->GetHeight());
            m_ItemInfo->SetWndPos({temp.x - m_ItemInfo->GetWndSize().x, temp.y});
        }
    }
    else
        m_ItemInfo->InitItem(nullptr);
}

void CUIDragDropCustomList::Draw()
{
    inherited::Draw();
    if (m_ItemInfo)
        m_ItemInfo->Draw();
}

void CUIDragDropCustomList::Update()
{
    inherited::Update();
    if (m_ItemInfo)
        m_ItemInfo->Update();
}
