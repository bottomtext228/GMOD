typedef void* (*CreateInterfaceFn)(const char* pName, int* pReturnCode);


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
	void GetInterfaces()
	{
	
		Trace = (CTrace*)GetPointer("engine.dll", "EngineTraceClient");
		Client = GetPointer("client.dll", "VClient");
		ClientEntityList = (IClientEntityList*)GetPointer("client.dll", "VClientEntityList");
		Engine = (CEngine*)GetPointer("engine.dll", "VEngineClient");
		UniformRandomStream = VMT.GetVMT<CUniformRandomStream>((uintptr_t)GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomSeed"), 0x5); 
		PanelWrapper = (VPanelWrapper*)GetPointer("vgui2.dll", "VGUI_Panel");
		ModelInfo = (CModelInfo*)GetPointer("engine.dll", "VModelInfoClient");
		EngineVGUI = GetPointer("engine.dll", "VEngineVGui");
		CreateInterfaceFn CreateInterface = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("server.dll"), "CreateInterface");
		// есть ещё PlayerInfoManager001 и он не подходит, поэтому нельзя через GetPointer()
		PlayerInfo = (CPlayerInfoManager*)CreateInterface("PlayerInfoManager002", NULL); 
		GlobalVars = PlayerInfo->GetGlobalVars(); 

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
				printf("%s Found: 0x%X\n", PossibleInterfaceName, (DWORD)Interface);
				#endif //  DEBUG

				
				break;
			}
			sprintf(PossibleInterfaceName, "%s00%i", InterfaceName, i);
			Interface = (void*)CreateInterface(PossibleInterfaceName, NULL);
			if (Interface != NULL)
			{
				#ifdef DEBUG
				printf("%s Found: 0x%X\n", PossibleInterfaceName, (DWORD)Interface);
				#endif // DEBUG

				break;
			}
		}

		return Interface;
	}
}; CInterfaces Interfaces;