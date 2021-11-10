#define _CRT_SECURE_NO_WARNINGS
#include "includes.h"

#ifdef _WIN64
#define GWL_WNDPROC GWLP_WNDPROC
#endif

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
float* getMatrix();

EndScene oEndScene = NULL;
WNDPROC oWndProc;
static HWND window = NULL;


struct CVector
{
	float x, y, z;

	CVector(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	CVector() { x = y = z = 0; }
};

class CVector4D {
public:
	float x, y, z, w;
};

class CVector2D {
public:
	int x;
	int y;
};
class CPed {

public:
	char pad_0000[143]; //0x0000
	uint8_t m_lifeState; // 0x008F
	uint32_t m_iHealth; //0x0090
	uint32_t m_imaxHealth; //0x0094
	char pad_00A0[4];
	uint32_t m_iTeamNum; //0x009C
	char pad_00A[144]; //0x00A0
	CVector m_fVelocity; //0x0130
	char pad_013C[292]; //0x013C
	CVector m_fPos; //0x0260
	char pad_026C[484]; //0x026C

	//char pad_0238[4572]; //0x0238
}; //Size: 0x1414




bool WorldToScreen(CVector pos, CVector2D& screen, float matrix[16])
{
	//Matrix-vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords
	CVector4D clipCoords;
	clipCoords.x = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
	clipCoords.y = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];
	clipCoords.z = pos.x * matrix[8] + pos.y * matrix[9] + pos.z * matrix[10] + matrix[11];
	clipCoords.w = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];

	if (clipCoords.w < 0.1f)
		return false;

	//perspective division, dividing by clip.W = Normalized Device Coordinates
	CVector NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	screen.x = (vars::resX / 2 * NDC.x) + (NDC.x + vars::resX / 2);
	screen.y = -(vars::resY / 2 * NDC.y) + (NDC.y + vars::resY / 2);
	return true;
}

uintptr_t getEntityPointerByIndex(const unsigned int entityIndex) {
	return *reinterpret_cast<uintptr_t*>(vars::client + offsets::entityList + 0x10 * entityIndex);
}


char* getPlayerNameByIndex(const unsigned int playerIndex) {
	DWORD playerStruct = getEntityPointerByIndex(playerIndex);
	playerStruct = *reinterpret_cast<DWORD*>(playerStruct + 0x14);
	playerStruct = *reinterpret_cast<DWORD*>(playerStruct + 0x8);
	return reinterpret_cast<char*>(playerStruct + 0xA4);
}

ImU32 calcESPColor(const unsigned int health) {
	if (health >= 100)
		return ImGui::ColorConvertFloat4ToU32(ImVec4(0.0, 1.0, 0.0, 1.0));
	float clrG = 255.0 * (health / 100.0);
	float clrR = 255.0 * ((100 - health) / 100.0);
	ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(clrR / 255.0, clrG / 255.0, 0, 1));
	return color;
}

void InitImGui(LPDIRECT3DDEVICE9 pDevice)
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 14, 0, io.Fonts->GetGlyphRangesCyrillic());
	io.Fonts->Build();
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(pDevice);
}


void DrawBoxESP(LPDIRECT3DDEVICE9 m_pDevice, CVector pos, unsigned int health) {
	CVector2D screenPos[8];	
	CVector corners[8];
	corners[0] = { pos.x - vars::boxESPSize, pos.y - vars::boxESPSize, pos.z };
	corners[1] = { pos.x + vars::boxESPSize, pos.y - vars::boxESPSize, pos.z };
	corners[2] = { pos.x - vars::boxESPSize, pos.y + vars::boxESPSize, pos.z };
	corners[3] = { pos.x + vars::boxESPSize, pos.y + vars::boxESPSize, pos.z };
	corners[4] = { pos.x - vars::boxESPSize, pos.y - vars::boxESPSize, pos.z + 70.0f};
	corners[5] = { pos.x + vars::boxESPSize, pos.y - vars::boxESPSize, pos.z + 70.0f};
	corners[6] = { pos.x - vars::boxESPSize, pos.y + vars::boxESPSize, pos.z + 70.0f };
	corners[7] = { pos.x + vars::boxESPSize, pos.y + vars::boxESPSize, pos.z + 70.0f };
	bool isAllPlayerInOurView = true;
	for (unsigned int i = 0; i < 8; i++) {
		if (!WorldToScreen(corners[i], screenPos[i], vars::matrix)) {
			isAllPlayerInOurView = false;
			break;
		}
	}
	if (!isAllPlayerInOurView)
		return;
	ImDrawList* draw = ImGui::GetBackgroundDrawList();
	ImU32 ESPColor = calcESPColor(health);
	draw->AddLine(ImVec2(screenPos[0].x, screenPos[0].y), ImVec2(screenPos[1].x, screenPos[1].y), ESPColor);
	draw->AddLine(ImVec2(screenPos[0].x, screenPos[0].y), ImVec2(screenPos[2].x, screenPos[2].y), ESPColor);
	draw->AddLine(ImVec2(screenPos[0].x, screenPos[0].y), ImVec2(screenPos[4].x, screenPos[4].y), ESPColor);
	draw->AddLine(ImVec2(screenPos[1].x, screenPos[1].y), ImVec2(screenPos[3].x, screenPos[3].y), ESPColor);
	draw->AddLine(ImVec2(screenPos[1].x, screenPos[1].y), ImVec2(screenPos[5].x, screenPos[5].y), ESPColor);
	draw->AddLine(ImVec2(screenPos[2].x, screenPos[2].y), ImVec2(screenPos[3].x, screenPos[3].y), ESPColor);
	draw->AddLine(ImVec2(screenPos[2].x, screenPos[2].y), ImVec2(screenPos[6].x, screenPos[6].y), ESPColor);
	draw->AddLine(ImVec2(screenPos[3].x, screenPos[3].y), ImVec2(screenPos[7].x, screenPos[7].y), ESPColor);
	draw->AddLine(ImVec2(screenPos[4].x, screenPos[4].y), ImVec2(screenPos[5].x, screenPos[5].y), ESPColor);
	draw->AddLine(ImVec2(screenPos[4].x, screenPos[4].y), ImVec2(screenPos[6].x, screenPos[6].y), ESPColor);
	draw->AddLine(ImVec2(screenPos[4].x, screenPos[4].y), ImVec2(screenPos[5].x, screenPos[5].y), ESPColor);
	draw->AddLine(ImVec2(screenPos[5].x, screenPos[5].y), ImVec2(screenPos[7].x, screenPos[7].y), ESPColor);
	draw->AddLine(ImVec2(screenPos[6].x, screenPos[6].y), ImVec2(screenPos[7].x, screenPos[7].y), ESPColor);

}






inline float* getMatrix() {
	return reinterpret_cast<float*>(*reinterpret_cast<uintptr_t*>(vars::engine + offsets::viewMatrix) + offsets::viewMatrixOffset);
}


CPed* getLocalPlayer() {
	return reinterpret_cast<CPed*>(*reinterpret_cast<uintptr_t*>(vars::client + offsets::localPlayer));
}
long __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice)
{
	if (!vars::init)
	{
		InitImGui(pDevice);
		ImGuiStyle& style = ImGui::GetStyle();
		style.FrameRounding = 3;
		style.WindowRounding = 7;
		vars::init = true;
	
	
	}

	
		ImGui_ImplDX9_NewFrame();
	
		ImGui_ImplWin32_NewFrame();
	
		ImGui::NewFrame();

		if (vars::wallHackState) {
			CPed* localPed = (CPed*)getLocalPlayer();
			if (localPed) {
				vars::matrix = getMatrix(); // на всякий случай

				for (unsigned int i = 0; i < 256; i++) {
					CPed* ped = (CPed*)getEntityPointerByIndex(i);
					if (ped && ped->m_lifeState == lifeStates::alive && ped->m_iHealth > 0 && ped->m_iTeamNum != 0 && ped != localPed) { // lifeStatus: 0 - alive, 2 - dead
						//ImGui::Text("Player %d NICK: %s state: %d, team: %d, health is: %d\n His position is: X: %f, Y: %f, Z: %f\n", i, getPlayerNameByIndex(i), ped->m_lifeState,ped->m_iTeamNum, ped->m_iHealth, ped->m_fPos.x, ped->m_fPos.y, ped->m_fPos.z);

						ImDrawList* draw = ImGui::GetBackgroundDrawList();
						CVector pos = ped->m_fPos;
						pos.z += 80;
						CVector2D screenPos;
						if (vars::ESPState) {
							DrawBoxESP(pDevice, ped->m_fPos, ped->m_iHealth);
							//ImGui::Text("TESTDAD %s", getPlayerNameByIndex(i));
							if (vars::renderHealthAndNick) {
								if (WorldToScreen(pos, screenPos, vars::matrix)) {



									char espText[64];
									sprintf_s(espText, "[%d] %s", ped->m_iHealth, getPlayerNameByIndex(i));
									ImVec2 textSize = ImGui::CalcTextSize(espText);
									draw->AddText(ImVec2(screenPos.x - textSize.x / 2, screenPos.y - textSize.y), -1, espText);
									//ImGui::Text("ESP WORKING AT %s, SCREENPOS: %d, %d", getPlayerNameByIndex(i), screenPos.x, screenPos.y);
								}


							}


						}
						if (vars::linesState) {
							if (WorldToScreen(pos, screenPos, vars::matrix)) {

								draw->AddLine(ImVec2(vars::resX / 2, 0), ImVec2(screenPos.x, screenPos.y), calcESPColor(ped->m_iHealth));
							}
						}
					}
				/*	if (vars::renderEntity) {
						if (ped && ped->m_iTeamNum == 0) {
							CVector2D entityPos;
							if (WorldToScreen(ped->m_fPos, entityPos, vars::matrix)) {
								ImDrawList* draw = ImGui::GetBackgroundDrawList();
								draw->AddLine(ImVec2(vars::resX / 2, vars::resY), ImVec2(entityPos.x, entityPos.y), -1);
							}
						}
					}
				*/
				/*	if (vars::debug) {
						CVector ourPos = localPed->m_fPos;
						ourPos.x += 100;
						CVector2D debugPos;
						bool w2s = WorldToScreen(ourPos, debugPos, vars::matrix);
						ImDrawList* draw = ImGui::GetBackgroundDrawList();
						draw->AddCircle(ImVec2(debugPos.x, debugPos.y), 10, -1);
						ImGui::Text("ResX %d, ResY ", vars::resX, vars::resY);
					}
				*/
				}
			
			}
		
			

		}

		if (vars::menu) {
			ImGui::SetNextWindowPos(ImVec2(vars::resX / 2, vars::resY / 2), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(200.0, 180.0), ImGuiCond_FirstUseEver);
			ImGui::Begin("GMOD Cheat", &vars::menu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);
			ImGui::Checkbox("Enable WallHack", &vars::wallHackState);
			ImGui::Separator();
			ImGui::Checkbox("Lines", &vars::linesState);
			ImGui::Checkbox("BoxESP", &vars::ESPState);
			ImGui::Checkbox("Health and nicks", &vars::renderHealthAndNick);
		//	ImGui::Checkbox("render Entity", &vars::renderEntity);
		//	ImGui::Checkbox("DEBUG", &vars::debug);
			ImGui::SetCursorPosY(150);
			ImGui::Text("By bottom_text");
			if (ImGui::IsItemClicked()) {
				system("start https://www.blast.hk/members/217146/");
			}
			ImGui::End();

		

		}
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	
	return oEndScene(pDevice);
}
#define menuKey VK_END

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (wParam == menuKey) {
		BOOL repeatFlag = (HIWORD(lParam) & KF_REPEAT) == KF_REPEAT;
		if (uMsg == WM_KEYDOWN && !repeatFlag){
			vars::menu = !vars::menu;
		}
		return 0;
	}
	
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse && (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP || uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP || uMsg == WM_MOUSEWHEEL || uMsg == WM_MOUSEMOVE))
	{
		return TRUE;
	}

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}


DWORD WINAPI MainThread(LPVOID lpReserved)
{
	bool attached = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D9) == kiero::Status::Success)
		{
			kiero::bind(42, (void**)& oEndScene, hkEndScene);
			window = FindWindowA(0, "Garry's Mod");
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)WndProc);
			attached = true;
		}
	} while (!attached);
	
	return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);
	

		//MessageBox(0, "Dll injected!", "DLL", 0);
		vars::client = (DWORD)GetModuleHandle("client.dll");
		vars::engine = (DWORD)GetModuleHandle("engine.dll");
		vars::resX = GetSystemMetrics(0);
		vars::resY = GetSystemMetrics(1);
	/*	if (AllocConsole())
		{
			(void)freopen("CONIN$", "r", stdin);
			(void)freopen("CONOUT$", "w", stdout);
			(void)freopen("CONOUT$", "w", stderr);
			
			printf("LOADED! wadad %f\n", *reinterpret_cast<float*>(*reinterpret_cast<DWORD*>(vars::engine + offsets::viewMatrix) + offsets::viewMatrixOffset));
		} */
		CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);

		break;
	case DLL_PROCESS_DETACH:
	//	FreeConsole();
		kiero::shutdown();
		break;
	}
	return TRUE;
}
