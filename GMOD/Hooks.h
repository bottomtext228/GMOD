#include "TriggerBot.h"
#include "Aim.h"
#include "NoSpread.h"
#include "BunnyHop.h"

CTriggerBot* TriggerBot;
CAim* Aim;
CNoSpread* NoSpread;
CBunnyHop* BunnyHop;

typedef bool(__thiscall* createMove_t)(void*, float, CUserCmd*);
createMove_t oCreateMove;


typedef void(__thiscall* setViewAngle_t)(void*, CVector&);
setViewAngle_t oSetViewAngle;


typedef void(__thiscall* paintTreverse_t)(void*, unsigned int, bool, bool);
paintTreverse_t oPaintTraverse;


typedef void(__thiscall* paint_t)(void*, int);
paint_t oPaint;


void __fastcall PaintTraverseFn(void* ecx, void* edx, unsigned int panel, bool forceRepaint, bool allowForce) {

	const char* panelName = Interfaces.PanelWrapper->GetName(panel);
	if (vars::misc::customSight && !strcmp(panelName, "HudCrosshair")) // скрытие оригинального прицела
		return;

	oPaintTraverse(ecx, panel, forceRepaint, allowForce);
}

bool __fastcall CreateMoveFn(void* ecx, void* edx, float SampleTime, CUserCmd* cmd) {
	
	if (!cmd->m_cmd_nr)
		return true;
	if (!localPed || !Interfaces.Engine->IsInGame())
		return true;
	

	if (vars::esp::setupBones)
		Misc->SetupBones(); // вызов вне хука виртуальной функции может привести к спонтанному крашу

	if (vars::aim::triggerBot || vars::misc::customSight)
		TriggerBot->Process(cmd);
	if (vars::aim::smoothAim || vars::aim::silentAim)
		Aim->Process(cmd);
	if (vars::aim::noSpread)
		NoSpread->Process(cmd);
	if (vars::bhop::autoJump) 	// TODO: попытаться сделать бхоп полноценный
		BunnyHop->Process(cmd);
	if (vars::misc::autoUncuff)
		Misc->AutoUncuff(cmd);
	


	return true;
}

void __fastcall SetViewAngleFn(void* ecx, void* edx, CVector& angle) { // для silent Aim, чтобы локально персонаж никак не поворачивался
	if ((vars::aim::silentAim || vars::aim::noSpread) && vars::aim::newAngles == angle) // vars::newAngles != angle - фикс замедления поворота персонажа при использовании сайлента
		return;
	oSetViewAngle(ecx, angle);
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
class CHooks {

public:
	CVMTHookManager* ClientHook;
	CVMTHookManager* SetViewAngleHook;
	CVMTHookManager* PaintTreverse;
	CVMTHookManager* Paint;
	void Hook() {
	
	
		void* ClientShared = **reinterpret_cast<void***>((*reinterpret_cast<uintptr_t**>(Interfaces.Client))[10] + 5);


		ClientHook = new CVMTHookManager(ClientShared);
		oCreateMove = (createMove_t)ClientHook->HookFunction(21, CreateMoveFn);
		ClientHook->HookTable(true);
		/////////////////////////////////////////////////////////////////////////////
		SetViewAngleHook = new CVMTHookManager(Interfaces.Engine);
		oSetViewAngle = (setViewAngle_t)SetViewAngleHook->HookFunction(20, SetViewAngleFn);
		SetViewAngleHook->HookTable(true);
		/////////////////////////////////////////////////////////////////////////////
		PaintTreverse = new CVMTHookManager(Interfaces.PanelWrapper);
		oPaintTraverse = (paintTreverse_t)PaintTreverse->HookFunction(41, PaintTraverseFn);
		PaintTreverse->HookTable(true);	
		/////////////////////////////////////////////////////////////////////////////
		Paint = new CVMTHookManager(Interfaces.EngineVGUI);
		oPaint = (paint_t)Paint->HookFunction(13, PaintFn);
		Paint->HookTable(true);
	
	}
	void UnHook() {
		ClientHook->~CVMTHookManager();
		SetViewAngleHook->~CVMTHookManager();
		PaintTreverse->~CVMTHookManager();
		Paint->~CVMTHookManager();
	}

};
CHooks Hooks;