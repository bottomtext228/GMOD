typedef CVector& (__thiscall* CShotManipulator_ApplySpread)(float*, CVector&, float); // Manipulator.ApplySpread
typedef int(__cdecl* CShotManipulator_constructor)(float* a1, float* a2, float* a3); // CShotManipulator Manipulator(info.m_vecDirShooting)

class CNoSpread {
public:
	void Process(CUserCmd* cmd) {
		if (!(cmd->m_buttons.IN_ATTACK))
			return;

		C_BaseCombatWeapon* weapon = (C_BaseCombatWeapon*)Interfaces.ClientEntityList->GetClientEntityFromHandle(localPed->GetActiveWeaponHandle());
		if (!weapon)
			return;

		const char* weaponName = weapon->GetName();

		// if it's a HL2 weapon, check if no spread is needed
		if (!weapon->UsesLua()) {
			if (strcmp(weaponName, "weapon_smg1")
				&& strcmp(weaponName, "weapon_pistol")
				&& strcmp(weaponName, "weapon_ar2")
				&& strcmp(weaponName, "weapon_357")) // only work for this HL2 weapons
				return;
		}

		auto Lua = Interfaces.LuaShared->GetLuaInterface(LuaInterfaceType::CLIENT);

		// TODO: 
		// * check other gun packs

		float spread = 0.0f;

		// if it is a Lua weapon
		if (weapon->IsScripted()) {

			// if it is a M9K weapon
			if (!strcmp(Utils::GetLuaBase(weapon), "bobs_gun_base") || !strcmp(Utils::GetLuaBase(weapon), "bobs_scoped_base")) {

				spread = Utils::GetM9KSpread(weapon, cmd->m_buttons.IN_ATTACK2);

				Utils::M9KNoRecoil(weapon);
			}

			// if it is a TFA weapon
			//if (!strcmp(Utils::GetLuaEntBase(weapon), "tfa_ins2_base"))
			//{
			//	spread = Utils::GetTFASpread(weapon);
			//}
		}


		/*
		* How NoSpread works?
		* We set prediction seed the same as the server and it allows us to get same random values.
		* Basically, we are doing the same thing as the game to get bullet direction and,
		* then we convert it to angles and calculate the difference between our view angles and bullet angles.
		* At the end we just add this difference to our view angles to compensate for bullet spread.
		*/

		/*
		*	Reversed functions from Source SDK
		*	CHLMachineGun::PrimaryAttack(processShotCaller2)
		*	CHLMachineGun::FireBullets(processShotCaller1)
		*	CHL2MP_Player::FireBullets(processShotCaller)
		*	CBaseEntity::FireBullets(processShot)
		*/


		BYTE seed = MD5_PseudoRandom(cmd->m_command_number) & 0xFF;

		Interfaces.UniformRandomStream->SetSeed(seed);


		static auto fnShotManipulator_ApplySpread = (CShotManipulator_ApplySpread)SignatureManager.fnShotManipulator_ApplySpread;
		static auto fnShotManipulator_constructor = (CShotManipulator_constructor)(SignatureManager.fnShotManipulator_constructor);

		CVector shootAngles = cmd->m_viewangles;

		// pistol resets view punch in PrimaryAttack(), so we don't set view punch when using it
		// also lua weapon seems to not use view punch too
		if (strcmp(weaponName, "weapon_pistol") && !weapon->UsesLua()) { // if not pistol or not lua weapon
			// ViewPunch are angles that are added to our shoot angles to simulate weapon recoil, so we should add them.
			// localPed->GetVecPunchAngle() here differs from ViewPunch when we are actually shooting, 
			// so we should predict its future value by multiplying ViewPunchVelocity on tick interval.		
#if USE_ENGINE_PREDICTION
			shootAngles += localPed->GetVecPunchAngle();
#else
			shootAngles += localPed->GetVecPunchAngle() + localPed->GetVecPunchAngleVel() * Interfaces.GlobalVars->interval_per_tick;
#endif // USE_ENGINE_PREDICTION		
		}
		CVector shootDir = shootAngles.Forward();


		// it is actually 4 Vectors
		float shotInfo[12] = {
			shootDir.x, shootDir.y, shootDir.z, // vShootDirection
			0.0f, 0.0f, 0.0f, // vRight
			0.0f, 0.0f, 0.0f, // vUp 
			0.0f, 0.0f, 0.0f // vResult	
		};

		// see CShotManipulator in source engine for more info
		fnShotManipulator_constructor((float*)&shotInfo, (float*)&shotInfo[3], (float*)&shotInfo[6]);

		CVector weaponSpread{ spread, spread, 0.0f };

		// revolver uses (0.0, 0.0, 0.0) spread
		// Lua weapons use spread that is stored in Lua
		if (strcmp(weaponName, "weapon_357") && !weapon->UsesLua())
			weaponSpread = weapon->GetBulletSpread(); // other default HL2 weapons use this

		CVector bulletDirection = fnShotManipulator_ApplySpread((float*)&shotInfo, weaponSpread, 1.0f);

		CVector bulletAngles = bulletDirection.ToAngle();
		CVector difference = cmd->m_viewangles - bulletAngles;

		cmd->m_viewangles += difference;
	}
};


// This is NoSpread realisation via rebuild fnShotManipulator_ApplySpread function
/*
BYTE seed = MD5_PseudoRandom(cmd->m_command_number) & 0xFF;
Interfaces.UniformRandomStream->SetSeed(seed);

float x, y, z;

float bias = 1.0f;
float shotBiasMin = ConVarManager.GetConVar("ai_shot_bias_min")->m_fFloatValue;
float shotBiasMax = ConVarManager.GetConVar("ai_shot_bias_max")->m_fFloatValue;

float shotBias = ((shotBiasMax - shotBiasMin) * bias) + shotBiasMin;

float flatness = (fabsf(shotBias) * 0.5);

do {
	auto pRandom = Interfaces.UniformRandomStream;
	x = pRandom->RandomFloat(-1, 1) * flatness + pRandom->RandomFloat(-1, 1) * (1 - flatness);
	y = pRandom->RandomFloat(-1, 1) * flatness + pRandom->RandomFloat(-1, 1) * (1 - flatness);

	z = x * x + y * y;
} while (z > 1);

float spread = weapon->GetBulletSpread().x;
auto angles = cmd->m_viewangles;

auto cd = angles.Forward() + (angles.Right() * spread * x * (-1)) + (angles.Up() * y * spread * (-1));

CVector bulletAngles = cd.toAngle();

cmd->m_viewangles = bulletAngles - localPed->GetVecPunchAngle();
*/