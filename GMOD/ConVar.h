#pragma once

class ConVar
{
	void** vmt;
public:
	class ConVar* m_pNext; //0x0004
	char pad_0008[4]; //0x0008
	char* m_pName; //0x000C
	char* m_pDescription; //0x0010
	char pad_0014[16]; //0x0014
	char* m_pStringValue; //0x0024
	int m_iMaxStringValueLength; //0x0028 // it updates everytime when we make pStringValue bigger and bigger
	float m_fFloatValue; //0x002C
	int m_iIntValue; //0x0030
};


class CCvar
{


public:
	char pad_0004[0x30]; //0x0000
	ConVar* m_pFirstConVar; //0x0030

};


class CConVarManager {
public:
	CCvar* Cvar;

	ConVar* GetConVar(const char* name) {
		ConVar* pCurrentConVar = Cvar->m_pFirstConVar;
		while (pCurrentConVar) {
			if (!strcmp(pCurrentConVar->m_pName, name)) {
				return pCurrentConVar;
			}
			pCurrentConVar = pCurrentConVar->m_pNext;

		}
		return nullptr;
	}



	CConVarManager() {
		Cvar = **reinterpret_cast<CCvar***>(SignatureManager.pCVarSig + 2);
	}


}; CConVarManager ConVarManager;