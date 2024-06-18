#pragma once


class CSignatureManager {
public:

	uintptr_t fnShotManipulator_ApplySpread;
	uintptr_t fnShotManipulator_constructor;
	uintptr_t pCVarSig;
	//uintptr_t pPlayerResource;
	
	CSignatureManager() {
		fnShotManipulator_ApplySpread = BTMemory::FindSignature("client.dll", "\x55\x8B\xEC\xF3\x0F\x10\x05\x00\x00\x00\x00\x83\xEC\x14\xF3\x0F\x10\x4D\x00\x0F\x2F\xC8\x56\x8B\xF1\x77\x08\x0F\x57\xC0\x0F\x2F\xC1", "xxxxxxx????xxxxxxx?xxxxxxxxxxxxxx");// reinterpret_cast<CShotManipulator_ApplySpread>(vars::client + 0x1515A0);
		fnShotManipulator_constructor = BTMemory::FindSignature("client.dll", "\x55\x8B\xEC\x8B\x4D\x08\x0F\x57\xDB\x8B\x55\x10\x56\xF3\x0F\x10\x01\x0F\x2E\xC3\x9F\xF6\xC4\x44\x7A\x49\xF3\x0F\x10\x41\x00\x0F\x2E\xC3", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx?xxx");//reinterpret_cast<CShotManipulator_constructor>(vars::client + 0x3B7A10);
		pCVarSig = BTMemory::FindSignature("engine.dll", "\x8B\x0D\x00\x00\x00\x00\x33\xC0\x89\x45\xF8\x83\xC4\x04\x89\x45\xF4", "xx????xxxxxxxxxxx");
		//pPlayerResource = BTMemory::FindSignature("client.dll", "\x89\x3D\x00\x00\x00\x00\x8B\xCF\x8B\x17\xFF\x75\x0C\xFF\x75\x08\xFF\x92\x00\x00\x00\x00\x33\xC0\x85\xFF\x0F\x44\xF0", "xx????xxxxxxxxxxxx????xxxxxxx");
		
		
		
		assert(fnShotManipulator_ApplySpread != NULL && "Signature is not found.");
		assert(fnShotManipulator_constructor != NULL && "Signature is not found.");
		assert(pCVarSig != NULL && "Signature is not found.");
		//assert(pPlayerResource != NULL && "Signature is not found.");
	}

} SignatureManager;