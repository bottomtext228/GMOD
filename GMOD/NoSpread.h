typedef CVector& (__thiscall* CShotManipulator_ApplySpread)(float*, CVector&, float); // Manipulator.ApplySpread
typedef int(__cdecl* CShotManipulator_constructor)(float* a1, float* a2, float* a3); // CShotManipulator Manipulator(info.m_vecDirShooting)

class CNoSpread {
public:
	void Process(CUserCmd* cmd) {
		if (!(cmd->m_buttons.IN_ATTACK))
			return;

		C_BaseCombatWeapon* weapon = (C_BaseCombatWeapon*)Interfaces.ClientEntityList->GetClientEntityFromHandle(localPed->m_iActiveWeapon);
		if (!weapon)
			return;


		if (weapon->IsScripted()) // will not work for Lua weapons
			return;


		const char* weaponName = weapon->GetName();
		if (strcmp(weaponName, "smg") && strcmp(weaponName, "pistol") && strcmp(weaponName, "ar2")) // only work for this HL2 weapons
			return;

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

		BYTE seed = MD5_PseudoRandom(cmd->m_cmd_nr) & 0xFF;

		Interfaces.UniformRandomStream->SetSeed(seed);


		static auto fnShotManipulator_ApplySpread = (CShotManipulator_ApplySpread)SignatureManager.fnShotManipulator_ApplySpread;
		static auto fnShotManipulator_constructor = (CShotManipulator_constructor)SignatureManager.fnShotManipulator_constructor;

		CVector shootAngles = cmd->m_viewangles;

		if (strcmp(weapon->GetName(), "pistol")) // pistol resets view punch in PrimaryAttack(), so we don't set view punch when using it
			shootAngles += localPed->m_fViewPunch;


		CVector shootDir = shootAngles.toDirection();


		float shotInfo[12] = {
			shootDir.x, shootDir.y, shootDir.z, // vShootDirection
			0.0f, 0.0f, 0.0f, // vRight
			0.0f, 0.0f, 0.0f, // vUp 
			0.0f, 0.0f, 0.0f // vResult
		};

		fnShotManipulator_constructor((float*)&shotInfo, (float*)&shotInfo[3], (float*)&shotInfo[6]); // see CShotManipulator in source engine for more info


		CVector bulletDirection = fnShotManipulator_ApplySpread((float*)&shotInfo, weapon->GetBulletSpread(), 1.0f);

		CVector bulletAngles = bulletDirection.toAngle();
		CVector difference = cmd->m_viewangles - bulletAngles;


		cmd->m_viewangles += difference;
		vars::aim::newAngles = cmd->m_viewangles;
	}

};

