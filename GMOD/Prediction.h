#pragma once
bool g_bIsPredicting = false;

class CPredictionSystem {
public:
	void StartPrediction(CUserCmd*);
	void EndPrediction();
	uint8_t m_flags;
	uint8_t m_moveType;
	float m_flOldCurtime;
	float m_flOldFrametime;
	int m_MoveData[0x400];
	int* m_pPredictionRandomSeed;
	CPed** m_pPredictionPlayer;
	CUserCmd** m_pCurrentCommand;
};


void CPredictionSystem::StartPrediction(CUserCmd* cmd) {
	g_bIsPredicting = true;
	static bool bInit = false;
	if (!bInit) {
		m_pPredictionRandomSeed = *(int**)(SignatureManager.pm_nPredictionRandomSeed + 0x4);
		m_pPredictionPlayer = *(CPed***)(SignatureManager.pm_pPredictionPlayer + 0x5);

		bInit = true;
	}

	m_flags = localPed->GetFlags();
	m_moveType = localPed->GetMoveType();

	typedef void(__cdecl* CPredictableIdHelperResetFn)();
	static CPredictableIdHelperResetFn Reset = (CPredictableIdHelperResetFn)(SignatureManager.fnCPredictableIdHelper_Reset
		+ *(uintptr_t*)(SignatureManager.fnCPredictableIdHelper_Reset + 0x1) + 0x5);

	Reset();

	localPed->GetCurrentCommand() = cmd;
	*m_pPredictionRandomSeed = MD5_PseudoRandom(cmd->m_command_number) & 0x7FFFFFFF;
	*m_pPredictionPlayer = localPed;

	m_flOldCurtime = Interfaces.GlobalVars->curtime;
	m_flOldFrametime = Interfaces.GlobalVars->frametime;

	Interfaces.GlobalVars->curtime = localPed->GetTickBase() * Interfaces.GlobalVars->interval_per_tick;
	Interfaces.GlobalVars->frametime = Interfaces.Prediction->IsEnginePaused() ? 0.0f : Interfaces.GlobalVars->interval_per_tick;
	Interfaces.GameMovement->StartTrackPredictionErrors(localPed);


	localPed->SetScrollSpeed(cmd->m_scroll_wheel_speed);

	static void* pLuaGameMode = **(void***)(SignatureManager.pLuaGameMode + 0x4);


	typedef bool(__thiscall* CLuaGamemodeCallWithArgsFn)(void*, int);
	static CLuaGamemodeCallWithArgsFn CallWithArgs = (CLuaGamemodeCallWithArgsFn)(
		SignatureManager.fnCLuaGamemode_CallWithArgs + *(uintptr_t*)(SignatureManager.fnCLuaGamemode_CallWithArgs + 0x1) + 0x5);

	typedef void(__cdecl* PushEntityFn)(CPed*);
	static PushEntityFn PushEntity = (PushEntityFn)(SignatureManager.fnPushEntity - 0xD);

	typedef void(__thiscall* CLuaGamemodeCallFinishFn)(void*, int);
	static CLuaGamemodeCallFinishFn CallFinish = (CLuaGamemodeCallFinishFn)(
		SignatureManager.fnCLuaGamemode_CallFinish + *(uintptr_t*)(SignatureManager.fnCLuaGamemode_CallFinish + 0x1) + 0x5);



	// iterate m_buttons_pressed
	for (int cmdIterator = 0; cmdIterator < 5; cmdIterator++) {
		auto cmdButton = cmd->m_buttons_pressed[cmdIterator];
		if (!cmdButton) break;
		// try to find current cmdButton in player buttons
		bool findFlag = false;
		for (int playerIterator = 0; playerIterator < 5; playerIterator++) {
			auto playerButton = localPed->GetButtonsArray()[playerIterator];
			if (cmdButton == playerButton) {
				findFlag = true;
			}
		}
		if (!findFlag) { // if not found
			// call lua stuff

			if (pLuaGameMode && CallWithArgs(pLuaGameMode, 78))
			{
				PushEntity(localPed);
				Interfaces.LuaShared->GetLuaInterface(LuaInterfaceType::CLIENT)->PushNumber(cmdButton);
				CallFinish(pLuaGameMode, 2);
			}
		}
	}

	// iterate player buttons
	for (int playerIterator = 0; playerIterator < 5; playerIterator++) {
		auto playerButton = localPed->GetButtonsArray()[playerIterator];
		if (!playerButton) break;
		// try to find current player button in cmd->m_buttons_pressed
		bool findFlag = false;
		for (int cmdIterator = 0; cmdIterator < 5; cmdIterator++) {
			auto cmdButton = cmd->m_buttons_pressed[cmdIterator];
			if (cmdButton == playerButton) {
				findFlag = true;
			}
		}
		if (!findFlag) { // if not found
			// call lua stuff

			if (pLuaGameMode && CallWithArgs(pLuaGameMode, 79))
			{
				PushEntity(localPed);
				Interfaces.LuaShared->GetLuaInterface(LuaInterfaceType::CLIENT)->PushNumber(playerButton);
				CallFinish(pLuaGameMode, 2);
			}
		}
	}

	if (cmd->m_impulse) {
		localPed->GetImpulse() = cmd->m_impulse;
	}

	typedef bool(__thiscall* PhysicsRunThinkFn)(CPed*, int thinkMethod);
	static PhysicsRunThinkFn PhysicsRunThink = (PhysicsRunThinkFn)(
		SignatureManager.fnCBaseEntity_PhysicsRunThink + *(uintptr_t*)(SignatureManager.fnCBaseEntity_PhysicsRunThink + 0x1) + 0x5);

	typedef int(__thiscall* GetNextThinkTickFn)(CPed*, const char* pszContext);
	static GetNextThinkTickFn GetNextThinkTick = (GetNextThinkTickFn)(
		SignatureManager.fnCBaseEntity_GetNextThinkTick + *(uintptr_t*)(SignatureManager.fnCBaseEntity_GetNextThinkTick + 0x1) + 0x5);

	typedef int(__thiscall* SetNextThinkFn)(CPed*, float thinkTime, const char* pszContext);
	static SetNextThinkFn SetNextThink = (SetNextThinkFn)(
		SignatureManager.fnCBaseEntity_SetNextThink + *(uintptr_t*)(SignatureManager.fnCBaseEntity_SetNextThink + 0x1) + 0x5);

	localPed->UpdateButtonState(cmd->m_ibuttons);
	localPed->SetLocalViewAngles(cmd->m_viewangles);

	if (cmd->m_weaponselect) {
		auto weapon = (C_BaseCombatWeapon*)Interfaces.ClientEntityList->GetClientEntity(cmd->m_weaponselect);
		localPed->SelectItem(weapon->GetName(), cmd->m_weaponsubtype);
	}


	if (PhysicsRunThink(localPed, 0)) {
		localPed->PreThink();
	}
	int thinkTick = GetNextThinkTick(localPed, 0);
	if (thinkTick > 0 && thinkTick <= localPed->GetTickBase())
	{
		SetNextThink(localPed, -1.0f, 0);
		localPed->Think();
	}

	int moveData[0x400];
	memset(&moveData, 0, sizeof(moveData));

	Interfaces.Prediction->SetupMove(localPed, cmd, Interfaces.MoveHelper, (CMoveData*)moveData);
	Interfaces.GameMovement->ProcessMovement(localPed, (CMoveData*)moveData);
	Interfaces.Prediction->FinishMove(localPed, cmd, (CMoveData*)moveData);

	typedef void(__thiscall* SetPlayerCollisionBoundFn)(CPed*, int);
	static SetPlayerCollisionBoundFn SetPlayerCollisionBound = (SetPlayerCollisionBoundFn)(SignatureManager.fnCBasePlayer_SetPlayerCollisionBounds);
	SetPlayerCollisionBound(localPed, 1);

	if (localPed->GetFlags() & FL_ONGROUND)
		localPed->GetFallVelocity() = 0.0f;
}

void CPredictionSystem::EndPrediction() {

	Interfaces.GameMovement->FinishTrackPredictionErrors(localPed);
	
	localPed->GetCurrentCommand() = 0;
	*m_pPredictionRandomSeed = -1;
	*m_pPredictionPlayer = 0;

	if (Interfaces.GlobalVars->frametime > 0.0f) {
		localPed->GetTickBase()++;
	}

	Interfaces.GlobalVars->curtime = m_flOldCurtime;
	Interfaces.GlobalVars->frametime = m_flOldFrametime;

	localPed->GetFlags() = m_flags;
	localPed->GetMoveType() = m_moveType;

	g_bIsPredicting = false;
}