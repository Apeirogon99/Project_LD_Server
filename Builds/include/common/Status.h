#pragma once

#define MAX_STATS_NUM 21

enum EStat
{
	armor_penetration,			//방어구 관통력
	attack_damage,				//공격력
	attack_speed,				//공격 속도
	critical_strike_chance,		//치명타 확율
	cirtical_strike_damage,		//치명타 대미지
	life_steal,					//기본 피해 흡수
	ability_power,				//마법력
	mage_penetration,			//마법 관통력
	omnivamp,					//스킬 피해 흡수
	physical_vamp,				//?
	armor,						//방어력
	heal_and_shield_power,		//힐 또는 방어막 증가량
	health,						//체력
	health_regeneration,		//체력 회복량
	magic_resistance,			//마법 방어력
	tenacity,					//군중제어 저항력
	slow_resist,				//슬로우 저항력
	ability_haste,				//재사용 대기시간 감소
	mana,						//마나
	mana_regeneration,			//마나 회복량
	movement_speed,				//이동 속도
	range,						//공격 범위
};

class Stats
{
public:
	Stats() { Clear(); }
	~Stats() {}

	Stats(const Stats& inStatus) { this->InitStats(inStatus.mStats); }
	Stats(Stats&& inStatus) { this->InitStats(inStatus.mStats); }

	Stats& operator=(const Stats& inStatus) noexcept { this->InitStats(inStatus.mStats); return *this; }
	Stats& operator=(Stats&& inStatus) noexcept { this->InitStats(inStatus.mStats); return *this; }

public:
	void Clear()
	{
		::memset(mStats, 0, MAX_STATS_NUM);
	}

	void InitStats(const float inStats[MAX_STATS_NUM])
	{
		::memcpy(mStats, inStats, MAX_STATS_NUM);
	}

	void InitStats(const float inArmorPenetration, const float inAttackDamage, const float inAttackSpeed, const float inCriticalStrikeChance, const float inCirticalStrikeDamage, const float inLifeSteal, const float inAbilityPower, const float inMagePenetration, const float inOmnivamp, const float inPhysicalVamp, const float inArmor, const float inHealAndShieldPower, const float inHealth, const float inHealthRegeneration, const float inMagicResistance, const float inTenacity, const float inSlowResist, const float inAbilityHaste, const float inMana, const float inManaRegeneration, const float inMovementSpeed, const float inRange)
	{
		mArmorPenetration		= inArmorPenetration;
		mAttackDamage			= inAttackDamage;
		mAttackSpeed			= inAttackSpeed;
		mCriticalStrikeChance	= inCriticalStrikeChance;
		mCirticalStrikeDamage	= inCirticalStrikeDamage;
		mLifeSteal				= inLifeSteal;
		mAbilityPower			= inAbilityPower;
		mMagePenetration		= inMagePenetration;
		mOmnivamp				= inOmnivamp;
		mPhysicalVamp			= inPhysicalVamp;
		mArmor					= inArmor;
		mHealAndShieldPower		= inHealAndShieldPower;
		mHealth					= inHealth;
		mHealthRegeneration		= inHealthRegeneration;
		mMagicResistance		= inMagicResistance;
		mTenacity				= inTenacity;
		mSlowResist				= inSlowResist;
		mAbilityHaste			= inAbilityHaste;
		mMana					= inMana;
		mManaRegeneration		= inManaRegeneration;
		mMovementSpeed			= inMovementSpeed;
		mRange					= inRange;
	}

public:
	void SetArmorPenetration(const float inArmorPenetration)			{ mArmorPenetration		= inArmorPenetration;}
	void SetAttackDamage(const float inAttackDamage)					{ mAttackDamage			= inAttackDamage; }
	void SetAttackSpeed(const float inAttackSpeed)						{ mAttackSpeed			= inAttackSpeed; }
	void SetCriticalStrikeChance(const float inCriticalStrikeChance)	{ mCriticalStrikeChance = inCriticalStrikeChance; }
	void SetCirticalStrikeDamage(const float inCirticalStrikeDamage)	{ mCirticalStrikeDamage = inCirticalStrikeDamage; }
	void SetLifeSteal(const float inLifeSteal)							{ mLifeSteal			= inLifeSteal; }
	void SetAbilityPower(const float inAbilityPower)					{ mAbilityPower			= inAbilityPower; }
	void SetMagePenetration(const float inMagePenetration)				{ mMagePenetration		= inMagePenetration; }
	void SetOmnivamp(const float inOmnivamp)							{ mOmnivamp				= inOmnivamp; }
	void SetPhysicalVamp(const float inPhysicalVamp)					{ mPhysicalVamp			= inPhysicalVamp; }
	void SetArmor(const float inArmor)									{ mArmor				= inArmor; }
	void SetHealAndShieldPower(const float inHealAndShieldPower)		{ mHealAndShieldPower	= inHealAndShieldPower; }
	void SetHealth(const float inHealth)								{ mHealth				= inHealth; }
	void SetHealthRegeneration(const float inHealthRegeneration)		{ mHealthRegeneration	= inHealthRegeneration; }
	void SetMagicResistance(const float inMagicResistance)				{ mMagicResistance		= inMagicResistance; }
	void SetTenacity(const float inTenacity)							{ mTenacity				= inTenacity; }
	void SetSlowResist(const float inSlowResist)						{ mSlowResist			= inSlowResist; }
	void SetAbilityHaste(const float inAbilityHaste)					{ mAbilityHaste			= inAbilityHaste; }
	void SetMana(const float inMana)									{ mMana					= inMana; }
	void SetManaRegeneration(const float inManaRegeneration)			{ mManaRegeneration		= inManaRegeneration; }
	void SetMovementSpeed(const float inMovementSpeed)					{ mMovementSpeed		= inMovementSpeed; }
	void SetRange(const float inRange)									{ mRange				= inRange; }
		
public:
	inline const float GetArmorPenetration()		const { return mArmorPenetration;}
	inline const float GetAttackDamage()			const { return mAttackDamage; }
	inline const float GetAttackSpeed()				const { return mAttackSpeed; }
	inline const float GetCriticalStrikeChance()	const { return mCriticalStrikeChance; }
	inline const float GetCirticalStrikeDamage()	const { return mCirticalStrikeDamage; }
	inline const float GetLifeSteal()				const { return mLifeSteal; }
	inline const float GetAbilityPower()			const { return mAbilityPower; }
	inline const float GetMagePenetration()			const { return mMagePenetration; }
	inline const float GetOmnivamp()				const { return mOmnivamp; }
	inline const float GetPhysicalVamp()			const { return mPhysicalVamp; }
	inline const float GetArmor()					const { return mArmor; }
	inline const float GetHealAndShieldPower()		const { return mHealAndShieldPower; }
	inline const float GetHealth()					const { return mHealth; }
	inline const float GetHealthRegeneration()		const { return mHealthRegeneration; }
	inline const float GetMagicResistance()			const { return mMagicResistance; }
	inline const float GetTenacity()				const { return mTenacity; }
	inline const float GetSlowResist()				const { return mSlowResist; }
	inline const float GetAbilityHaste()			const { return mAbilityHaste; }
	inline const float GetMana()					const { return mMana; }
	inline const float GetManaRegeneration()		const { return mManaRegeneration; }
	inline const float GetMovementSpeed()			const { return mMovementSpeed; }
	inline const float GetRange()					const { return mRange; }

	float* GetStats() { return mStats; }

private:
	union
	{
		struct
		{
			float mArmorPenetration;
			float mAttackDamage;
			float mAttackSpeed;
			float mCriticalStrikeChance;
			float mCirticalStrikeDamage;
			float mLifeSteal;
			float mAbilityPower;
			float mMagePenetration;
			float mOmnivamp;
			float mPhysicalVamp;
			float mArmor;
			float mHealAndShieldPower;
			float mHealth;
			float mHealthRegeneration;
			float mMagicResistance;
			float mTenacity;
			float mSlowResist;
			float mAbilityHaste;
			float mMana;
			float mManaRegeneration;
			float mMovementSpeed;
			float mRange;
		};

		float mStats[MAX_STATS_NUM];
	};
};