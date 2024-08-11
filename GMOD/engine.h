
typedef struct player_info_s {
	char name[128];
	int userid;
	char guid[33];
	unsigned int friendsid;
	char friendsname[128];
	bool fakeplayer;
	bool ishltv;
	unsigned int customfiles[4];
	unsigned char filesdownloaded;
} player_info_t;

class CEngine {

public:
	void GetPlayerNick(char* buffer, int entityIndex) {
		player_info_t pInfo;
		GetPlayerInfo(&pInfo, entityIndex);		
		strcpy(buffer, pInfo.name); // фиксим варнинг
	}
	void GetPlayerInfo(player_info_t* pInfo, int ent_num) {
		typedef bool(__thiscall* fn)(void*, int, player_info_t*); // фиксим варнинг
		VMT.getvfunc<fn>(this, 8)(this, ent_num, pInfo);
	}
	float* WorldToScreenMatrix() {
		typedef float*(__thiscall* fn)(void*);
		 return VMT.getvfunc<fn>(this, 36)(this);
	}
	int GetLocalPlayer() {	
		typedef int(__thiscall* fn)(void*);
		return VMT.getvfunc<fn>(this, 12)(this);
	}
	CVector GetViewAngles() {
		CVector angles;
		typedef void(__thiscall* fn)(void*, CVector&);
		VMT.getvfunc<fn>(this, 19)(this, angles);
		return angles;
	}
	void SetViewAngles(CVector angles) {
		typedef void(__thiscall* fn)(void*, CVector&);
		VMT.getvfunc<fn>(this, 20)(this, angles);
	} 
	bool IsInGame() {
		typedef bool(__thiscall* fn)(void*);
		return VMT.getvfunc<fn>(this, 26)(this);
	}
	/*
	void ExecuteClientCmd(const char* szCmdString) {
		typedef void(__thiscall* fn)(void*, const char*);
		VMT.getvfunc<fn>(this, 102)(this, szCmdString);
	}
	*/
};

