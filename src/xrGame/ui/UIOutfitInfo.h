#pragma once
#include "xrUICore/Windows/UIWindow.h"
#include "xrUICore/ProgressBar/UIDoubleProgressBar.h"
#include "xrServerEntities/alife_space.h"
#define ITEMS_ARRAY_MAX 20
class CCustomOutfit;
class CHelmet;
class CActorGlove;
class CUIStatic;
class CUIDoubleProgressBar;
class CUIXml;

class CUIOutfitImmunity final : public CUIWindow
{
public:
    CUIOutfitImmunity();

    bool InitFromXml(CUIXml& xml_doc, LPCSTR base_str, cpcstr name_str, cpcstr desc_str);
    void SetValue(float cur, float comp, bool items_equal);

    pcstr GetDebugType() override { return "CUIOutfitImmunity"; }

protected:
    CUIStatic m_name; // texture + name
    CUIStatic m_value; // 100%
    CUIStatic m_value2; // 100%
    float m_magnitude;

}; // class CUIOutfitImmunity

// -------------------------------------------------------------------------------------

class CUIOutfitInfo final : public CUIWindow
{
public:
    CUIOutfitInfo() : CUIWindow("CUIOutfitInfo") {}

    void InitFromXml(CUIXml& xml_doc);
    void UpdateInfo(CCustomOutfit* cur_outfit, CCustomOutfit* slot_outfit = nullptr);
    void UpdateInfo(CHelmet* cur_helmet, CHelmet* slot_helmet = nullptr);
    void UpdateInfo(CActorGlove* cur_glove, CActorGlove* slot_glove = nullptr);

    pcstr GetDebugType() override { return "CUIOutfitInfo"; }

protected:
    CUIStatic* m_caption{};
    CUIStatic* m_Prop_line{};
    xr_array<CUIOutfitImmunity*, ITEMS_ARRAY_MAX> m_items{};

}; // class CUIOutfitInfo
