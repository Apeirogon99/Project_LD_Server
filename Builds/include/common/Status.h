#pragma once

#define MAX_STATS_NUM 22

enum class EStatType
{
	Stat_Unspecified,			//존재하지 않음
	Stat_ArmorPenetration,		//방어구 관통력
	Stat_AttackDamage,			//공격력
	Stat_AttackSpeed,			//공격 속도
	Stat_CriticalStrikeChance,	//치명타 확율
	Stat_CirticalStrikeDamage,	//치명타 대미지
	Stat_LifeSteal,				//기본 피해 흡수
	Stat_AbilityPower,			//마법력
	Stat_MagePenetration,		//마법 관통력
	Stat_Omnivamp,				//스킬 피해 흡수
	Stat_PhysicalVamp,			//?
	Stat_Armor,					//방어력
	Stat_HealAndShieldPower,	//힐 또는 방어막 증가량
	Stat_Health,				//체력
	Stat_HealthRegeneration,	//체력 회복량
	Stat_MagicResistance,		//마법 방어력
	Stat_Tenacity,				//군중제어 저항력
	Stat_SlowResist,			//슬로우 저항력
	Stat_AbilityHaste,			//재사용 대기시간 감소
	Stat_Mana,					//마나
	Stat_ManaRegeneration,		//마나 회복량
	Stat_MovementSpeed,			//이동 속도
	Stat_Range,					//공격 범위
};

enum class EBuffType
{
	Buff_Unspecified,			//존재하지 않음
	DeBuff_BlinkSturn,
	Buff_Berserk_AttackDamage,
	DeBuff_Berserk_Armor,
	DeBuff_ShoulShackles,
	DeBuff_RealmOfDeath,
	Buff_Warrior_AttackDamage,
	Buff_Warrior_Armor,
	Buff_Warrior_MovementSpeed,
	Buff_Dash,
	DeBuff_WarriorShiedBash,
	DeBuff_OnslaughtOfShadows,
};

class Stats
{
public:
	Stats() { Clear(); }
	~Stats() {}

	Stats(const Stats& inStatus) { this->InitStats(inStatus.mStats); }
	Stats(Stats&& inStatus) noexcept { this->InitStats(inStatus.mStats); }

	Stats& operator=(const Stats& inStatus) { this->InitStats(inStatus.mStats); return *this; }
	Stats& operator=(Stats&& inStatus) noexcept { this->InitStats(inStatus.mStats); return *this; }

	Stats& operator+=(const Stats& inStatus)
	{
		for (int32 index = 0; index < MAX_STATS_NUM; ++index)
		{
			this->SetStats(index, this->GetStat(index) + inStatus.GetStat(index));
		}
		return *this;
	}

	Stats operator+(const Stats& inStats)
	{
		Stats stats;
		for (int32 index = 0; index < MAX_STATS_NUM; ++index)
		{
			stats.SetStats(index, this->GetStat(index) + inStats.GetStat(index));
		}
		return stats;
	}

	Stats operator*(const int32 inVlaue)
	{
		Stats stats;
		for (int32 index = 0; index < MAX_STATS_NUM; ++index)
		{
			stats.SetStats(index, this->GetStat(index) * inVlaue);
		}
		return stats;
	}

public:
	void Clear()
	{
		::memset(mStats, 0, sizeof(float) * MAX_STATS_NUM);
	}

	void InitStats(const float inStats[MAX_STATS_NUM])
	{
		::memcpy(mStats, inStats, sizeof(float) * MAX_STATS_NUM);
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
	void SetStats(const int32 inIndex, const float inFValue)			{ mStats[inIndex] = inFValue; }
		
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
	const float*	   GetStats()					const { return mStats; }
	const float		   GetStat(int32 inIndex)		const { return mStats[inIndex]; }

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

class StatUtils
{
public:
	const static float RandomDamage(const float inDamage) { return static_cast<float>(Random::GetNormalDistribution(inDamage / 2, 0.8) + inDamage * 0.2); }
	const static int64 CoolTime(const float inBasic, const float inEqipment, const float inBuff, const float inDeBuff) { return FloatToMillSecond(1.0f / inBasic * (1.0f + inEqipment) * (1.0f + inBuff) * (1.0f - inDeBuff)); }


private:
	const static int64 FloatToMillSecond(const float inFloatTime) { return static_cast<int64>(inFloatTime * 1000); }
};