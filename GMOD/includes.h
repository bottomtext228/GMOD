#pragma once
#include <Windows.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <Vector>
#include <array>
#include <map>
#include <thread>
#include <mutex>
#include <fstream>
#include <algorithm>
#include <shlobj.h>
#include <filesystem>
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment (lib, "D3dx9.lib")


#include "BTMemory.h"

/*           DirectX Hook           */
#include "kiero/kiero.h"
#include "kiero/minhook/include/MinHook.h"
/*                                  */

/*           Dear ImGui           */
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/InputUtil.h" 
#include "imgui/Hotkey.h" 
/*                                */


#ifdef DEBUG
	void inline debug(const char* msg);
#endif // DEBUG


/*           SDK           */
#include "vector.h"
#include "math_utils.h"
#include "usercmd.h"
#include "CSignatureManager.h"
#include "DTParser.h"
#include "CPlayerResource.h"
#include "ConVar.h"
#include "vmt.h"
#include "CGlobalVars.h"
#include "checksum_md5.h"
#include "bones.h"
#include "CUniformRandomStream.h"
#include "CEntity.h"
CPed* localPed = NULL;
#include "studiobone.h"
#include "CModelInfo.h"
#include "clientrenderable.h"
#include "panelwrapper.h"
#include "trace.h"
#include "engine.h"
#include "cliententlist.h"
#include "icliententitylist.h"
#include "CPrediction.h"
#include "CLua.h"
#include "localization.h"
#include "interfaces.h"
/*                         */


/*           MAIN           */
#include "settings.h"
#include "LuaManager.h"
#include "Utils.h"
#include "Misc.h"
#include "Hooks.h"
#include "Menu.h"
#include "ESP.h"
/*                          */





typedef LRESULT(__stdcall* EndScene)(LPDIRECT3DDEVICE9);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);


