#pragma once

//
//class ConVar;
//
//class ConCommandBase
//{
//public:
//	// Next ConVar in chain
//	ConVar* m_pNext;
//
//	// Has the cvar been added to the global list?
//	bool						m_bRegistered;
//
//	// Static data
//	const char* m_pszName;
//	const char* m_pszHelpString;
//
//	// ConVar flags
//	int							m_nFlags;
//
//	//static ConCommandBase* s_pConCommandBases;
//	//static IConCommandBaseAccessor* s_pAccessor;
//};

//class ConVar : public ConCommandBase {
//	ConVar* m_pParent;
//
//	// Static data
//	const char* m_pszDefaultValue;
//
//	// Value
//	// Dynamically allocated
//	char* m_pszString;
//	int							m_StringLength;
//
//	// Values
//	float						m_fValue;
//	int							m_nValue;
//
//	// Min/Max values
//	bool						m_bHasMin;
//	float						m_fMinVal;
//	bool						m_bHasMax;
//	float						m_fMaxVal;
//
//	// Call this function when ConVar changes
//	void*			m_fnChangeCallback;
//
//	// Min/Max values for competitive.
//	bool						m_bHasCompMin;
//	float						m_fCompMinVal;
//	bool						m_bHasCompMax;
//	float						m_fCompMaxVal;
//
//	bool						m_bCompetitiveRestrictions;
//};


class ConVar
{
	void** vmt;
public:
	class ConVar* m_pNext;
	uintptr_t pad_000;
	char* m_pName;
	char* m_pDescription;
#ifdef _WIN64
	char pad_001[23];
#else
	char pad_001[12];
#endif
	char* m_pDefaultValue;
	char* m_pStringValue;
	int m_iMaxStringValueLength;
#ifndef _WIN64 // I think in x64 these values are not present and only string value is parsed and used
	float m_fFloatValue; //0x002C
	int m_iIntValue; //0x0030
#endif

	float GetFloat() {
#ifdef _WIN64
		try {
			return std::stof(m_pStringValue);
		}
		catch (exception) {
			return 0.0f;
		}
#else
		return m_fFloatValue;
#endif 
	}

	int GetInt() {
#ifdef _WIN64
		try {
			return std::stoi(m_pStringValue);
		}
		catch (exception) {
			return 0;
		}
#else
		return m_iIntValue;
#endif 
	}
};


//class ConVar
//{
//	void** vmt;
//public:
//	class ConVar* m_pNext; //0x0004
//	char pad_0008[4]; //0x0008
//	char* m_pName; //0x000C
//	char* m_pDescription; //0x0010
//	char pad_0014[16]; //0x0014
//	char* m_pStringValue; //0x0024
//	int m_iMaxStringValueLength; //0x0028 // it updates everytime when we make pStringValue bigger and bigger
//	float m_fFloatValue; //0x002C
//	int m_iIntValue; //0x0030
//};



class CCvar
{
public:
#ifdef _WIN64
	char pad_0004[0x50];
#else 
	char pad_0004[0x30];
#endif
	ConVar* m_pFirstConVar;
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
		Cvar = reinterpret_cast<CCvar*>(SignatureManager.pCVarSig);
	}

}; CConVarManager ConVarManager;


// not tested on x64
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