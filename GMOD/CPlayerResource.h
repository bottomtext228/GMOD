#pragma once

//class CPlayerResource {
//	uintptr_t g_pPlayerResource;
//public:
//	CPlayerResource() {
//		// if CPlayerResource will be needed don't forget to uncomment pPlayerResource in CSignatureManager.h 
//		g_pPlayerResource = **reinterpret_cast<uintptr_t**>(SignatureManager.pPlayerResource + 2);
//	}
//	const char* GetName(int index) {
//		// 0x1390
//		return *(const char**)((g_pPlayerResource + 0x1394) + index * 4);
//	}
//	// CPlayerResource stores player names, score, ping, deaths count, etc.
//	// Info stored like that:
//	// const char* pszNames[MAX_PLAYERS_COUNT];
//	// int scores[MAX_PLAYERS_COUNT];
//} PlayerResource;