#define CHEAT_VERSION 2.3
#define _CRT_SECURE_NO_WARNINGS
//#define DEBUG
#include "includes.h"


extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#ifdef  DEBUG
std::ofstream debugFile;

void inline debug(const char* msg) {
	//debugFile << msg;
	//	printf(msg);
}
#endif //  DEBUG



EndScene oEndScene = NULL;
WNDPROC oWndProc;
HWND window = NULL;




LRESULT WINAPI hkEndScene(LPDIRECT3DDEVICE9 pDevice) {

	if (!vars::init && !Menu->Init(pDevice, window))
		return oEndScene(pDevice);

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	if (Interfaces.Engine->IsInGame()) {
		localPed = Interfaces.ClientEntityList->GetClientEntity(Interfaces.Engine->GetLocalPlayer());
		ESP->Process();	
	}
	Menu->Render();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	if (vars::misc::DLLUnload) {
		Misc->Unload(pDevice, window, oWndProc);
	}
	return oEndScene(pDevice);
}


#define menuKey VK_END

LRESULT WINAPI WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	/*
	* this fix IM_ASSERT(GImGui != NULL && "No current context. Did you call ImGui::CreateContext() and ImGui::SetCurrentContext() ?");
	*/
	if (!ImGui::GetCurrentContext())
		return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);


	if (wParam == menuKey) {
		BOOL repeatFlag = (HIWORD(lParam) & KF_REPEAT) == KF_REPEAT;
		if (uMsg == WM_KEYDOWN && !repeatFlag) {
			vars::menu = !vars::menu;

		}
		return 0; // блокируем все взаимодействия с клавишей активации чтобы не спалится
	}

	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;


	ImGuiIO& io = ImGui::GetIO();

	if (io.WantCaptureKeyboard) {
		return true; // we don't want to game know about key input when writing in ImGui::Input() 
	}

	if (io.WantCaptureMouse && (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP
		|| uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP || uMsg == WM_MOUSEWHEEL || uMsg == WM_MOUSEMOVE)) {
		return true; // чтобы при нажатии на меню мышкой игра не видела нажатия    
	}



	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}


DWORD WINAPI MainThread(HMODULE hMod)
{
	bool attached = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D9) == kiero::Status::Success)
		{
			kiero::bind(42, (void**)&oEndScene, hkEndScene);
			window = FindWindowA(0, "Garry's Mod");
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)WndProc);
			attached = true;
		}
	} while (!attached);
	

	// TODO:
	// * maybe make aim distance check
	// * try to get proper hostname
	while (!vars::misc::DLLUnload) {
		Sleep(100);
	}
	Sleep(1000);
	FreeLibraryAndExitThread(hMod, 0);

	return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);

		vars::client = (DWORD)GetModuleHandle("client.dll");
		vars::engine = (DWORD)GetModuleHandle("engine.dll");
		vars::resX = GetSystemMetrics(0);
		vars::resY = GetSystemMetrics(1);
		vars::bSendPacket = (bool*)(SignatureManager.pbSendPacket + 0x3);
		DWORD oldProtection;
		VirtualProtect(vars::bSendPacket, sizeof(bool), PAGE_EXECUTE_READWRITE, &oldProtection);
		
#ifdef DEBUG
		if (AllocConsole())
		{
			(void)freopen("CONIN$", "r", stdin);
			(void)freopen("CONOUT$", "w", stdout);
			(void)freopen("CONOUT$", "w", stderr);
			printf("INJECTED!\n");

		}
#endif // DEBUG


#ifdef DEBUG
		remove("debug.txt");
		debugFile.open("debug.txt", std::ios::app);
#endif // DEBUG


		Interfaces.GetInterfaces();
		//MH_Initialize();
		Hooks.Hook();
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hMod, 0, nullptr);

		break;
	case DLL_PROCESS_DETACH:
#ifdef DEBUG
		FreeConsole();
#endif // DEBUG


		Hooks.UnHook();
		ImGui::DestroyContext();
		kiero::shutdown();
#ifdef DEBUG
		debugFile.close();
#endif // DEBUG


		break;
	}
	return TRUE;
}

