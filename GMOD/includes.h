#pragma once
#include <Windows.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <Vector>
#include <array>
#include <map>
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment (lib, "D3dx9.lib")

/*           DirectX Hook           */
#include "kiero/kiero.h"
#include "kiero/minhook/include/MinHook.h"
/*                                  */

/*           Dear ImGui           */
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx9.h"
/*                                */


#ifdef DEBUG
	void inline debug(const char* msg);
#endif // DEBUG


/*           SDK           */
#include "vmt.h"
#include "CGlobalVars.h"
#include "checksum_md5.h"
#include "bones.h"
#include "CUniformRandomStream.h"
#include "offsets.h"
#include "vector.h"
#include "CEntity.h"
CPed* localPed = NULL;
#include "studiobone.h"
#include "CModelInfo.h"
#include "clientrenderable.h"
#include "panelwrapper.h"
#include "usercmd.h"
#include "trace.h"
#include "engine.h"
#include "cliententlist.h"
#include "icliententitylist.h"
#include "interfaces.h"
/*                         */


/*           MAIN           */
#include "settings.h"
#include "Misc.h"
#include "hooks.h"
#include "Menu.h"
#include "ESP.h"
/*                          */





typedef LRESULT(__stdcall* EndScene)(LPDIRECT3DDEVICE9);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);


