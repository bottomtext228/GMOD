
class IClientRenderable;

class CPed {


public:

	char pad_0000[92]; //0x0000
	class modelInfo* m_modelInfo; //0x005C
	char pad_0060[47]; //0x0060
	uint8_t m_lifeState; //0x008F
	enum lifeStates {
		alive = 0,
		dead = 2,
	};
	uint32_t m_iHealth; //0x0090
	uint32_t m_imaxHealth; //0x0094
	char pad_00A0[4];
	uint32_t m_iTeamNum; //0x009C
	char pad_00A[72];
	CVector m_fViewOffset;
	char pad_00F4[60];
	CVector m_fVelocity; //0x0130
	char pad_013C[60]; //0x013C
	uint32_t m_iMoveType; // 0x178
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
	char pad_0178[228]; // 0x17C
	CVector m_fPos; //0x0260
	float m_fYRot; // 0x026C
	float m_fXRot; //0x0270
	char pad_0274[220];  //0x0350
	union {
		uint32_t flags;
		struct {
			bool ON_GROUND : 1;
			bool DUCKING : 1;
			bool PAD3 : 1;
			bool PAD4 : 1;
			bool PAD5 : 1;
			bool PAD6 : 1;
			bool PAD7 : 1;
			bool PAD8 : 1;
			bool PAD9 : 1;
			bool PAD10 : 1;
			bool IN_WATER : 1;
			// 1 << 10 - in water
		} m_flags;
	};

	char pad_026C[4252]; //0x0354
	uintptr_t boneBase; //0x13E8
	char pad_13EC[4132]; //0x13EC
	uint32_t m_iActiveWeapon; //0x2418
	char pad_2414[188]; //0x2414
	// float m_fFallSpeed; // 0x24C4
	CVector m_fViewPunch; //0x24D8 
	char pad_24DC[48];
	//char pad_24DC[48]; 
	CVector m_fViewPunchVelocity; //0x2514
	// CVector m_fViewAngles (for local player maybe only) // 0x2678

	inline int GetPredictionRandomSeed() {
		return *(int*)((uintptr_t)this + 0x2C2C);
	}

	CVector GetBonePosition(unsigned int boneIndex) {
		CVector bonePos;
		bonePos.x = *reinterpret_cast<float*>(this->boneBase + 0x30 * boneIndex + 0xC);
		bonePos.y = *reinterpret_cast<float*>(this->boneBase + 0x30 * boneIndex + 0x1C);
		bonePos.z = *reinterpret_cast<float*>(this->boneBase + 0x30 * boneIndex + 0x2C);
		return bonePos;
	}
	bool isEntityIsPlayer() {
		typedef bool(__thiscall* fn)(void*);
		return VMT.getvfunc<fn>(this, 130)(this);
	}
	IClientRenderable* GetClientRenderable() {
		typedef IClientRenderable* (__thiscall* fn)(void*);
		return VMT.getvfunc<fn>(this, 5)(this);
	}
	bool inline isAlive() {
		return this->m_lifeState == lifeStates::alive;
	}
	CVector GetEyePosition() {
		return this->m_fPos + this->m_fViewOffset;
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
		typedef float*(__thiscall* fn)(void*, CVector&, float);
		CVector viewDirection;
		constexpr float magicNumber = 0.087155744f;
		VMT.getvfunc<fn>(this, 288)(this, viewDirection, magicNumber); // view angle + aim/view punch;
		return viewDirection;
	}
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
	 	
	const char* GetName() {
		typedef const char* (__thiscall* fn)(void*);
		return VMT.getvfunc<fn>(this, 372)(this);
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

