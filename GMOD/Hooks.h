#define USE_ENGINE_PREDICTION 1
#include "TriggerBot.h"
#include "Aim.h"
#include "NoSpread.h"
#include "Prediction.h"

CTriggerBot* TriggerBot;
CAim* Aim;
CNoSpread* NoSpread;

typedef bool(__thiscall* createMove_t)(void*, float, CUserCmd*);
createMove_t oCreateMove;


typedef void(__thiscall* paintTreverse_t)(void*, unsigned int, bool, bool);
paintTreverse_t oPaintTraverse;


typedef void(__thiscall* paint_t)(void*, int);
paint_t oPaint;


typedef void(__thiscall* runCommand_t)(void*, CPed* player, CUserCmd* ucmd, IMoveHelper* moveHelper);
runCommand_t oRunCommand;


typedef void(__thiscall* postThink_t)(void*);
postThink_t oPostThink;

typedef void(__thiscall* extraMouseSample_t)(void*, float, bool);
extraMouseSample_t oExtraMouseSample;



void __fastcall PaintTraverseFn(void* ecx, void* edx, unsigned int panel, bool forceRepaint, bool allowForce) {

	const char* panelName = Interfaces.PanelWrapper->GetName(panel);

	if (!strcmp(panelName, "FocusOverlayPanel")) {
		LuaManager.ExecuteQueue(); // should do it in game thread or it crashes

		Utils::UpdateLuaNicknames();
	}

	if (vars::misc::customSight && !strcmp(panelName, "HudCrosshair")) // скрытие оригинального прицела
		return;

	oPaintTraverse(ecx, panel, forceRepaint, allowForce);
}


bool __fastcall CreateMoveFn(void* ecx, void* edx, float SampleTime, CUserCmd* cmd) {

	const bool result = oCreateMove(ecx, SampleTime, cmd);

	if (!cmd || !cmd->m_command_number)
		return result;
	if (!localPed || !Interfaces.Engine->IsInGame() || !localPed->IsAlive())
		return result;

	Interfaces.Engine->SetViewAngles(cmd->m_viewangles);

	static CPredictionSystem PredictionSystem;

	if (vars::esp::setupBones)
		Utils::SetupBones(); // вызов вне хука виртуальной функции может привести к спонтанному крашу


	if (vars::misc::autoJump)
		Misc->AutoJump(cmd);



	PredictionSystem.StartPrediction(cmd);

	if (vars::aim::triggerBot || vars::misc::customSight)
		TriggerBot->Process(cmd);

	if (vars::misc::edgeJump)
		Misc->EdgeJump(cmd, PredictionSystem.m_flags);

	if (vars::aim::smoothAim || vars::aim::silentAim)
		Aim->Process(cmd);

	if (vars::aim::noSpread)
		NoSpread->Process(cmd);

	PredictionSystem.EndPrediction();

	if (vars::misc::autoUncuff)
		Misc->AutoUncuff(cmd);


	return false;
}

void __fastcall PaintFn(void* ecx, void* edx, int mode) {
	constexpr int PAINT_INGAMEPANELS = (1 << 1);
	if (mode & PAINT_INGAMEPANELS) {
		float* pMatrix = Interfaces.Engine->WorldToScreenMatrix();
		/* матрица ВНЕ хука (причём даже в хуке PaintTraverse может быть неправильная матрица) на некоторых серверах возращаётся поломанная, поэтому делаем такое */
		memcpy(vars::viewMatrix, pMatrix, 64); // копируем 16 float = 16 * 4 = 64 байт
	}

	oPaint(ecx, mode);
}

void __fastcall RunCommandFn(void* ecx, void* edx, CPed* player, CUserCmd* ucmd, IMoveHelper* moveHelper) {
	if (g_bIsPredicting) return;
	oRunCommand(ecx, player, ucmd, moveHelper);
}

void __fastcall ExtraMouseSampleFn(void* ecx, void* edx, float frametime, bool active) {
	if (vars::aim::smoothAim && vars::aim::aimKeyBind.isDown() && m_shouldSmoothMouse) {

		auto currentAngles = Interfaces.Engine->GetViewAngles();

		if (!vars::aim::stickyAim) {
#if USE_ENGINE_PREDICTION
			CVector localPos = localPed->GetEyePosition();
#else
			CVector localPos = localPed->GetEyePosition() + Utils::predictEntityOffsetPosition(localPed->GetVecVelocity());
#endif // USE_ENGINE_PREDICTION	

			trace_t trace = Interfaces.Trace->TraceRayWrapper(localPos, Utils::GetPointOfSight(localPos, currentAngles), localPed); // trace.hitbox == 0 - head maybe;
			if (trace.ped == m_target)
				return oExtraMouseSample(ecx, frametime, active);
		}

		auto smoothAngles = Aim->calcSmoothAnglesFrame(currentAngles, m_vecWantedVA, Interfaces.GlobalVars->frametime);
		if (vars::aim::ignoreZ) {
			smoothAngles.x = currentAngles.x; // restore old angles if ignoring
		}
		Interfaces.Engine->SetViewAngles(smoothAngles);

	}
	oExtraMouseSample(ecx, frametime, active);
}

class CHooks {

public:
	CVMTHookManager* ClientHook;
	CVMTHookManager* SetViewAngleHook;
	CVMTHookManager* PaintTreverse;
	CVMTHookManager* Paint;
	CVMTHookManager* RunCommand;
	CVMTHookManager* ItemPostFrame;
	CVMTHookManager* ExtraMouseSample;
	void Hook() {
		// 22 ExtraMouseSample CHLClient
		ExtraMouseSample = new CVMTHookManager(Interfaces.Client);
		oExtraMouseSample = (extraMouseSample_t)ExtraMouseSample->HookFunction(22, ExtraMouseSampleFn);
		ExtraMouseSample->HookTable(true);
		/////////////////////////////////////////////////////////////////////////////
		void* ClientShared = **reinterpret_cast<void***>((*reinterpret_cast<uintptr_t**>(Interfaces.Client))[10] + 5);
		ClientHook = new CVMTHookManager(ClientShared);
		oCreateMove = (createMove_t)ClientHook->HookFunction(21, CreateMoveFn);
		ClientHook->HookTable(true);
		/////////////////////////////////////////////////////////////////////////////
		PaintTreverse = new CVMTHookManager(Interfaces.PanelWrapper);
		oPaintTraverse = (paintTreverse_t)PaintTreverse->HookFunction(41, PaintTraverseFn);
		PaintTreverse->HookTable(true);
		/////////////////////////////////////////////////////////////////////////////
		Paint = new CVMTHookManager(Interfaces.EngineVGUI);
		oPaint = (paint_t)Paint->HookFunction(13, PaintFn);
		Paint->HookTable(true);
		/////////////////////////////////////////////////////////////////////////////
#if USE_ENGINE_PREDICTION
		RunCommand = new CVMTHookManager(Interfaces.Prediction);
		oRunCommand = (runCommand_t)RunCommand->HookFunction(17, RunCommandFn);
		RunCommand->HookTable(true);
#endif
	}
	void UnHook() {
		ExtraMouseSample->~CVMTHookManager();
		ClientHook->~CVMTHookManager();
		PaintTreverse->~CVMTHookManager();
		Paint->~CVMTHookManager();
#if USE_ENGINE_PREDICTION
		RunCommand->~CVMTHookManager();
#endif USE_ENGINE_PREDICTION
	}

};
CHooks Hooks;