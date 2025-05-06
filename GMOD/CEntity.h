
class IClientRenderable;
class modelInfo;
#define	FL_ONGROUND				(1<<0)	// At rest / on the ground
#define FL_DUCKING				(1<<1)	// Player flag -- Player is fully crouched
#define FL_ANIMDUCKING			(1<<2)	// Player flag -- Player is in the process of crouching or uncrouching but could be in transition
// examples:                                   Fully ducked:  FL_DUCKING &  FL_ANIMDUCKING
//           Previously fully ducked, unducking in progress:  FL_DUCKING & !FL_ANIMDUCKING
//                                           Fully unducked: !FL_DUCKING & !FL_ANIMDUCKING
//           Previously fully unducked, ducking in progress: !FL_DUCKING &  FL_ANIMDUCKING
#define	FL_WATERJUMP			(1<<3)	// player jumping out of water
#define FL_ONTRAIN				(1<<4) // Player is _controlling_ a train, so movement commands should be ignored on client during prediction.
#define FL_INRAIN				(1<<5)	// Indicates the entity is standing in rain
#define FL_FROZEN				(1<<6) // Player is frozen for 3rd person camera
#define FL_ATCONTROLS			(1<<7) // Player can't move, but keeps key inputs for controlling another entity
#define	FL_CLIENT				(1<<8)	// Is a player
#define FL_FAKECLIENT			(1<<9)	// Fake client, simulated server side; don't send network messages to them
// NON-PLAYER SPECIFIC (i.e., not used by GameMovement or the client .dll ) -- Can still be applied to players, though
#define	FL_INWATER				(1<<10)	// In water

#define MAX_WEAPONS 256

class CPed {


public:
	enum moveTypes {
		MOVETYPE_NONE,
		MOVETYPE_ISOMETRIC,
		MOVETYPE_WALK,
		MOVETYPE_STEP,
		MOVETYPE_FLY,
		MOVETYPE_FLYGRAVITY,
		MOVETYPE_VPHYSICS,
		MOVETYPE_PUSH,
		MOVETYPE_NOCLIP,
		MOVETYPE_LADDER,
		MOVETYPE_OBSERVER,
		MOVETYPE_CUSTOM,
	};

	enum lifeStates {
		alive = 0,
		dead = 2,
	};

	CVector GetBonePosition(unsigned int boneIndex) {
		CVector bonePos;
		bonePos.x = *reinterpret_cast<float*>(GetCachedBones() + 0x30 * boneIndex + 0xC);
		bonePos.y = *reinterpret_cast<float*>(GetCachedBones() + 0x30 * boneIndex + 0x1C);
		bonePos.z = *reinterpret_cast<float*>(GetCachedBones() + 0x30 * boneIndex + 0x2C);
		return bonePos;
	}

	matrix3x4_t& GetBoneMatrix(unsigned int boneIndex) {
		return ((matrix3x4_t*)GetCachedBones())[boneIndex];
	}

	bool IsPlayer() {
		typedef bool(__thiscall* fn)(void*);
		return VMT.getvfunc<fn>(this, 130)(this);
	}

	IClientRenderable* GetClientRenderable() {
		typedef IClientRenderable* (__thiscall* fn)(void*);
		return VMT.getvfunc<fn>(this, 5)(this);
	}

	bool inline IsAlive() {
		return this->GetLifeState() == lifeStates::alive;
	}

	CVector GetEyePosition() {
		return this->GetVecOrigin() + this->GetVecViewOffset();
	}

	enum LineOfSightCheckType
	{
		IGNORE_NOTHING, // игнорирует любые entity, проходя сквозь них. Видит препятстие только в "статичных объектах" (((( в карте ))))
		IGNORE_ACTORS	// не игнорирует entity на пути
	};

	bool IsLineOfSightClear(CVector const& point, int checkType, CPed* ignoredEntity) {
		typedef bool(__thiscall* fn)(void*, const CVector&, int, CPed*);
		return VMT.getvfunc<fn>(this, 274)(this, point, checkType, ignoredEntity);
	}

	CVector GetViewDirection() {
		typedef float* (__thiscall* fn)(void*, CVector&, float);
		CVector viewDirection;
		constexpr float magicNumber = 0.087155744f;
		VMT.getvfunc<fn>(this, 288)(this, viewDirection, magicNumber); // view angle + aim/view punch;
		return viewDirection;
	}

	void SelectItem(const char* pszItem, int iSubType) {
		typedef void(__thiscall* fn)(void*, const char*, int);
		VMT.getvfunc<fn>(this, 320)(this, pszItem, iSubType);
	}

	void PreThink() {
		typedef void(__thiscall* fn)(void*);
		VMT.getvfunc<fn>(this, 310)(this);
	}

	void Think() {
		typedef void(__thiscall* fn)(void*);
		VMT.getvfunc<fn>(this, 120)(this);
	}

	void PostThink() {
		typedef void(__thiscall* fn)(void*);
		VMT.getvfunc<fn>(this, 311)(this);
	}

	void SetScrollSpeed(int scrollSpeed) {
		typedef void(__thiscall* fn)(void*, int);
		VMT.getvfunc<fn>(this, 374)(this, scrollSpeed);
	}

	void UpdateButtonState(int nButtonMask) {
		typedef void(__thiscall* fn)(void*, int);
		VMT.getvfunc<fn>(this, 325);
	}

	void SetLocalViewAngles(CVector& angles) {
		typedef void(__thiscall* fn)(void*, CVector&);
		VMT.getvfunc<fn>(this, 351);
	}

	int GetHealth() {
		static uintptr_t offset = DTManager::GetOffset("DT_BaseEntity", "m_iHealth");
		return *(int*)((uintptr_t)this + offset);
	}

	int GetMaxHealth() {
		static uintptr_t offset = DTManager::GetOffset("DT_BaseEntity", "m_iMaxHealth");
		return *(int*)((uintptr_t)this + offset);
	}

	CVector& GetVecOrigin() {
		static uintptr_t offset = DTManager::GetOffset("DT_BaseEntity", "m_vecOrigin");
		return *(CVector*)((uintptr_t)this + offset);
	}

	CVector& GetAngRotation() {
		static uintptr_t offset = DTManager::GetOffset("DT_BaseEntity", "m_angRotation");
		return *(CVector*)((uintptr_t)this + offset);
	}

	int GetTeamNum() {
		static uintptr_t offset = DTManager::GetOffset("DT_BaseEntity", "m_iTeamNum");
		return *(int*)((uintptr_t)this + offset);
	}

	CVector& GetVecVelocity() {
		static uintptr_t offset = DTManager::GetOffset("DT_BaseEntity", "m_vecVelocity[0]");
		return *(CVector*)((uintptr_t)this + offset);
	}

	uint8_t& GetMoveType() {
		static uintptr_t offset = DTManager::GetOffset("DT_BaseEntity", "m_iParentAttachment") - 0x2;
		return *(uint8_t*)((uintptr_t)this + offset);
	}

	bool IsDormant() {
		static uintptr_t offset = DTManager::GetOffset("DT_BaseEntity", "m_iParentAttachment") + 0x4;
		return *(bool*)((uintptr_t)this + offset);
	}

	int& GetTickBase() {
		static uintptr_t offset = DTManager::GetOffset("DT_LocalPlayerExclusive", "m_nTickBase");
		return *(int*)((uintptr_t)this + offset);
	}

	int& GetImpulse() {
		static uintptr_t offset = DTManager::GetOffset("DT_LocalPlayerExclusive", "m_nTickBase") - 0x28;
		return *(int*)((uintptr_t)this + offset);
	}

	uint8_t* GetButtonsArray() {
		static uintptr_t offset = DTManager::GetOffset("DT_LocalPlayerExclusive", "m_nTickBase") + 0xD;
		return (uint8_t*)((uintptr_t)this + offset);
	}

	float& GetFallVelocity() {
		static uintptr_t offset = DTManager::GetOffset("DT_LocalPlayerExclusive", "m_Local") + DTManager::GetOffset("DT_Local", "m_flFallVelocity");
		return *(float*)((uintptr_t)this + offset);
	}

	CUserCmd*& GetCurrentCommand() {
		static uintptr_t offset = DTManager::GetOffset("DT_LocalPlayerExclusive", "m_hConstraintEntity") - 0x4;
		return *(CUserCmd**)((uintptr_t)this + offset);
	}

	CVector& GetVecPunchAngle() {
		static uintptr_t offset = DTManager::GetOffset("DT_LocalPlayerExclusive", "m_Local") + DTManager::GetOffset("DT_Local", "m_vecPunchAngle");
		return *(CVector*)((uintptr_t)this + offset);
	}

	CVector& GetVecPunchAngleVel() {
		static uintptr_t offset = DTManager::GetOffset("DT_LocalPlayerExclusive", "m_Local") + DTManager::GetOffset("DT_Local", "m_vecPunchAngleVel");
		return *(CVector*)((uintptr_t)this + offset);
	}

	uint8_t GetLifeState() {
		static uintptr_t offset = DTManager::GetOffset("DT_BasePlayer", "m_lifeState");
		return *(uint8_t*)((uintptr_t)this + offset);
	}

	uint8_t& GetFlags() {
		static uintptr_t offset = DTManager::GetOffset("DT_BasePlayer", "m_fFlags");
		return *(uint8_t*)((uintptr_t)this + offset);
	}

	uint32_t GetActiveWeaponHandle() {
		static uintptr_t offset = DTManager::GetOffset("DT_BaseCombatCharacter", "m_hActiveWeapon");
		return *(uint32_t*)((uintptr_t)this + offset);
	}

	uintptr_t* GetWeaponHandles() {
		static uintptr_t offset = DTManager::GetOffset("DT_BaseCombatCharacter", "m_hMyWeapons");
		return (uint32_t*)((uintptr_t)this + offset);
	}

	uintptr_t GetWeaponHandle(int i) {
		if (i < 0 || i >= MAX_WEAPONS) return NULL;
		auto weapons = GetWeaponHandles();
		auto weaponHandle = weapons[i];
		return weaponHandle;
	}

	modelInfo* GetModelInfo() {
		static uintptr_t offset = DTManager::GetOffset("DT_AnimTimeMustBeFirst", "m_flAnimTime") - 0x4;
		return *(modelInfo**)((uintptr_t)this + offset);
	}

	uint32_t GetCachedBones() {
		static uintptr_t offset = DTManager::GetOffset("DT_BaseAnimating", "m_bClientSideFrameReset") - 0x1C;
		return *(uint32_t*)((uintptr_t)this + offset);
	}

	CVector& GetVecViewOffset() {
		static uintptr_t offset = DTManager::GetOffset("DT_BaseFlex", "m_vecViewOffset[0]");
		return *(CVector*)((uintptr_t)this + offset);
	}

	int& GetObserverMode() {
		static uintptr_t offset = DTManager::GetOffset("DT_BasePlayer", "m_iObserverMode");
		return *(int*)((uintptr_t)this + offset);
	}

	bool IsObserver() {
		return GetObserverMode() != OBS_MODE_NONE;
	}

	uintptr_t GetObserverTarget() {
		static uintptr_t offset = DTManager::GetOffset("DT_BasePlayer", "m_hObserverTarget");
		return *(uintptr_t*)((uintptr_t)this + offset);
	}

	enum {
		OBS_MODE_NONE = 0,	// not in spectator mode
		OBS_MODE_DEATHCAM,	// special mode for death cam animation
		OBS_MODE_FREEZECAM,	// zooms to a target, and freeze-frames on them
		OBS_MODE_FIXED,		// view from a fixed camera position
		OBS_MODE_IN_EYE,	// follow a player in first person view
		OBS_MODE_CHASE,		// follow a player in third person view
		OBS_MODE_POI,		// PASSTIME point of interest - game objective, big fight, anything interesting; added in the middle of the enum due to tons of hard-coded "<ROAMING" enum compares
		OBS_MODE_ROAMING,	// free roaming

		NUM_OBSERVER_MODES,
	};

	// DT_LocalPlayerExclusive:
	//	m_nTickBase
	//	m_Local (m_local + DT_Local = offset)

	// DT_Local:
	// m_vecPunchAngle
	// m_vecPunchAngleVel
	// m_flFallVelocity

	// DT_BaseEntity:
	//	m_vecOrigin
	//	m_angRotation
	//	m_iHealth
	//	m_iTeamNum
	//	m_iMaxHealth
	//	m_vecVelocity[0]
	//	m_iParentAttachment (m_iParentAttachment - 2 = m_moveType, m_iParentAttachment + 4 = m_bDormant)

	// DT_BasePlayer:
	//	m_lifeState
	//	m_fFlags
	//	pl (DT_PlayerState, localViewAngles = pl + 0x8 )
	// 

	// DT_BaseCombatCharacter:
	//	m_hActiveWeapon
	//	m_hMyWeapons

	// DT_AnimTimeMustBeFirst
	//	m_flAnimTime (m_flAnimTime - 4 = GetModelInfo())

	// DT_BaseAnimating
	//	m_bClientSideFrameReset (m_bClientSideFrameReset - 0x1C = boneBase (m_CachedBones))

	// DT_BaseFlex
	//	m_vecViewOffset[0]
};



class C_BaseCombatWeapon
{
public:
	CVector& GetBulletSpread() {
		typedef CVector& (__thiscall* fn)(void*);
		return VMT.getvfunc<fn>(this, 325)(this);
	}

	bool IsScripted() {
		// can't find how game checks it, but this is analogue of Weapon:IsScripted() Lua method
		// if it is SWEP than (this + 0x10) stores pointer to SWEP name
		return *(uintptr_t*)((uintptr_t)this + 0x10) != 0;
	}

	const char* GetHoldType() {
		typedef const char* (__thiscall* fn)(void*);
		return VMT.getvfunc<fn>(this, 372)(this);
	}
	const char* GetName() {
		typedef const char* (__thiscall* fn)(void*);
		return VMT.getvfunc<fn>(this, 366)(this);
	}
	const char* GetPrintName() {
		typedef const char* (__thiscall* fn)(void*);
		return VMT.getvfunc<fn>(this, 367)(this);
	}
};

/*
class CScriptedEntity { weapon + 0x18b0 = m_pScriptedEntity
	char pad[16];
	char name[???];

	// if you need more info about this class it can be easily found by ReClass
}
*/

class modelInfo
{
public:
	char pad_0000[4]; //0x0000
	char* m_ModelName; //0x0004
};

