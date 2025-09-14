#pragma once
#define SignatureAssert(sig) (assert(sig != NULL && "Signature is not found."))

uintptr_t GetRealFromRelative(uintptr_t address, int offset, int instructionSize, bool isRelative) // Address must be an instruction, not a pointer! And offset = the offset to the bytes you want to retrieve.
{
#ifdef _WIN64
	isRelative = true;
#endif
	uintptr_t instruction = address + offset;
	if (!isRelative)
	{
		return *(uintptr_t*)(instruction);
	}

	int relativeAddress = *(int*)(instruction);
	uintptr_t realAddress = address + instructionSize + relativeAddress;
	return realAddress;
}

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

		//ida sigs: 
		// x86: 
		//  55 8B EC F3 0F 10 05 ? ? ? ? 83 EC 14 F3 0F 10 4D
		// \x55\x8B\xEC\xF3\x0F\x10\x05\x00\x00\x00\x00\x83\xEC\x14\xF3\x0F\x10\x4D\x00\x0F\x2F\xC8\x56\x8B\xF1\x77\x08\x0F\x57\xC0\x0F\x2F\xC1", "xxxxxxx????xxxxxxx?xxxxxxxxxxxxxx"
		// 
		// 
		// x86 chromium:
		// 55 8B EC F3 0F 10 45 ? 83 EC 10 F3 0F 5F 05 ? ? ? ? 56 8B F1 8B 0D ? ? ? ?
		// \x55\x8B\xEC\xF3\x0F\x10\x45\x00\x83\xEC\x10\xF3\x0F\x5F\x05\x00\x00\x00\x00\x56\x8B\xF1\x8B\x0D\x00\x00\x00\x00, xxxxxxx ? xxxxxxx ? ? ? ? xxxxx ? ? ? ?
		//
		// x64 chromium:
		// 48 8B C4 48 89 58 08 57 48 81 EC ? ? ? ? 0F 29 70 E8 48 8B FA 0F 29 78 D8 48 8D 15 ? ? ? ? 44 0F 29 40 ? 
		// \x48\x8B\xC4\x48\x89\x58\x08\x57\x48\x81\xEC\x00\x00\x00\x00\x0F\x29\x70\xE8\x48\x8B\xFA\x0F\x29\x78\xD8\x48\x8D\x15\x00\x00\x00\x00\x44\x0F\x29\x40\x00, xxxxxxxxxxx????xxxxxxxxxxxxxx????xxxx?
#ifdef _WIN64
		fnShotManipulator_ApplySpread = BTMemory::FindSignature("client.dll", "\x48\x8B\xC4\x48\x89\x58\x08\x57\x48\x81\xEC\x00\x00\x00\x00\x0F\x29\x70\xE8\x48\x8B\xFA\x0F\x29\x78\xD8\x48\x8D\x15\x00\x00\x00\x00\x44\x0F\x29\x40\x00", "xxxxxxxxxxx????xxxxxxxxxxxxxx????xxxx??");
#else
		fnShotManipulator_ApplySpread = BTMemory::FindSignature("client.dll", "\x55\x8B\xEC\xF3\x0F\x10\x05\x00\x00\x00\x00\x83\xEC\x14\xF3\x0F\x10\x4D\x00\x0F\x2F\xC8\x56\x8B\xF1\x77\x08\x0F\x57\xC0\x0F\x2F\xC1", "xxxxxxx????xxxxxxx?xxxxxxxxxxxxxx");
#endif


		// ida sigs:
		// x86:
		// must sub 0x2F
		// "\x0F\x28\xDC\xC7\x00\x00\x00\x00\x00", "xxxxx????"
		// 
		// x86 chromium:
		// 55 8B EC 8B 4D 08 F3 0F 10 0D ? ? ? ? F2 0F 10 15 ? ? ? ? 8B 55 10 F3 0F 10 01 
		// \x55\x8B\xEC\x8B\x4D\x08\xF3\x0F\x10\x0D\x00\x00\x00\x00\xF2\x0F\x10\x15\x00\x00\x00\x00\x8B\x55\x10\xF3\x0F\x10\x01, xxxxxxxxxx????xxxx????xxxxxxx
		//
		// x64 chromium:
		// 48 83 EC 38 F3 0F 10 01 F3 0F 10 0D ? ? ? ? F2 0F 10 15 ? ? ? ? 0F 54 C1 0F 29 74 24 ? 
		// \x48\x83\xEC\x38\xF3\x0F\x10\x01\xF3\x0F\x10\x0D\x00\x00\x00\x00\xF2\x0F\x10\x15\x00\x00\x00\x00\x0F\x54\xC1\x0F\x29\x74\x24\x00, xxxxxxxxxxxx????xxxx????xxxxxxx?
#ifdef _WIN64
		fnShotManipulator_constructor = BTMemory::FindSignature("client.dll", "\x48\x83\xEC\x38\xF3\x0F\x10\x01\xF3\x0F\x10\x0D\x00\x00\x00\x00\xF2\x0F\x10\x15\x00\x00\x00\x00\x0F\x54\xC1\x0F\x29\x74\x24\x00", "xxxxxxxxxxxx????xxxx????xxxxxxx?");
		//fnShotManipulator_constructor = BTMemory::FindSignature("client.dll", "\x0F\x28\xDC\xC7\x00\x00\x00\x00\x00", "xxxxx????");
#else
		fnShotManipulator_constructor = BTMemory::FindSignature("client.dll", "\x0F\x28\xDC\xC7\x00\x00\x00\x00\x00", "xxxxx????");
#endif

		// x86:
		// must add + 0x2
		// 8B 0D ?? ?? ?? ?? 33 C0 89 45 FC 83 C4 04 89 45 F8 C7 45 ?? ?? ?? ?? ?? 8B 01
		// must add + 0x2
		// 
		// x86 chromium:
		// must add + 0x2
		// 8B 0D ? ? ? ? 33 DB 66 89 45 0A 66 89 45 E0 89 5D D0 
		// \x8B\x0D\x00\x00\x00\x00\x33\xDB\x66\x89\x45\x0A\x66\x89\x45\xE0\x89\x5D\xD0, xx????xxxxxxxxxxxxx
		// 
		// x64 chromium:
		// 48 8B 0D ? ? ? ? 48 8D 05 ? ? ? ? 4C 8B E0 48 89 44 24 ? 41 8B C7
		// \x48\x8B\x0D\x00\x00\x00\x00\x48\x8D\x05\x00\x00\x00\x00\x4C\x8B\xE0\x48\x89\x44\x24\x00\x41\x8B\xC7, xxx????xxx????xxxxxxx?xxx
#ifdef _WIN64
		pCVarSig = BTMemory::FindSignature("engine.dll", "\x48\x8B\x0D\x00\x00\x00\x00\x48\x8D\x05\x00\x00\x00\x00\x4C\x8B\xE0\x48\x89\x44\x24\x00\x41\x8B\xC7", "xxx????xxx????xxxxxxx?xxx");
#else
		pCVarSig = BTMemory::FindSignature("engine.dll", "\x8B\x0D\x00\x00\x00\x00\x33\xC0\x89\x45\xFC\x83\xC4\x04\x89\x45\xF8\xC7\x45\x00\x00\x00\x00\x00\x8B\x01", "xx????xxxxxxxxxxxxx?????xx");
#endif
		// not used
		////pPlayerResource = BTMemory::FindSignature("client.dll", "\x89\x3D\x00\x00\x00\x00\x8B\xCF\x8B\x17\xFF\x75\x0C\xFF\x75\x08\xFF\x92\x00\x00\x00\x00\x33\xC0\x85\xFF\x0F\x44\xF0", "xx????xxxxxxxxxxxx????xxxxxxx");

		// x86:
		// must add + 0x2
		// 8B 0D ? ? ? ? 8B 46 08 68
		// \x8B\x0D\x00\x00\x00\x00\x8B\x46\x08\x68, xx????xxxx
		// 
		// x86 chromium:
		// must add + 0x2
		// 8B 0D ? ? ? ? 8B 46 08 68
		// \x8B\x0D\x00\x00\x00\x00\x8B\x46\x08\x68, xx????xxxx
		// 
		// x64 chromium:
		// 48 8B 0D ? ? ? ? 48 8B 53 10 48 81 C2 ? ? ? ? 48 8B 01 FF 50 28 
		// \x48\x8B\x0D\x00\x00\x00\x00\x48\x8B\x53\x10\x48\x81\xC2\x00\x00\x00\x00\x48\x8B\x01\xFF\x50\x28, xxx????xxxxxxx????xxxxxx
#ifdef _WIN64
		pMoveHelperClient = BTMemory::FindSignature("client.dll", "\x48\x8B\x0D\x00\x00\x00\x00\x48\x8B\x53\x10\x48\x81\xC2\x00\x00\x00\x00\x48\x8B\x01\xFF\x50\x28", "xxx????xxxxxxx????xxxxxx");
#else 
		pMoveHelperClient = BTMemory::FindSignature("client.dll", "\x8B\x0D\x00\x00\x00\x00\x8B\x46\x08\x68", "xx????xxxx");
#endif
		// x86:
		// must add + 0x1
		// A1 ? ? ? ? 83 C4 04 89 3D
		// \xA1\x00\x00\x00\x00\x83\xC4\x04\x89\x3D\x00\x00\x00\x00, x????xxxxx????
		// 
		// x86 chromium:
		// must add + 0x1
		// A1 ? ? ? ? 83 C4 04 89 3D
		// \xA1\x00\x00\x00\x00\x83\xC4\x04\x89\x3D\x00\x00\x00\x00, x????xxxxx????
		// 
		// x64 chromium:
		// 48 8B 05 ? ? ? ? 0F 57 F6 48 89 3D ? ? ? ? 66 0F 6E 87 ? ? ? ? 0F 5B C0 
		// \x48\x8B\x05\x00\x00\x00\x00\x0F\x57\xF6\x48\x89\x3D\x00\x00\x00\x00\x66\x0F\x6E\x87\x00\x00\x00\x00\x0F\x5B\xC0, xxx????xxxxxx????xxxx????xxx
#ifdef _WIN64

		pGlobalVars = BTMemory::FindSignature("client.dll", "\x48\x8B\x05\x00\x00\x00\x00\x0F\x57\xF6\x48\x89\x3D\x00\x00\x00\x00\x66\x0F\x6E\x87\x00\x00\x00\x00\x0F\x5B\xC0", "xxx????xxxxxx????xxxx????xxx");
#else 
		pGlobalVars = BTMemory::FindSignature("client.dll", "\xA1\x00\x00\x00\x00\x83\xC4\x04\x89\x3D\x00\x00\x00\x00", "x????xxxxx????");
#endif
		// x86:
		// must add + 0x3
		// C6 45 FF 01 8B 01 8B 40 18
		// \xC6\x45\xFF\x01\x8B\x01\x8B\x40\x18, xxxxxxxxx
		// 
		// x86 chromium:
		// must add + 0x1
		// B3 01 8B 01 8B 40 18 FF D0 
		// \xB3\x01\x8B\x01\x8B\x40\x18\xFF\xD0, xxxxxxxxx
		// 
		// x64 chromium:
		// must add + 0x2;
		// 40 B7 01 48 8B 01 FF 50 30 84 C0
		// \x40\xB7\x01\x48\x8B\x01\xFF\x50\x30\x84\xC0, xxxxxxxxxxx
#ifdef _WIN64
		pbSendPacket = BTMemory::FindSignature("engine.dll", "\x40\xB7\x01\x48\x8B\x01\xFF\x50\x30\x84\xC0", "xxxxxxxxxxx");
#else 
		pbSendPacket = BTMemory::FindSignature("engine.dll", "\xC6\x45\xFF\x01\x8B\x01\x8B\x40\x18", "xxxxxxxxx");
#endif
		// x86:
		// E8 ? ? ? ? 8B 7D 08 8B 75 0C
		// \xE8\x00\x00\x00\x00\x8B\x7D\x08\x8B\x75\x0C, x????xxxxxx
		// 
		// x86 chromium:
		// E8 ? ? ? ? 8B 7D 08 8B 75 0C
		// \xE8\x00\x00\x00\x00\x8B\x7D\x08\x8B\x75\x0C, x????xxxxxx
		// 
		// x64 chromium:
		// E8 ? ? ? ? 48 8B CD 48 89 AF ? ? ? ? E8 ? ? ? ? 
		// \xE8\x00\x00\x00\x00\x48\x8B\xCD\x48\x89\xAF\x00\x00\x00\x00\xE8\x00\x00\x00\x00, x????xxxxxx????x????
#ifdef _WIN64
		fnCPredictableIdHelper_Reset = BTMemory::FindSignature("client.dll", "\xE8\x00\x00\x00\x00\x48\x8B\xCD\x48\x89\xAF\x00\x00\x00\x00\xE8\x00\x00\x00\x00", "x????xxxxxx????x????");
#else
		fnCPredictableIdHelper_Reset = BTMemory::FindSignature("client.dll", "\xE8\x00\x00\x00\x00\x8B\x7D\x08\x8B\x75\x0C", "x????xxxxxx");
#endif

		// x86:
		// must add + 0x4
		// 75 0C C7 05 ? ? ? ? ? ? ? ? 5D
		// \x75\x0C\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\x5D, xxxx????????x
		// 
		// x86 chromium:
		// must add + 0x4
		// 75 0C C7 05 ? ? ? ? ? ? ? ? 5D
		// \x75\x0C\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\x5D, xxxx????????x
		// 
		// x64 chromium:
		// 48 85 C9 75 0B C7 05 ? ? ? ? ? ? ? ? 
		// \x48\x85\xC9\x75\x0B\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00, xxxxxxx????????
#ifdef _WIN64
		pm_nPredictionRandomSeed = BTMemory::FindSignature("client.dll", "\x48\x85\xC9\x75\x0B\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00", "xxxxxxx????????");
#else 
		pm_nPredictionRandomSeed = BTMemory::FindSignature("client.dll", "\x75\x0C\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\x5D", "xxxx????????x");
#endif
		// x86:
		// must add + 0x5
		// 83 C4 04 89 3D
		// \x83\xC4\x04\x89\x3D\x00\x00\x00\x00", "xxxxx????"

		// x86 chromium:
		// must add + 0x5
		// 83 C4 04 89 3D
		// \x83\xC4\x04\x89\x3D\x00\x00\x00\x00", "xxxxx????"
		// 
		// x64 chromium:

		// 48 89 3D ? ? ? ? 66 0F 6E 87 ? ? ? ? 0F 5B C0 
		// \x48\x89\x3D\x00\x00\x00\x00\x66\x0F\x6E\x87\x00\x00\x00\x00\x0F\x5B\xC0, xxx????xxxx????xxx
#ifdef _WIN64
		pm_pPredictionPlayer = BTMemory::FindSignature("client.dll", "\x48\x89\x3D\x00\x00\x00\x00\x66\x0F\x6E\x87\x00\x00\x00\x00\x0F\x5B\xC0", "xxx????xxxx????xxx");
#else
		pm_pPredictionPlayer = BTMemory::FindSignature("client.dll", "\x83\xC4\x04\x89\x3D\x00\x00\x00\x00", "xxxxx????");
#endif

		// x86:
		// E8 ?? ?? ?? ?? 84 C0 0F 84 ?? ?? ?? ?? F3 0F 10 87
		// \xE8\x00\x00\x00\x00\x84\xC0\x0F\x84\x00\x00\x00\x00\xF3\x0F\x10\x87, x????xxxx????xxxx

		// x86 chromium:
		// E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? F3 0F 10 86 ? ? ? ? 0F 57 C9 0F 2F C1 
		// \xE8\x00\x00\x00\x00\x84\xC0\x0F\x84\x00\x00\x00\x00\xF3\x0F\x10\x86\x00\x00\x00\x00\x0F\x57\xC9\x0F\x2F\xC1, x????xxxx????xxxx????xxxxxx

		// x64 chromium:
		// E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 0F 29 B4 24 ? ? ? ? 0F 57 F6 0F 2F B3
		// \xE8\x00\x00\x00\x00\x84\xC0\x0F\x84\x00\x00\x00\x00\x0F\x29\xB4\x24\x00\x00\x00\x00\x0F\x57\xF6\x0F\x2F\xB3, x????xxxx????xxxx????xxxxxx
#ifdef _WIN64
		fnCBaseEntity_PhysicsRunThink = BTMemory::FindSignature("client.dll", "\xE8\x00\x00\x00\x00\x84\xC0\x0F\x84\x00\x00\x00\x00\x0F\x29\xB4\x24\x00\x00\x00\x00\x0F\x57\xF6\x0F\x2F\xB3", "x????xxxx????xxxx????xxxxxx");
#else
		fnCBaseEntity_PhysicsRunThink = BTMemory::FindSignature("client.dll", "\xE8\x00\x00\x00\x00\x84\xC0\x0F\x84\x00\x00\x00\x00\xF3\x0F\x10\x87", "x????xxxx????xxxx");
#endif
		// x86:
		// E8 ? ? ? ? 85 C0 7E 25 3B 87
		// "\xE8\x00\x00\x00\x00\x85\xC0\x7E\x25\x3B\x87", "x????xxxxxx"
		//
		// x86 chromium:
		// E8 ? ? ? ? 85 C0 7E 23 3B 87 ? ? ? ? 7F 1B 
		// \xE8\x00\x00\x00\x00\x85\xC0\x7E\x23\x3B\x87\x00\x00\x00\x00\x7F\x1B, x????xxxxxx????xx
		//
		// x64 chromium:
		// E8 ? ? ? ? 85 C0 7E 27 3B 87 ? ? ? ? 7F 1F 
		// \xE8\x00\x00\x00\x00\x85\xC0\x7E\x27\x3B\x87\x00\x00\x00\x00\x7F\x1F, x????xxxxxx????xx
#ifdef _WIN64
		fnCBaseEntity_GetNextThinkTick = BTMemory::FindSignature("client.dll", "\xE8\x00\x00\x00\x00\x85\xC0\x7E\x27\x3B\x87\x00\x00\x00\x00\x7F\x1F", "x????xxxxxx????xx");
#else
		fnCBaseEntity_GetNextThinkTick = BTMemory::FindSignature("client.dll", "\xE8\x00\x00\x00\x00\x85\xC0\x7E\x25\x3B\x87", "x????xxxxxx");
#endif

		// x86:
		// E8 ? ? ? ? 8B 46 08 8D 4E 08 C6
		// \xE8\x00\x00\x00\x00\x8B\x46\x08\x8D\x4E\x08\xC6\x86
		// x86 chromium:
		// E8 ? ? ? ? 8B 46 08 8D 4E 08 C6
		// \xE8\x00\x00\x00\x00\x8B\x46\x08\x8D\x4E\x08\xC6\x86
		// x64 chromium:
		// E8 ? ? ? ? 48 8B 07 48 8B CF FF 90 ? ? ? ? 48 8B 05 ? ? ? ? 4D 8B CC 
		// \xE8\x00\x00\x00\x00\x48\x8B\x07\x48\x8B\xCF\xFF\x90\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x4D\x8B\xCC, x????xxxxxxxx????xxx????xxx
#ifdef _WIN64
		fnCBaseEntity_SetNextThink = BTMemory::FindSignature("client.dll", "\xE8\x00\x00\x00\x00\x48\x8B\x07\x48\x8B\xCF\xFF\x90\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x4D\x8B\xCC", "x????xxxxxxxx????xxx????xxx");
#else
		fnCBaseEntity_SetNextThink = BTMemory::FindSignature("client.dll", "\xE8\x00\x00\x00\x00\x8B\x46\x08\x8D\x4E\x08\xC6\x86", "x????xxxxxxxx");
#endif

		// x86: 
		// 8B 91 ? ? ? ? B8 ? ? ? ? 56
		// x8B\x91\x00\x00\x00\x00\xB8\x00\x00\x00\x00\x56", "xx????x????x
		// 
		// x86 chromium:
		// x8B\x91\x00\x00\x00\x00\xB8\x00\x00\x00\x00\x56", "xx????x????x
		//
		// x64 chromium:
		// 8B 81 ? ? ? ? BA ? ? ? ? 83 E0 02 
		//\x8B\x81\x00\x00\x00\x00\xBA\x00\x00\x00\x00\x83\xE0\x02, xx????x????xxx
#ifdef _WIN64
		fnCBasePlayer_SetPlayerCollisionBounds = BTMemory::FindSignature("client.dll", "\x8B\x81\x00\x00\x00\x00\xBA\x00\x00\x00\x00\x83\xE0\x02", "xx????x????xxx");
#else
		fnCBasePlayer_SetPlayerCollisionBounds = BTMemory::FindSignature("client.dll", "\x8B\x91\x00\x00\x00\x00\xB8\x00\x00\x00\x00\x56", "xx????x????x");
#endif

		// x86:
		// must add 0x2;
		// 8B 0D ?? ?? ?? ?? 0F B7 F0 57 8B 04 F1
		// \x8B\x0D\x00\x00\x00\x00\x0F\xB7\xF0\x57\x8B\x04\xF1, xx????xxxxxxx
		//
		// x86 chromium:
		// must add 0x2;
		// 8B 0D ?? ?? ?? ?? 0F B7 F0 57 8B 04 F1
		// \x8B\x0D\x00\x00\x00\x00\x0F\xB7\xF0\x57\x8B\x04\xF1, xx????xxxxxxx
		// 
		// x64 chromium:
		// 48 8B 15 ? ? ? ? 66 0F 1F 44 00 ? 0F B7 D8 48 03 DB 
		// \x48\x8B\x15\x00\x00\x00\x00\x66\x0F\x1F\x44\x00\x00\x0F\xB7\xD8\x48\x03\xDB, xxx????xxxxx?xxxxxx
#ifdef _WIN64
		pDTStore = BTMemory::FindSignature("engine.dll", "\x48\x8B\x15\x00\x00\x00\x00\x66\x0F\x1F\x44\x00\x00\x0F\xB7\xD8\x48\x03\xDB", "xxx????xxxxx?xxxxxx");
#else 
		pDTStore = BTMemory::FindSignature("engine.dll", "\x8B\x0D\x00\x00\x00\x00\x0F\xB7\xF0\x57\x8B\x04\xF1", "xx????xxxxxxx");
#endif
		///////////////////////////////////////
		SignatureAssert(fnShotManipulator_ApplySpread);
		SignatureAssert(fnShotManipulator_constructor);
		SignatureAssert(pCVarSig);
		SignatureAssert(pMoveHelperClient);
		SignatureAssert(pGlobalVars);
		SignatureAssert(fnCPredictableIdHelper_Reset);
		SignatureAssert(pm_nPredictionRandomSeed);
		SignatureAssert(pm_pPredictionPlayer);
		SignatureAssert(fnCBaseEntity_PhysicsRunThink);
		SignatureAssert(fnCBaseEntity_GetNextThinkTick);
		SignatureAssert(fnCBaseEntity_SetNextThink);
		SignatureAssert(fnCBasePlayer_SetPlayerCollisionBounds);
		SignatureAssert(pDTStore);


		//
		SignatureAssert(pbSendPacket);
		///////////////////////////////////////
#ifndef _WIN64
		fnShotManipulator_constructor = fnShotManipulator_constructor - 0x2F;
#else 
		fnShotManipulator_constructor = fnShotManipulator_constructor;
#endif

#ifndef _WIN64
		pCVarSig = **(uintptr_t**)(pCVarSig + 0x2);
#else
		pCVarSig = *(uintptr_t*)GetRealFromRelative(pCVarSig, 3, 7, true);
#endif

#ifndef _WIN64
		pMoveHelperClient = **(uintptr_t**)(pMoveHelperClient + 0x2);
#else
		pMoveHelperClient = GetRealFromRelative(pMoveHelperClient, 3, 7, true);
#endif

#ifndef _WIN64
		pGlobalVars = **(uintptr_t**)(pGlobalVars + 0x1);
#else 
		pGlobalVars = *(uintptr_t*)GetRealFromRelative(pGlobalVars, 3, 7, true);
#endif

#ifndef _WIN64
		pbSendPacket = pbSendPacket + 0x3;
#else
		pbSendPacket = pbSendPacket + 0x2;
#endif
#ifndef  _WIN64
		fnCPredictableIdHelper_Reset = fnCPredictableIdHelper_Reset + *(uintptr_t*)(fnCPredictableIdHelper_Reset + 0x1) + 0x5;
#else 
		fnCPredictableIdHelper_Reset = GetRealFromRelative(fnCPredictableIdHelper_Reset, 1, 5, true);
#endif

#ifndef _WIN64
		pm_nPredictionRandomSeed = *(uintptr_t*)(pm_nPredictionRandomSeed + 0x4);
#else 
		pm_nPredictionRandomSeed = GetRealFromRelative(pm_nPredictionRandomSeed + 0x5, 2, 10, true);
#endif

#ifndef _WIN64
		pm_pPredictionPlayer = *(uintptr_t*)(pm_pPredictionPlayer + 0x5);
#else 
		pm_pPredictionPlayer = GetRealFromRelative(pm_pPredictionPlayer, 3, 7, true);
#endif

#ifndef _WIN64
		fnCBaseEntity_PhysicsRunThink = fnCBaseEntity_PhysicsRunThink + *(uintptr_t*)(fnCBaseEntity_PhysicsRunThink + 0x1) + 0x5;

#else
		fnCBaseEntity_PhysicsRunThink = GetRealFromRelative(fnCBaseEntity_PhysicsRunThink, 1, 5, true);
#endif

#ifndef  _WIN64
		fnCBaseEntity_GetNextThinkTick = fnCBaseEntity_GetNextThinkTick + *(uintptr_t*)(fnCBaseEntity_GetNextThinkTick + 0x1) + 0x5;
#else
		fnCBaseEntity_GetNextThinkTick = GetRealFromRelative(fnCBaseEntity_GetNextThinkTick, 1, 5, true);
#endif

#ifndef _WIN64
		fnCBaseEntity_SetNextThink = fnCBaseEntity_SetNextThink + *(uintptr_t*)(fnCBaseEntity_SetNextThink + 0x1) + 0x5;
#else 
		fnCBaseEntity_SetNextThink = GetRealFromRelative(fnCBaseEntity_SetNextThink, 1, 5, true);
#endif

#ifndef _WIN64
		pDTStore = **(uintptr_t**)(pDTStore + 0x2);
#else
		pDTStore = *(uintptr_t*)GetRealFromRelative(pDTStore, 3, 7, true);
#endif
	}
} SignatureManager;