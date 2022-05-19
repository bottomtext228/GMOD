
class CNoSpread {
public:
	void Process(CUserCmd* CMD){	
		if (!(CMD->m_buttons.IN_ATTACK))
			return;

		C_BaseCombatWeapon* weapon = (C_BaseCombatWeapon*)Interfaces.ClientEntityList->GetClientEntityFromHandle(localPed->m_iActiveWeapon);
		if (!weapon)
			return;
		CVector gunSpread = weapon->GetBulletSpread();

		float spread = (gunSpread.x + gunSpread.y + gunSpread.z) / 3;
		BYTE seed = MD5_PseudoRandom(CMD->m_cmd_nr) & 0xFF;
		Interfaces.UniformRandomStream->SetSeed(seed);

		CVector engineSpread(Interfaces.UniformRandomStream->RandomFloat(-0.5f, 0.5f) + Interfaces.UniformRandomStream->RandomFloat(-0.5f, 0.5f),
			Interfaces.UniformRandomStream->RandomFloat(-0.5f, 0.5f) + Interfaces.UniformRandomStream->RandomFloat(-0.5f, 0.5f), 0);

		CVector ShootDirection = CVector(1.f, 1.f, 1.f);
		ShootDirection.y = (spread * engineSpread.x);
		ShootDirection.z = -(spread * engineSpread.y);

		CMD->m_viewangles += ShootDirection.toAngle();
		vars::aim::newAngles = CMD->m_viewangles;
	}
};