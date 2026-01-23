// ActorCondition.h: класс состояния игрока
//

#pragma once

#include "EntityCondition.h"
#include "actor_defs.h"

template <typename _return_type>
class CScriptCallbackEx;
class CActor;
class CActorDeathEffector;

class CActorCondition : public CEntityCondition
{
private:
    typedef CEntityCondition inherited;
    enum
    {
        eCriticalPowerReached = (1 << 0),
        eCriticalMaxPowerReached = (1 << 1),
        eCriticalBleedingSpeed = (1 << 2),
        eCriticalSatietyReached = (1 << 3),
        eCriticalRadiationReached = (1 << 4),
        eWeaponJammedReached = (1 << 5),
        ePhyHealthMinReached = (1 << 6),
        eCantWalkWeight = (1 << 7),
        eCantWalkWeightReached = (1 << 8),
    };
    Flags16 m_condition_flags;

private:
    CActor* m_object;
    CActorDeathEffector* m_death_effector;
    void UpdateTutorialThresholds();
    void UpdateSatiety();
    virtual void UpdateRadiation();
    virtual void UpdatePsyHealth();
    void UpdateStats();

public:
    CActorCondition(CActor* object);
    virtual ~CActorCondition();

    virtual void LoadCondition(LPCSTR section);
    virtual void reinit();

    virtual CWound* ConditionHit(SHit* pHDS);
    virtual void UpdateCondition();
    void UpdateBoosters();

    virtual void ChangeAlcohol(const float value);
    virtual void ChangeSatiety(const float value);

    void BoostParameters(const SBooster& B);
    void DisableBoostParameters(const SBooster& B);
    void WoundForEach(const luabind::functor<bool>& funct);
    void BoosterForEach(const luabind::functor<bool>& funct);
    bool ApplyBooster_script(const SBooster& B, LPCSTR sect);
    void ClearAllBoosters();
    void BoostMaxWeight(const float value);
    void BoostHpRestore(const float value);
    void BoostPowerRestore(const float value);
    void BoostRadiationRestore(const float value);
    void BoostBleedingRestore(const float value);
    void BoostBurnImmunity(const float value);
    void BoostShockImmunity(const float value);
    void BoostRadiationImmunity(const float value);
    void BoostTelepaticImmunity(const float value);
    void BoostChemicalBurnImmunity(const float value);
    void BoostExplImmunity(const float value);
    void BoostStrikeImmunity(const float value);
    void BoostFireWoundImmunity(const float value);
    void BoostWoundImmunity(const float value);
    void BoostRadiationProtection(const float value);
    void BoostTelepaticProtection(const float value);
    void BoostChemicalBurnProtection(const float value);
    void BoostGraveImmunity(const float value);
    void BoostHealthIncrease(const float value);
    void BoostPowerIncrease(const float value);
    void BoostSniper(const float value);
    void BoostDoubleShot(const float value);
    void BoostSpeedShot(const float value);
    void BoostSpeedReload(const float value);
    void BoostMoveSpeedIncrease(const float value);
    void BoostJumpIncrease(const float value);
    void BoostMoveSpeedMinus(const float value);
    void BoostJumpMinus(const float value);
    const auto& GetCurBoosterInfluences() const { return m_booster_influences; }
    // хромание при потере сил и здоровья
    virtual bool IsLimping() const;
    virtual bool IsCantWalk() const;
    virtual bool IsCantWalkWeight();
    virtual bool IsCantSprint() const;
    float GetDamagePerk() { return m_fBoostDoubleShot; }
    float GetSpeedReloadPerk() { return m_fBoostSpeedReload; }
    float GetSpeedShotPerk() { return m_fBoostSpeedShot; }
    float GetSniperPerk() { return m_fBoostSniper; }

    void PowerHit(float power, bool apply_outfit);
    float GetPower() const { return m_fPower; }
    void ConditionJump(float weight);
    void ConditionWalk(float weight, bool accel, bool sprint);
    void ConditionStand(float weight);
    IC float MaxWalkWeight() const { return m_MaxWalkWeight; }
    float GetAlcohol() { return m_fAlcohol; }
    float GetPsy() { return 1.0f - GetPsyHealth(); }
    float GetSatiety() { return m_fSatiety; }
    IC float GetSatietyPower() const { return m_fV_SatietyPower * m_fSatiety; };
    void AffectDamage_InjuriousMaterialAndMonstersInfluence();
    float GetInjuriousMaterialDamage();

    void SetZoneDanger(float danger, ALife::EInfluenceType type);
    float GetZoneDanger() const;

public:
    IC CActor& object() const
    {
        VERIFY(m_object);
        return (*m_object);
    }
    virtual void save(NET_Packet& output_packet);
    virtual void load(IReader& input_packet);
    //	IC		float const&	Satiety					()	{ return m_fSatiety; }
    IC float const& V_Satiety() { return m_fV_Satiety; }
    IC float const& V_SatietyPower() { return m_fV_SatietyPower; }
    IC float const& V_SatietyHealth() { return m_fV_SatietyHealth; }
    IC float const& SatietyCritical() { return m_fSatietyCritical; }
    float GetZoneMaxPower(ALife::EInfluenceType type) const;
    float GetZoneMaxPower(ALife::EHitType hit_type) const;

    bool DisableSprint(SHit* pHDS);
    bool PlayHitSound(SHit* pHDS);
    float HitSlowmo(SHit* pHDS);
    virtual bool ApplyInfluence(const SMedicineInfluenceValues& V, const shared_str& sect);
    virtual bool ApplyBooster(const SBooster& B, const shared_str& sect);
    float GetMaxPowerRestoreSpeed() const { return m_max_power_restore_speed; };
    float GetMaxWoundProtection() const { return m_max_wound_protection; };
    float GetMaxFireWoundProtection() const { return m_max_fire_wound_protection; };
protected:
    SMedicineInfluenceValues m_curr_medicine_influence;
    float m_fAlcohol;
    float m_fV_Alcohol;
    //--
    float m_fSatiety;
    float m_fV_Satiety;
    float m_fV_SatietyPower;
    float m_fV_SatietyHealth;
    float m_fSatietyCritical;
    //--
    //--perks--
    float m_fBoostAdditionalWeight{0.f};
    float m_fBoostGraveImmunity{0.f};
    float m_fBoostHealthIncrease{0.f};
    float m_fBoostPowerIncrease{0.f};
    float m_fBoostSniper{1.f};
    float m_fBoostDoubleShot{1.f};
    float m_fBoostSpeedShot{1.f};
    float m_fBoostSpeedReload{1.f};
    float m_fBoostMoveSpeedIncrease{0.f};
    float m_fBoostJumpIncrease{0.f};
    //--perks--
    float m_fPowerLeakSpeed;
    xr_array<u16,100> HPLeveling;

    float m_fJumpPower;
    float m_fStandPower;
    float m_fWalkPower;
    float m_fJumpWeightPower;
    float m_fWalkWeightPower;
    float m_fOverweightWalkK;
    float m_fOverweightJumpK;
    float m_fAccelK;
    float m_fSprintK;
    //--stats--
    u32 m_iActorLevel;
    u32 m_iActorPoint;
    u32 m_iActorVitality;
    u32 m_iActorStrength;
    u32 m_iActorIntelligence;
    u32 m_iActorDexterity;
    u32 m_fActorExperience;
    u32 m_fActorRequiredExperience;
    u32 m_iActorAbilityPoint;

    u32 m_iActorVitalityTemp;
    u32 m_iActorStrengthTemp;
    u32 m_iActorIntelligenceTemp;
    u32 m_iActorDexterityTemp;
    //--stats--
    float stored_MaxWalkWeight{60.f};

    float m_fHealthLevel{0.f};
    float m_fPowerLevel{0.f};
    float m_fWeightLevel{0.f};
    float m_fPsyHealthLevel{0.f};
    float m_fStrScaleLevel{0.f};
    float m_fDexScaleLevel{0.f};
    float m_fIntScaleLevel{0.f};

    float m_fItemHealth{0.f};
    float m_fItemPower{0.f};
    float m_fItemPsyHealth{0.f};
    float m_fStrScaleItem{0.f};
    float m_fDexScaleItem{0.f};
    float m_fIntScaleItem{0.f};

    float m_fLvlUpHeal{0.f};
    float m_fLvlUpPower{0.f};
    float m_fLvlUpPsyHeal{0.f};

public:
    float m_MaxWalkWeight;
    //--stats--
    void SetActorPointLevel(u32 val) { m_iActorPoint = val; }
    void SetActorLevel(u8 val);
    void SetActorVitality(u32 val) { m_iActorVitality = val; }
    void SetActorStrength(u32 val) { m_iActorStrength = val; }
    void SetActorIntelligence(u32 val) { m_iActorIntelligence = val; }
    void SetActorDexterity(u32 val) { m_iActorDexterity = val; }
    void SetAbilityPoint(u32 val) { m_iActorAbilityPoint = val; }

    u32 GetActorLevel() const { return m_iActorLevel; }
    u32 GetActorPointLevel() const { return m_iActorPoint; }
    u32 GetActorVitality() const { return m_iActorVitality; }
    u32 GetActorStrength() const { return m_iActorStrength; }
    u32 GetActorIntelligence() const { return m_iActorIntelligence; }
    u32 GetActorDexterity() const { return m_iActorDexterity; }
    u32 GetActorExperience() const { return m_fActorExperience; }
    u32 GetActorRequiredExp() const { return m_fActorRequiredExperience; }
    u32 GetAbilityPoint() const { return m_iActorAbilityPoint; }

    u32 GetActorVitalityTemp() const { return m_iActorVitalityTemp; }
    u32 GetActorIntelligenceTemp() const { return m_iActorIntelligenceTemp; }
    u32 GetActorStrengthTemp() const { return m_iActorStrengthTemp; }
    u32 GetActorDexterityTemp() const { return m_iActorDexterityTemp; }

    u16 GetHealthPerLevel(u8 current_level);
    float GetWeightPerLevel(u8 current_level);
    u16 GetPsyHealthPerLevel(u8 current_level);
    u16 GetPowerPerLevel(u8 current_level);

    float GetStrScalePerLevel(u8 current_level);
    float GetDexScalePerLevel(u8 current_level);
    float GetIntScalePerLevel(u8 current_level);

    float GetActorHTProtection(ALife::EHitType hit_type);
    float GetActorHTProtection(bool head);

    void GetActorLevelUp();

    float GetActorHealthLevel() const { return m_fHealthLevel; }
    float GetActorPowerLevel() const { return m_fPowerLevel; }
    float GetActorWeightLevel() const { return m_fWeightLevel; }
    float GetActorPsyHealthLevel() const { return m_fPsyHealthLevel; }
    float GetActorStrScaleLevel() const { return m_fStrScaleLevel; }
    float GetActorDexScaleLevel() const { return m_fDexScaleLevel; }
    float GetActorIntScaleLevel() const { return m_fIntScaleLevel; }

    float GetActorItemHealth() const { return m_fItemHealth; }
    float GetActorItemPower() const { return m_fItemPower; }
    float GetActorItemPsyHealth() const { return m_fItemPsyHealth; }
    float GetActorItemStrScale() const { return m_fStrScaleItem; }
    float GetActorItemDexScale() const { return m_fDexScaleItem; }
    float GetActorItemIntScale() const { return m_fIntScaleItem; }

    void SetActorHealthLevel(float val) { m_fHealthLevel = val; }
    void SetActorPowerLevel(float val) { m_fPowerLevel = val; }
    void SetActorWeightLevel(float val) { m_fWeightLevel = val; }
    void SetActorPsyHealthLevel(float val) { m_fPsyHealthLevel = val; }

    void SetActorItemHealth(float val) { m_fItemHealth = val; }
    void SetActorItemPower(float val) { m_fItemPower = val; }
    void SetActorItemPsyHealth(float val) { m_fItemPsyHealth = val; }

    void ChangeStatsVitality(u8 val, bool add, bool permanent);
    void ChangeStatsStrength(u8 val, bool add, bool permanent);
    void ChangeStatsIntelligence(u8 val, bool add, bool permanent);
    void ChangeStatsDexterity(u8 val, bool add, bool permanent);

    void ChangeStatsHealth(float val, bool add);
    void ChangeStatsPsyHealth(float val, bool add);
    void ChangeStatsPower(float val, bool add);

    //--stats--
protected:
    float m_zone_max_power[ALife::infl_max_count];
    float m_zone_danger[ALife::infl_max_count];
    float m_f_time_affected;
    float m_max_power_restore_speed;
    float m_max_wound_protection;
    float m_max_fire_wound_protection;

    mutable bool m_bLimping;
    mutable bool m_bCantWalk;
    mutable bool m_bCantSprint;

    //порог силы и здоровья меньше которого актер начинает хромать
    float m_fLimpingPowerBegin;
    float m_fLimpingPowerEnd;
    float m_fCantWalkPowerBegin;
    float m_fCantWalkPowerEnd;

    float m_fCantSprintPowerBegin;
    float m_fCantSprintPowerEnd;

    float m_fLimpingHealthBegin;
    float m_fLimpingHealthEnd;

    // typedef xr_vector<SMedicineInfluenceValues> BOOSTS_VECTOR;
    // typedef xr_vector<SMedicineInfluenceValues>::iterator BOOSTS_VECTOR_ITER;
    // BOOSTS_VECTOR m_vecBoosts;
    ref_sound m_use_sound;
};

class CActorDeathEffector
{
    CActorCondition* m_pParent;
    ref_sound m_death_sound;
    bool m_b_actual;
    float m_start_health;
    void OnPPEffectorReleased();

public:
    CActorDeathEffector(CActorCondition* parent, LPCSTR sect); // -((
    ~CActorDeathEffector();
    void UpdateCL();
    IC bool IsActual() { return m_b_actual; }
    void Stop();
};
