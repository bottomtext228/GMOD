#pragma once

enum class LuaType;

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
	char pad008[44];
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

	enum
	{
		SPECIAL_GLOB,       // Global table
		SPECIAL_ENV,        // Environment table
		SPECIAL_REG,        // Registry table
	};

	struct UserData
	{
		void* data;
		unsigned char type; // Change me to a uint32 one day
	};

	template <class T>
	struct UserData_Value : UserData
	{
		T value;
	};

	/*0*/	virtual void* Top(void) = 0;
	/*1*/	virtual void* Push(int iStackPos) = 0;
	/*2*/	virtual void* Pop(int iAmt = 1) = 0;
	/*3*/	virtual void* GetTable(int iStackPos) = 0;
	/*4*/	virtual void* const GetField(int iStackPos, char const*) = 0;
	/*5*/	virtual void SetField(int iStackPos, char const*) = 0;
	/*6*/	virtual void* CreateTable(void) = 0;
	/*7*/	virtual void* SetTable(int) = 0;
	/*8*/	virtual void* SetMetaTable(int) = 0;
	/*9*/	virtual void* GetMetaTable(int) = 0;
	/*10*/	virtual void* Call(int iArgs, int iRets) = 0;
	/*11*/	virtual void* PCall(int, int, int) = 0;
	/*12*/	virtual void* Equal(int, int) = 0;
	/*13*/	virtual void* RawEqual(int, int) = 0;
	/*14*/	virtual void* Insert(int) = 0;
	/*15*/	virtual void* Remove(int) = 0;
	/*16*/	virtual void* Next(int) = 0;
	/*17*/	virtual void* NewUserdata(unsigned int) = 0;
	/*18*/	virtual void* ThrowError(char const*) = 0;
	/*19*/	virtual void* CheckType(int, int) = 0;
	/*20*/	virtual void* ArgError(int, char const*) = 0;
	/*21*/	virtual void* RawGet(int iStackPos) = 0;
	/*22*/	virtual void* RawSet(int iStackPost) = 0;
	/*23*/	virtual const char* GetString(int iStackPos = -1, unsigned int* iOutLen = NULL) = 0;
	/*24*/	virtual double GetNumber(int iStackPos = -1) = 0;
	/*25*/	virtual bool GetBool(int iStackPos = -1) = 0;
	/*26*/	virtual void* GetCFunction(int iStackPos = -1) = 0;
	/*27*/	virtual void* GetUserdata(int iStackPos = -1) = 0;
	/*28*/	virtual void PushNil(void) = 0;
	/*29*/	virtual void PushString(char const*, unsigned int iLen = 0) = 0;
	/*30*/	virtual void PushNumber(double) = 0;
	/*31*/	virtual void PushBool(bool) = 0;
	/*32*/	virtual void PushCFunction(int (*)(void*)) = 0;
	/*33*/	virtual void PushCClosure(int (*)(void*), int) = 0;
	/*34*/	virtual void PushUserdata(void*) = 0;
	/*35*/	virtual void* ReferenceCreate(void) = 0;
	/*36*/	virtual void* ReferenceFree(int) = 0;
	/*37*/	virtual void* ReferencePush(int) = 0;
	/*38*/	virtual void* PushSpecial(int) = 0;
	/*39*/	virtual bool IsType(int, LuaType) = 0;
	/*40*/	virtual void GetType(int) = 0;
	/*41*/	virtual void* GetTypeName(int) = 0;
	/*42*/	virtual void* CreateMetaTableType(char const*, int) = 0;
	/*43*/	virtual void* CheckString(int) = 0;
	/*44*/	virtual void* CheckNumber(int) = 0;
	/*45*/	virtual void* ObjLen(int) = 0;
	/*46*/	virtual void* GetAngle(int) = 0;
	/*47*/	virtual void* GetVector(int) = 0;
	/*48*/	virtual void* PushAngle(CVector const&) = 0;
	/*49*/	virtual void* PushVector(CVector const&) = 0;
	/*50*/	virtual void* SetState(void*) = 0;
	/*51*/	virtual void* CreateMetaTable(char const*) = 0;
	/*52*/	virtual void* PushMetaTable(int) = 0;
	/*53*/	virtual void* PushUserType(void*, int) = 0;
	/*54*/	virtual void* SetUserType(int, void*) = 0;
	/*55*/	virtual void* Init(void*, bool) = 0;
	/*56*/	virtual void* Shutdown(void) = 0;
	/*57*/	virtual void* Cycle(void) = 0;
	/*58*/	virtual void* Global(void) = 0;
	/*59*/	virtual void* GetObjectt(int) = 0;
	/*60*/	virtual void* PushLuaObject(void*) = 0;
	/*61*/	virtual void* PushLuaFunction(int (*)(void*)) = 0;
	/*62*/	virtual void* LuaError(char const*, int) = 0;
	/*63*/	virtual void* TypeError(char const*, int) = 0;
	/*64*/	virtual void* CallInternal(int, int) = 0;
	/*65*/	virtual void* CallInternalNoReturns(int) = 0;
	/*66*/	virtual void* CallInternalGetBool(int) = 0;
	/*67*/	virtual void* CallInternalGetString(int) = 0;
	/*68*/	virtual void* CallInternalGet(int, void*) = 0;
	/*69*/	virtual void* NewGlobalTable(char const*) = 0;
	/*70*/	virtual void* NewTemporaryObject(void) = 0;
	/*71*/	virtual bool isUserData(int) = 0;
	/*72*/	virtual void* GetMetaTableObject(char const*, int) = 0;
	/*73*/	virtual void* GetMetaTableObject(int) = 0;
	/*74*/	virtual void* GetReturn(int) = 0;
	/*75*/	virtual bool IsServer(void) = 0;
	/*76*/	virtual bool IsClient(void) = 0;
	/*77*/	virtual bool IsMenu(void) = 0;
	/*78*/	virtual void* DestroyObject(void*) = 0;
	/*79*/	virtual void* CreateObject(void) = 0;
	/*80*/	virtual void* SetMember(void*, void*, void*) = 0;
	/*81*/	virtual void* GetNewTable(void) = 0;
	/*82*/	virtual void* SetMember(void*, float) = 0;
	/*83*/	virtual void* SetMember(void*, float, void*) = 0; // ILuaObject
	/*84*/	virtual void* SetMember(void*, char const*) = 0;
	/*85*/	virtual void* SetMember(void*, char const*, void*) = 0; // ILuaObject* key, ILuaObject* value
	/*86*/	virtual void* SetType(unsigned char) = 0;
	/*87*/	virtual void* PushLong(long) = 0;
	/*88*/	virtual void* GetFlags(int) = 0;
	/*89*/	virtual void* FindOnObjectsMetaTable(int, int) = 0;
	/*90*/	virtual void* FindObjectOnTable(int, int) = 0;
	/*91*/	virtual void* SetMemberFast(void*, int, int) = 0;
	/*92*/	virtual void* RunString(char const*, char const*, char const*, bool, bool) = 0;
	/*93*/	virtual bool IsEqual(void*, void*) = 0;
	/*94*/	virtual void* Error(char const*) = 0;
	/*95*/	virtual void* GetStringOrError(int) = 0;
	/*96*/	virtual void* RunLuaModule(char const*) = 0;
	/*97*/	virtual void* FindAndRunScript(char const*, bool, bool, char const*, bool) = 0;
	/*98*/	virtual void* SetPathID(char const*) = 0;
	/*99*/	virtual const char* GetPathID(void) = 0;
	/*100*/	virtual void* ErrorNoHalt(char const*, ...) = 0;
	/*101*/	virtual void* Msg(char const*, ...) = 0;
	/*102*/	virtual void* PushPath(char const*) = 0;
	/*103*/	virtual void* PopPath(void) = 0;
	/*104*/	virtual const char* GetPath(void) = 0;
	/*105*/	virtual void* GetColor(int) = 0;
	/*106*/	virtual void* PushColor(void*) = 0;
	/*107*/	virtual void* GetStack(int, void*) = 0;
	/*108*/	virtual void* GetInfo(char const*, void*) = 0;
	/*109*/	virtual void* GetLocal(void*, int) = 0;
	/*110*/	virtual void* GetUpvalue(int, int) = 0;
	/*111*/	virtual void* RunStringEx(char const*, char const*, char const*, bool, bool, bool, bool) = 0;
	/*112*/	virtual void* GetDataString(int, void**) = 0;
	/*113*/	virtual void* ErrorFromLua(char const*, ...) = 0;
	/*114*/	virtual void* GetCurrentLocation(void) = 0;
	/*115*/	virtual void* MsgColour(void* const&, char const*, ...) = 0;
	/*116*/	virtual void* GetCurrentFile(std::string, std::string) = 0;
	/*117*/	virtual void* CompileString(void*, std::string const&) = 0;
	/*118*/	virtual void* CallFunctionProtected(int, int, bool) = 0;
	/*119*/	virtual void* Require(char const*) = 0;
	/*120*/	virtual void* GetActualTypeName(int) = 0;
	/*121*/	virtual void* PreCreateTable(int, int) = 0;
	/*122*/	virtual void* PushPooledString(int) = 0;
	/*123*/	virtual void* GetPooledString(int) = 0;
	/*124*/	virtual void* AddThreadedCall(void*) = 0;
	/*125*/	virtual void* AppendStackTrace(char*, unsigned long) = 0;
	/*126*/	virtual void* CreateConVar(char const*, char const*, char const*, int) = 0;
	/*127*/	virtual void* CreateConCommand(char const*, char const*, int, void (*)(void const*), int (*)(char const*, char(*)[128])) = 0;

	int ExecuteCode(const char* pCode) {
		typedef int(__thiscall* fn)(void* pLuaInterface, const char* a2, const char* a3, const char* pCode, int a5, int a6);
		return VMT.getvfunc<fn>(this, 92)(this, "", "", pCode, 1, 1);
	}

	int OpenScript(const char* scriptName) {
		typedef int(__thiscall* fn)(void*, const char*, int, int, const char*, int);
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

	/*	void PushNumber(double value) {
			typedef void(__thiscall* fn)(void*, double);
			return VMT.getvfunc<fn>(this, 30)(this, value);
		}*/

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



enum class LuaType
{


	// Default Lua Types
	None = -1,
	Nil,
	Bool,
	LightUserData,
	Number,
	String,
	Table,
	Function,
	UserData,
	Thread,

	// GMod Types
	Entity,
	Vector,
	Angle,
	PhysObj,
	Save,
	Restore,
	DamageInfo,
	EffectData,
	MoveData,
	RecipientFilter,
	UserCmd,
	ScriptedVehicle,
	Material,
	Panel,
	Particle,
	ParticleEmitter,
	Texture,
	UserMsg,
	ConVar,
	IMesh,
	Matrix,
	Sound,
	PixelVisHandle,
	DLight,
	Video,
	File,
	Locomotion,
	Path,
	NavArea,
	SoundHandle,
	NavLadder,
	ParticleSystem,
	ProjectedTexture,
	PhysCollide,
	SurfaceInfo,

	Type_Count,


	// Lua Types
	NONE = None,
	NIL = Nil,
	BOOL = Bool,
	LIGHTUSERDATA = LightUserData,
	NUMBER = Number,
	STRING = String,
	TABLE = Table,
	FUNCTION = Function,
	USERDATA = UserData,
	THREAD = Thread,

	// GMod Types
	ENTITY = Entity,
	VECTOR = Vector,
	ANGLE = Angle,
	PHYSOBJ = PhysObj,
	SAVE = Save,
	RESTORE = Restore,
	DAMAGEINFO = DamageInfo,
	EFFECTDATA = EffectData,
	MOVEDATA = MoveData,
	RECIPIENTFILTER = RecipientFilter,
	USERCMD = UserCmd,
	SCRIPTEDVEHICLE = ScriptedVehicle,
	MATERIAL = Material,
	PANEL = Panel,
	PARTICLE = Particle,
	PARTICLEEMITTER = ParticleEmitter,
	TEXTURE = Texture,
	USERMSG = UserMsg,
	CONVAR = ConVar,
	IMESH = IMesh,
	MATRIX = Matrix,
	SOUND = Sound,
	PIXELVISHANDLE = PixelVisHandle,
	DLIGHT = DLight,
	VIDEO = Video,
	FILE = File,
	LOCOMOTION = Locomotion,
	PATH = Path,
	NAVAREA = NavArea,
	SOUNDHANDLE = SoundHandle,
	NAVLADDER = NavLadder,
	PARTICLESYSTEM = ParticleSystem,
	PROJECTEDTEXTURE = ProjectedTexture,
	PHYSCOLLIDE = PhysCollide,

	COUNT = Type_Count

};
