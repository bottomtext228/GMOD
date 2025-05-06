#pragma once
#define SignatureAssert(sig) (assert(sig != NULL && "Signature is not found."))

class CSignatureManager {
public:
	// NoSpread
	uintptr_t fnShotManipulator_ApplySpread;
	uintptr_t fnShotManipulator_constructor;
	uintptr_t pMoveHelperClient;


	uintptr_t pCVarSig;

	uintptr_t pGlobalVars;

	//uintptr_t pPlayerResource;
	uintptr_t pbSendPacket;

	// Prediction  
	uintptr_t fnCPredictableIdHelper_Reset;
	uintptr_t pm_nPredictionRandomSeed;
	uintptr_t pm_pPredictionPlayer;
	uintptr_t fnCBaseEntity_PhysicsRunThink;
	uintptr_t fnCBaseEntity_GetNextThinkTick;
	uintptr_t fnCBaseEntity_SetNextThink;
	uintptr_t fnCBasePlayer_SetPlayerCollisionBounds;
	uintptr_t pDTStore;

	CSignatureManager() {
		fnShotManipulator_ApplySpread = BTMemory::FindSignature("client.dll", "\x55\x8B\xEC\xF3\x0F\x10\x05\x00\x00\x00\x00\x83\xEC\x14\xF3\x0F\x10\x4D\x00\x0F\x2F\xC8\x56\x8B\xF1\x77\x08\x0F\x57\xC0\x0F\x2F\xC1", "xxxxxxx????xxxxxxx?xxxxxxxxxxxxxx");// reinterpret_cast<CShotManipulator_ApplySpread>(vars::client + 0x1515A0);
		fnShotManipulator_constructor = BTMemory::FindSignature("client.dll", "\x0F\x28\xDC\xC7\x00\x00\x00\x00\x00", "xxxxx????"); //reinterpret_cast<CShotManipulator_constructor>(vars::client + 0x3B7A10);
		// must sub 0x2F

		pCVarSig = BTMemory::FindSignature("engine.dll", "\x8B\x0D\x00\x00\x00\x00\x33\xC0\x89\x45\xF8\x83\xC4\x04\x89\x45\xF4", "xx????xxxxxxxxxxx");
		//pPlayerResource = BTMemory::FindSignature("client.dll", "\x89\x3D\x00\x00\x00\x00\x8B\xCF\x8B\x17\xFF\x75\x0C\xFF\x75\x08\xFF\x92\x00\x00\x00\x00\x33\xC0\x85\xFF\x0F\x44\xF0", "xx????xxxxxxxxxxxx????xxxxxxx");

		// must add + 0x2
		pMoveHelperClient = BTMemory::FindSignature("client.dll", "\x8B\x0D\x00\x00\x00\x00\x8B\x46\x08\x68", "xx????xxxx");

		pGlobalVars = BTMemory::FindSignature("client.dll", "\xA1\x00\x00\x00\x00\x83\xC4\x04\x89\x3D\x00\x00\x00\x00", "x????xxxxx????");
		// must add + 0x1

		pbSendPacket = BTMemory::FindSignature("engine.dll", "\xC6\x45\xFF\x01\x8B\x01\x8B\x40\x18", "xxxxxxxxx");
		// must add + 0x3

		fnCPredictableIdHelper_Reset = BTMemory::FindSignature("client.dll", "\xE8\x00\x00\x00\x00\x8B\x7D\x08\x8B\x75\x0C", "x????xxxxxx");
		//fnCPredictableIdHelper_Reset + *(fnCPredictableIdHelper_Reset + 0x1) + 0x5;

		pm_nPredictionRandomSeed = BTMemory::FindSignature("client.dll", "\x75\x0C\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\x5D", "xxxx????????x");
		// must add + 0x4;

		pm_pPredictionPlayer = BTMemory::FindSignature("client.dll", "\x83\xC4\x04\x89\x3D\x00\x00\x00\x00", "xxxxx????");
		// must add + 0x5;

		fnCBaseEntity_PhysicsRunThink = BTMemory::FindSignature("client.dll", "\xE8\x00\x00\x00\x00\x84\xC0\x0F\x84\x00\x00\x00\x00\xF3\x0F\x10\x87\x00\x00\x00\x00", "x????xxxx????xxxx????");
		// fnCBaseEntity_PhysicsRunThink + *(fnCBaseEntity_PhysicsRunThink + 0x1) + 0x5;

		fnCBaseEntity_GetNextThinkTick = BTMemory::FindSignature("client.dll", "\xE8\x00\x00\x00\x00\x85\xC0\x7E\x25\x3B\x87\x00\x00\x00\x00", "x????xxxxxx????");
		// fnCBaseEntity_GetNextThinkTick + *(fnCBaseEntity_GetNextThinkTick + 0x1) + 0x5;

		fnCBaseEntity_SetNextThink = BTMemory::FindSignature("client.dll", "\xE8\x00\x00\x00\x00\x8B\x46\x08\x8D\x4E\x08\xC6\x86\x00\x00\x00\x00\x00", "x????xxxxxxxx?????");
		// fnCBaseEntity_SetNextThink + *(fnCBaseEntity_SetNextThink + 0x1) + 0x5;

		fnCBasePlayer_SetPlayerCollisionBounds = BTMemory::FindSignature("client.dll", "\x8B\x91\x00\x00\x00\x00\xB8\x00\x00\x00\x00\x56", "xx????x????x");

		pDTStore = BTMemory::FindSignature("engine.dll", "\x8B\x7D\x08\x8B\x0D\x00\x00\x00\x00\x8D\x64\x24\x00", "xxxxx????xxxx");
		// must add 0x5;

		SignatureAssert(fnShotManipulator_ApplySpread);
		SignatureAssert(fnShotManipulator_constructor);
		SignatureAssert(pCVarSig);
		SignatureAssert(pMoveHelperClient);
		SignatureAssert(fnCPredictableIdHelper_Reset);
		SignatureAssert(pm_nPredictionRandomSeed);
		SignatureAssert(pm_pPredictionPlayer);
		SignatureAssert(pDTStore);
		SignatureAssert(fnCBaseEntity_PhysicsRunThink);
		SignatureAssert(fnCBaseEntity_GetNextThinkTick);
		SignatureAssert(fnCBaseEntity_SetNextThink);
		SignatureAssert(fnCBasePlayer_SetPlayerCollisionBounds);
		SignatureAssert(pGlobalVars);
		SignatureAssert(pbSendPacket);
	}
} SignatureManager;