#pragma once
#include "UIDragDropListEx.h"
#include "xrServerEntities/inventory_space.h"
class CInventoryOwner;

class CUIDragDropCustomList final : public CUIDragDropListEx
{
    typedef CUIDragDropListEx inherited;

public:
    CUIDragDropCustomList();
    ~CUIDragDropCustomList();
    CUICellContainer* GetContainer() { return m_container; };
    virtual void SetItem(CUICellItem* itm);
    virtual CUICellItem* RemoveItem(CUICellItem* itm, bool force_root);

    virtual void OnItemStartDragging(CUIWindow* w, void* pData);
    virtual void OnItemDrop(CUIWindow* w, void* pData);
    virtual void OnItemSelected(CUIWindow* w, void* pData);
    virtual void OnItemLButtonClick(CUIWindow* w, void* pData);
    virtual void OnItemRButtonClick(CUIWindow* w, void* pData);
    virtual void OnItemDBClick(CUIWindow* w, void* pData);
    virtual void OnItemFocusReceived(CUIWindow* w, void* pData);
    virtual void OnItemFocusLost(CUIWindow* w, void* pData);
    virtual void OnItemFocusedUpdate(CUIWindow* w, void* pData);

    
    virtual void Draw();
    virtual void Update();
    float dx_pos{0.f};

private:
    CUIItemInfo* m_ItemInfo{};
};
