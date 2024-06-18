#pragma once

enum class LuaCode {
	LUA_ERROR = -1, // added by me
	LUA_OK = 0,
	LUA_ERRSYNTAX = 3,
	LUA_ERRMEM = 4,
	LUA_ERRERR = 5
};

enum class LuaInterfaceType : unsigned __int8 {
	CLIENT,
	SERVER,
	MENU
};


class CLuaFile {
public:
	char pad000[4];
	char* m_pFileName;
	char padd001[44];
	char* m_pContent;
};


class CConCommand {
public:
	int m_iArgsCount;
	char pad_0004[1028]; //0x0004
	const char* m_pArgList[64]; // first argument is always command name;
	const char* getArg(int index) {
		if (index >= 0 && index < m_iArgsCount) {
			return m_pArgList[index];
		}
		return "";
	}
	const char* getCommandName() {
		return m_pArgList[0];
	}



};


class CLuaInterface {
public:

	


	void** pVMT; // 0x0
	void* pLuaState; // 0x4



	int ExecuteCode(const char* pCode) {
		typedef int(__thiscall* fn)(void* pLuaInterface, const char* a2, const char* a3, const char* pCode, int a5, int a6);
		return VMT.getvfunc<fn>(this, 92)(this, "", "", pCode, 1, 1);
	}

	int OpenScript(const char* scriptName) {
		typedef int(__thiscall* fn)(void*, const char*, int, int, const char*, int );
		return VMT.getvfunc<fn>(this, 97)(this, scriptName, 1, 1, "!UNKNOWN", 1);
	}

	bool isServerInterface() {
		typedef bool(__thiscall* fn)(void*);
		return VMT.getvfunc<fn>(this, 75)(this);
	}
	
	bool isClientInterface() {
		typedef bool(__thiscall* fn)(void*);
		return VMT.getvfunc<fn>(this, 76)(this);
	}

	bool isMenuInterface() {
		typedef bool(__thiscall* fn)(void*);
		return VMT.getvfunc<fn>(this, 77)(this);
	}

	//int PrintLuaError(const char* a1, ...) {
	///*	typedef int(__cdecl* fn)(void*, const char*, ...);
	//	return VMT.getvfunc<fn>(this, 113)(this, a1, args);*/
	//	va_list va;
	//	va_start(va, a1);
	//	return (*(int (**)(void*, const char*, ...))(*(DWORD*)this + 0x1C4))(this, a1, va); // don't work properly
	//}


};


class CLuaShared {
public:



	virtual void unk0(); // 1
	virtual void unk1(); // 2
	virtual void unk2(); // 3
	virtual int LogLuaFileStats(); // 4 
	virtual void* CreateLuaInterface(unsigned __int8 a2, int a3); // 4
	virtual void unk3(); // 5
	// Check CLuaInterface* != nullptr after calling this  0 = CLIENT, 1 = SERVER, 2 = MENU
	virtual CLuaInterface* GetLuaInterface(LuaInterfaceType); // 6
	virtual void* ExecuteLuaFileMaybe(void* fileName, void* Src, int isClientInterface, const char* luaFilePath); // 7
	//////////////////////////////////////////////////////////////



	CLuaFile** m_pFiles; //0x0004
	int m_unk; //0x0008 // maybe max count of the files?
	char pad_000C[4]; //0x000C
	int m_iFilesInCache; //0x0010
	void* m_pUnk; //0x0014
	char pad_0018[20]; //0x0018
	int m_iFailedLoads; //0x002C
	int m_iSuccessfulLoads; //0x0030
	int m_iCacheHits; //0x0034
	int m_iDiskLoads; //0x0038



};