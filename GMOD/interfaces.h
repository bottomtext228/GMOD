typedef void* (*CreateInterfaceFn)(const char* pName, int* pReturnCode);


#ifdef _WIN64
constexpr auto RandomSeedOffset = 0x2;
#else 
constexpr auto RandomSeedOffset = 0x5;
#endif

class CInterfaces
{
public:

	CTrace* Trace;
	void* Client;
	IClientEntityList* ClientEntityList;
	CEngine* Engine;
	CUniformRandomStream* UniformRandomStream;
	VPanelWrapper* PanelWrapper;
	CModelInfo* ModelInfo;
	CPlayerInfoManager* PlayerInfo;
	CGlobalVarsBase* GlobalVars;
	void* EngineVGUI;
	CPrediction* Prediction;
	CGameMovement* GameMovement;
	CLuaShared* LuaShared;
	IMoveHelper* MoveHelper;
	/*IVRenderView* RenderView;*/
	/*ILocalize* Localize;*/
	void GetInterfaces()
	{
		Trace = (CTrace*)GetPointer("engine.dll", "EngineTraceClient");
		Client = GetPointer("client.dll", "VClient");
		ClientEntityList = (IClientEntityList*)GetPointer("client.dll", "VClientEntityList");
		Engine = (CEngine*)GetPointer("engine.dll", "VEngineClient");
		UniformRandomStream = VMT.GetVMT<CUniformRandomStream>((uintptr_t)GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomSeed"), RandomSeedOffset);
		PanelWrapper = (VPanelWrapper*)GetPointer("vgui2.dll", "VGUI_Panel");
		ModelInfo = (CModelInfo*)GetPointer("engine.dll", "VModelInfoClient");
		EngineVGUI = GetPointer("engine.dll", "VEngineVGui");
		LuaShared = (CLuaShared*)GetPointer("lua_shared.dll", "LUASHARED");
		Prediction = (CPrediction*)GetPointer("client.dll", "VClientPrediction");
		GameMovement = (CGameMovement*)GetPointer("client.dll", "GameMovement");
		MoveHelper = (IMoveHelper*)(SignatureManager.pMoveHelperClient);
		/*RenderView = (IVRenderView*)GetPointer("engine.dll", "VEngineRenderView");*/
		/*Localize = (ILocalize*)GetPointer("vgui2.dll", "VGUI_Localize");*/
		assert(Trace != NULL && "EngineTraceClient interface is not found.");
		assert(Client != NULL && "VClient interface is not found.");
		assert(Engine != NULL && "VEngineClient interface is not found.");
		assert(UniformRandomStream != NULL && "UniformRandomStream is not found.");
		assert(PanelWrapper != NULL && "PanelWrapper interface is not found.");
		assert(ModelInfo != NULL && "VModelInfoClient interface is not found.");
		assert(EngineVGUI != NULL && "VEngineVGui interface is not found.");
		assert(LuaShared != NULL && "LUASHARED interface is not found.");
		assert(Prediction != NULL && "Prediction interface is not found.");
		assert(GameMovement != NULL && "GameMovement interface is not found.");
		/*assert(Localize != NULL && "Localize interface is not found.");*/
		CreateInterfaceFn CreateInterface = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("server.dll"), "CreateInterface");
		// есть ещё PlayerInfoManager001 и он не подходит, поэтому нельзя через GetPointer()
		PlayerInfo = (CPlayerInfoManager*)CreateInterface("PlayerInfoManager002", NULL);
		assert(PlayerInfo != NULL && "PlayerInfoManager002 interface is not found.");
		GlobalVars = (CGlobalVarsBase*)(SignatureManager.pGlobalVars); //PlayerInfo->GetGlobalVars();


	}

	void* GetPointer(const char* Module, const char* InterfaceName)
	{
		void* Interface = NULL;
		char PossibleInterfaceName[1024];

		CreateInterfaceFn CreateInterface = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA(Module), "CreateInterface");
		for (int i = 1; i < 100; i++)
		{
			sprintf(PossibleInterfaceName, "%s0%i", InterfaceName, i);
			Interface = (void*)CreateInterface(PossibleInterfaceName, NULL);
			if (Interface != NULL)
			{
#ifdef  DEBUG
#ifndef _WIN64
				printf("%s Found: 0x%X\n", PossibleInterfaceName, (uintptr_t)Interface);
#else 
				printf("%s Found: 0x%llX\n", PossibleInterfaceName, (uintptr_t)Interface);
#endif
#endif //  DEBUG


				break;
			}
			sprintf(PossibleInterfaceName, "%s00%i", InterfaceName, i);
			Interface = (void*)CreateInterface(PossibleInterfaceName, NULL);
			if (Interface != NULL)
			{
#ifdef DEBUG
#ifndef _WIN64
				printf("%s Found: 0x%X\n", PossibleInterfaceName, (uintptr_t)Interface);
#else
				printf("%s Found: 0x%llX\n", PossibleInterfaceName, (uintptr_t)Interface);
#endif
#endif // DEBUG

				break;
			}
		}

		return Interface;
	}
}; CInterfaces Interfaces;