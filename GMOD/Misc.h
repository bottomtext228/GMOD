class CMisc {
public:
	void AutoJump(CUserCmd* cmd) {
		if (cmd->m_buttons.IN_JUMP && localPed->GetMoveType() != localPed->MOVETYPE_NOCLIP && localPed->GetMoveType() != localPed->MOVETYPE_LADDER) {
			if (!(localPed->GetFlags() & FL_ONGROUND) && !(localPed->GetFlags() & FL_INWATER))
				cmd->m_buttons.IN_JUMP = false;
		}
	}

	void EdgeJump(CUserCmd* cmd, uint8_t oldFlags) {
		if (vars::misc::edgeJumpKeyBind.isDown() &&
			localPed->GetMoveType() != localPed->MOVETYPE_NOCLIP && localPed->GetMoveType() != localPed->MOVETYPE_LADDER &&
			!cmd->m_buttons.IN_JUMP && (oldFlags & FL_ONGROUND) && !(localPed->GetFlags() & FL_ONGROUND)) {
			cmd->m_buttons.IN_JUMP = true;
		}
	}

	void CustomSight() {
		ImDrawList* draw = ImGui::GetBackgroundDrawList();
		ImVec2 Pos2D((float)vars::resX / 2.0f, (float)vars::resY / 2.0f);
		draw->AddCircle(Pos2D, 3, vars::misc::sightColor, 20, 1.0f);
		draw->AddCircleFilled(Pos2D, 1.0, vars::misc::sightColor, 20);
	}

	void AutoUncuff(CUserCmd* cmd) {
		static bool isInAttack = false;
		cmd->m_buttons.IN_ATTACK2 = !isInAttack;
		isInAttack = !isInAttack;
	}

	void Unload(LPDIRECT3DDEVICE9 pDevice, HWND window, WNDPROC oWndProc) {
		/* Всё ниже обязательно для нормальной выгрузки */
		kiero::shutdown();
		pDevice->Release();
		BTMemory::UnpatchAll();
		SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)(oWndProc));
	}

	void CamHack(CUserCmd* cmd) {
		cmd->m_forwardmove = 0;
		cmd->m_sidemove = 0;
		cmd->m_upmove = 0;
		cmd->m_ibuttons = 0;
		cmd->m_impulse = 0;
		cmd->m_weaponselect = 0;
		cmd->m_viewangles = g_lastViewAngles;
	}

	void CamHackOverrideView(CViewSetup* pSetup) {
		CVector& angles = pSetup->angles;
		float speed = Interfaces.GlobalVars->frametime * 1500;

		CVector offset;

		if (g_userCmd.m_forwardmove > 0) { // forward
			offset += angles.Forward();
		}

		if (g_userCmd.m_forwardmove < 0) { // backward
			offset -= angles.Forward();
		}

		if (g_userCmd.m_sidemove < 0) { // left
			offset -= angles.Right();
		}

		if (g_userCmd.m_sidemove > 0) { // right
			offset += angles.Right();
		}

		if (g_userCmd.m_buttons.IN_JUMP) { // up
			offset += angles.Up();
		}

		if (g_userCmd.m_buttons.IN_SPEED) { // shift key
			speed *= 3;
		}

		if (g_userCmd.m_buttons.IN_DUCK) { // ctrl key
			speed /= 3;
		}

		offset.Normalize();

		g_lastViewPosition += offset * speed;
		pSetup->origin = g_lastViewPosition;
	}

}; CMisc* Misc;

