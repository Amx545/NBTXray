#pragma once
#include "UIDialogWnd.h"
#include "xrUICore/listbox/UIListBoxItem.h"
#include "xrUICore/Windows/UIWindow.h"
#include "xrUICore/Callbacks/UIWndCallback.h"
#include "xrUICore/Windows/UIFrameLineWnd.h"
/**
enum ESelectedType
{
    eActorClassSecurity = 0,
    eActorClassForgotten,
    eActorClassMercenary,
    eActorClassBandit,
    eActorClassHunter,
    eActorClassFinder,
    eActorClassDigger,
    eActorClassEnd
};
*/
struct SActorClassInfo
{
    u8 selected_type;
    u8 iActorClassLevel;
    u8 iActorClassVitality;
    u8 iActorClassStrength;
    u8 iActorClassIntelligence;
    u8 iActorClassDexterity;
    shared_str iActorClassDescription;
    shared_str cActorIcon;
    shared_str cActorClassItems;
};
struct SSpawnedItemStatus
{
    shared_str sName;
    bool bStatus = 0;
};
class CUIStatic;
class CUIXml;
class CUIFrameLineWnd;
class CUIFrameWindow;
class UIHint;
class UIHintWindow;
class CUI3tButton;
class CUIComboBox;
class CUIListBoxItem;
class UIDragDropCustomList;

class CUIActorClassSelector final : public CUIDialogWnd, public CUIWndCallback
{
private:

    typedef CUIDialogWnd inherited;

public:
    CUIActorClassSelector();
    virtual ~CUIActorClassSelector() override;
    virtual void Init();

    virtual bool StopAnyMove() { return true; }

    virtual void Draw();
    virtual void Show(bool status);
    virtual void ShowDialog(bool bDoHideIndicators);
    virtual void HideDialog();
    virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);

    virtual void Update();

    void OnActorClassSelected();
    void OnActorClassApply();

    pcstr GetDebugType() override { return "CUIActorClassSelector"; }
private:
    CUIFrameWindow* m_downbackground{};
    CUIFrameWindow* m_pClassOverlay{};
    CUIFrameWindow* m_pStatsOverlay{};
    CUIFrameWindow* m_pDescrOverlay{};
    CUIFrameWindow* m_pPersonOverlay{};
    CUIFrameWindow* m_pSuppliesOverlay{};
    CUIListBox* m_pActorClassList{};
    CUIStatic* m_background{};
    CUIStatic* m_pStaticPerson{};
    CUIStatic* m_pActorLvl{};
    CUIStatic* m_pActorStr{};
    CUIStatic* m_pActorDex{};
    CUIStatic* m_pActorInt{};
    CUIStatic* m_pActorVit{};
    CUIStatic* m_pClassDescr{};
    CUIScrollView* m_pActorClassDescr{};
    CUIDragDropCustomList* m_pList;
    CUI3tButton* btn_apply;
    xr_vector<SActorClassInfo> m_actor_class_info;
    u8 ELastSelectedID;
    CInifile* m_ini_file;
    xr_vector<SSpawnedItemStatus> m_iItemListID;
};

