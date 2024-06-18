
#pragma once
#include <Windows.h>
#include <vector>


namespace BTMemory {
	namespace Hooker {
		enum class HookType {
			DETOUR, // jmp 
			TRAMPOLINE, // jmp with trampoline
#ifndef _WIN64
			REDIRECT // call | nearly pointless in x64 
#endif
		};
		class CHook {
			const uintptr_t m_fnHookCallback;
			const uintptr_t m_pToHook;
			uint32_t m_fnOriginal = 0;
			BYTE* m_pOriginalBytes = 0;
			BYTE* m_pPrologueBytes = 0;
			size_t m_uHookSize;
			bool m_bHookAbove = false;
#ifdef _WIN64
			uintptr_t m_pInterlayer = 0;
#endif // _WIN64

			const HookType m_hookType;
			static std::vector<CHook*> pHooks;
		public:
			CHook(void* fnToHook, void* fnHookCallback, HookType hookType, size_t hookSize) :
				m_pToHook((uintptr_t)fnToHook), m_fnHookCallback((uintptr_t)fnHookCallback), m_hookType(hookType), m_uHookSize(hookSize) {
			}

			// HookType REDIRECT or TRAMPOLINE -> return value is the original function | DETOUR -> return value is nullptr
			void* ApplyHook() {
				if (!this) {
					return nullptr;
				}
				switch (m_hookType) {
#ifndef _WIN64
				case HookType::REDIRECT:
					return InstallRedirectHook();
					break;
#endif
				case HookType::TRAMPOLINE:
					return InstallTrampolineHook();
					break;
				case HookType::DETOUR:
					InstallDetourHook();
					break;

				}
				return nullptr;

			}
			void DestroyHook() {
#ifndef _WIN64
				if (m_hookType == HookType::REDIRECT) {
					DWORD oldProtect;
					VirtualProtect((void*)m_pToHook, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
					*reinterpret_cast<uint32_t*>(m_pToHook + 1) = (uint32_t)m_fnOriginal; // restore the original relative address 
					VirtualProtect((void*)m_pToHook, 5, oldProtect, &oldProtect);
				}
#endif
				if (m_hookType == HookType::TRAMPOLINE || m_hookType == HookType::DETOUR) { // both have similar working method
					DWORD oldProtect;
					VirtualProtect((void*)m_pToHook, m_uHookSize, PAGE_EXECUTE_READWRITE, &oldProtect);
					if (m_bHookAbove) {
						*reinterpret_cast<uint32_t*>(m_pToHook + 1) = m_fnOriginal; //  // restore the original relative address  (same method for x86/x64)
					}
					else {
						memcpy((void*)m_pToHook, m_pOriginalBytes, m_uHookSize); // restore the original code of the function
					}
					VirtualProtect((void*)m_pToHook, m_uHookSize, oldProtect, &oldProtect);
					VirtualFree(m_pOriginalBytes, NULL, MEM_RELEASE);  // delete allocated memory for original code/trampoline
#ifdef _WIN64
					VirtualFree((void*)m_pInterlayer, NULL, MEM_RELEASE);
#endif
					if (m_pPrologueBytes) {
						VirtualFree(m_pPrologueBytes, NULL, MEM_RELEASE);
					}

				}
				auto index = std::find(pHooks.begin(), pHooks.end(), this); // if we are destroying the hook we should erase him from the hooks array 
				if (index != pHooks.end()) {
					pHooks.erase(index);
					delete this;
				}


			}
		private:

#ifdef _WIN64


			inline uintptr_t findNextFreeMemory(std::uintptr_t from, std::uintptr_t to, std::uintptr_t granularity) {

				from -= from % granularity; // alignment
				from += granularity;
				while (from <= to) {
					MEMORY_BASIC_INFORMATION mbi;
					if (VirtualQuery(reinterpret_cast<void*>(from), &mbi, sizeof(mbi)) == 0) break;
					if (mbi.State == MEM_FREE) return from;
					if (reinterpret_cast<std::uintptr_t>(mbi.AllocationBase) < granularity) break;
					from = reinterpret_cast<std::uintptr_t>(mbi.AllocationBase) + mbi.RegionSize;

					from += granularity - 1;
					from -= from % granularity;
				}
				return 0;
			}

			inline uintptr_t findPrevFreeMemory(std::uintptr_t from, std::uintptr_t to, std::uintptr_t granularity) {
				to -= to % granularity; // alignment
				to -= granularity;
				while (from < to) {
					MEMORY_BASIC_INFORMATION mbi;
					if (VirtualQuery(reinterpret_cast<void*>(to), &mbi, sizeof(mbi)) == 0) break;
					if (mbi.State == MEM_FREE) return to;
					if (reinterpret_cast<std::uintptr_t>(mbi.AllocationBase) < granularity) break;
					to = reinterpret_cast<std::uintptr_t>(mbi.AllocationBase) - granularity;
				}
				return 0;
			}

			void* getFreeMemoryForJmp(uintptr_t address) {
				// credit to kin4stat
				constexpr auto kMaxMemoryRange = 0x40000000; // 1gb
				constexpr auto kMemoryBlockSize = 0x1000;    // windows page size
				SYSTEM_INFO si;
				GetSystemInfo(&si);
				std::uintptr_t min_address = reinterpret_cast<std::uintptr_t>(si.lpMinimumApplicationAddress);
				std::uintptr_t max_address = reinterpret_cast<std::uintptr_t>(si.lpMaximumApplicationAddress);

				if (kMaxMemoryRange <= address && min_address < address - kMaxMemoryRange) min_address = address - kMaxMemoryRange;

				if (address + kMaxMemoryRange <= max_address) max_address = address + kMaxMemoryRange;

				// Make room for one page
				max_address -= kMemoryBlockSize - 1;

				void* result = nullptr;
				{
					std::uintptr_t alloc = address;
					while (min_address <= alloc) {
						alloc = findPrevFreeMemory(min_address, alloc, si.dwAllocationGranularity);
						if (alloc == 0) break;

						result = VirtualAlloc(reinterpret_cast<void*>(alloc), kMemoryBlockSize, MEM_COMMIT | MEM_RESERVE,
							PAGE_EXECUTE_READWRITE);
						if (result != nullptr) break;
					}
				}
				if (result == nullptr) {
					std::uintptr_t alloc = address;
					while (alloc <= max_address) {
						alloc = findNextFreeMemory(alloc, max_address, si.dwAllocationGranularity);
						if (alloc == 0) break;

						result = VirtualAlloc(reinterpret_cast<void*>(alloc), kMemoryBlockSize, MEM_COMMIT | MEM_RESERVE,
							PAGE_EXECUTE_READWRITE);
						if (result != nullptr) break;
					}
				}
				return result;

			}

			bool isAddressExecutable(uintptr_t address) {
				MEMORY_BASIC_INFORMATION mbi;
				if (!VirtualQuery((void*)address, &mbi, sizeof(mbi))) return false;
				if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) return false;
				return (mbi.Protect & PAGE_EXECUTE_READWRITE);
			}

			int64_t SignExtend(uint32_t n) {
				// to calculate absolute address from relative: RIP = RIP + 32-bit displacement sign extended to 64-bits
				return (int64_t)n | 0xFFFFFFFF00000000;
			}


#endif // _WIN64

			void* InstallTrampolineHook() {

				InstallDetourHook();
#ifdef _WIN64

				uintptr_t jumpBackOffset = m_uHookSize;

				if (m_bHookAbove) {
					jumpBackOffset += 9; // 14 - 5

				}

				*reinterpret_cast<WORD*>(m_pOriginalBytes + jumpBackOffset) = 0x25FF;
				*reinterpret_cast<uint32_t*>(m_pOriginalBytes + jumpBackOffset + 2) = 0x0;
				*reinterpret_cast<uintptr_t*>(m_pOriginalBytes + jumpBackOffset + 6) = m_pToHook + m_uHookSize; // jmp to original function 
#else

				* reinterpret_cast<BYTE*>(m_pOriginalBytes + m_uHookSize) = 0xE9;
				*reinterpret_cast<uintptr_t*>(m_pOriginalBytes + m_uHookSize + 1) = m_pToHook - (uintptr_t)m_pOriginalBytes - m_uHookSize; // jmp to original function 
#endif

				return m_pOriginalBytes; // return trampoline 				
			}
#ifndef _WIN64
			void* InstallRedirectHook() {

				m_fnOriginal = *reinterpret_cast<uint32_t*>(m_pToHook + 1); // get original relative address 
				DWORD oldProtect;
				VirtualProtect((void*)m_pToHook, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
				uint32_t relativeAddress = (uint32_t)(m_fnHookCallback - m_pToHook - 5);
				*reinterpret_cast<uint32_t*>(m_pToHook + 1) = relativeAddress;
				VirtualProtect((void*)m_pToHook, 5, oldProtect, &oldProtect);

				return (void*)(m_pToHook + m_fnOriginal + 5);
			}
#endif




			void InstallDetourHook() {
				size_t memoryToNop = m_uHookSize;
				DWORD oldProtect;

				m_pOriginalBytes = (BYTE*)VirtualAlloc(0, m_uHookSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
				if (!m_pOriginalBytes) {
					return; // sad
				}

				VirtualProtect((void*)m_pToHook, m_uHookSize, PAGE_EXECUTE_READWRITE, &oldProtect);
				memcpy(m_pOriginalBytes, (void*)m_pToHook, m_uHookSize); // copy original bytes

				if (*reinterpret_cast<BYTE*>(m_pToHook) == 0xE9) { // was hooked
					m_bHookAbove = true;
					uint32_t relativeAddress = *reinterpret_cast<uint32_t*>(m_pToHook + 1);
					m_fnOriginal = relativeAddress;
#if _WIN64
					uintptr_t absoluteAddress = m_pToHook + SignExtend(relativeAddress) + 5;
					*reinterpret_cast<WORD*>(m_pOriginalBytes) = 0x25FF;
					*reinterpret_cast<uint32_t*>(m_pOriginalBytes + 2) = 0x0;
					*reinterpret_cast<uintptr_t*>(m_pOriginalBytes + 6) = absoluteAddress;

#else	
					uintptr_t absoluteAddress = m_pToHook + relativeAddress + 5;
					*reinterpret_cast<BYTE*>(m_pOriginalBytes) = 0xE9;
					*reinterpret_cast<uint32_t*>(m_pOriginalBytes + 1) = absoluteAddress - (uintptr_t)m_pOriginalBytes - 5;
#endif // _WIN64
				}


				memset((void*)(m_pToHook), 0x90, memoryToNop); // fill with nops in case if we replace some bytes

#ifdef _WIN64

				// TODO: check with multiple dlls
				auto freeMemory = (uintptr_t)getFreeMemoryForJmp(m_pToHook);


				if (freeMemory) {

					m_pInterlayer = freeMemory;

					// place 5 bytes jump to the interlayer with 14 long bytes jump
					*reinterpret_cast<BYTE*>(m_pToHook) = 0xE9;
					*reinterpret_cast<uint32_t*>(m_pToHook + 1) = (uint32_t)(m_pInterlayer - m_pToHook - 5);

					*reinterpret_cast<WORD*>(m_pInterlayer) = 0x25FF;
					*reinterpret_cast<uint32_t*>(m_pInterlayer + 2) = 0x0;
					*reinterpret_cast<uintptr_t*>(m_pInterlayer + 6) = m_fnHookCallback;
				}
				else {
					// place long 14 bytes jump 
					/*
					* if we couldn't find memory for 5 byte jmp. This won't be work actually and it will crush because for example, we set hook size for 5, but we coudn't find free memory
					so hook is 14 size now, but we can (90%) brake assembler listing after that. So, what to do with that?
					*/
					*reinterpret_cast<WORD*>(m_pToHook) = 0x25FF;
					*reinterpret_cast<uint32_t*>(m_pToHook + 2) = 0x0;
					*reinterpret_cast<uintptr_t*>(m_pToHook + 6) = m_fnHookCallback;
				}
#else
				* reinterpret_cast<BYTE*>(m_pToHook) = 0xE9;
				*reinterpret_cast<uint32_t*>(m_pToHook + 1) = m_fnHookCallback - m_pToHook - 5;
#endif // WIN64

				VirtualProtect((void*)m_pToHook, m_uHookSize, oldProtect, &oldProtect);
			}
			friend CHook* Hook(void*, void*, HookType, size_t);
			friend void UnhookAll();
		};
		std::vector<CHook*> CHook::pHooks;
		// ApplyHook() & DestroyHook() | in x86 minimal hook size is 5 bytes, in x64 with some tricks too
		CHook* Hook(void* fnToHook, void* fnHookCallback, HookType hookType, size_t hookSize = 5) {
			constexpr size_t minimalHookSize = 5; // 0xE9 + 4 byte address
			if (hookSize < minimalHookSize) {
				return nullptr; // maybe assert()?
			}
			CHook* hook = new CHook(fnToHook, fnHookCallback, hookType, hookSize);
			CHook::pHooks.push_back(hook);
			return hook;
		}
		void UnhookAll() {
			for (auto& hook : CHook::pHooks) {
				hook->DestroyHook();
			}
		}
	};
	namespace VMTHooker {
		class CVMTHook {
			void* m_pOriginalVMT;
			void** m_pVMT;
			const int m_iHookedMethod;
			void* m_fnHookFunction;
			static std::vector<CVMTHook*> pVMTHooks;

		public:
			void* ApplyHook() {
				void* oldMethod = m_pVMT[m_iHookedMethod];
				DWORD oldProtect;
				VirtualProtect(m_pVMT + m_iHookedMethod, sizeof(uintptr_t), PAGE_EXECUTE_READWRITE, &oldProtect);
				m_pVMT[m_iHookedMethod] = m_fnHookFunction; // changing the pointer to the method
				VirtualProtect(m_pVMT + m_iHookedMethod, sizeof(uintptr_t), oldProtect, &oldProtect);
				return oldMethod;
			}
			void DestroyHook() {
				DWORD oldProtect;
				VirtualProtect(m_pVMT + m_iHookedMethod, sizeof(uintptr_t), PAGE_EXECUTE_READWRITE, &oldProtect);
				m_pVMT[m_iHookedMethod] = m_pOriginalVMT; // restoring the pointer
				VirtualProtect(m_pVMT + m_iHookedMethod, sizeof(uintptr_t), oldProtect, &oldProtect);
				auto index = std::find(pVMTHooks.begin(), pVMTHooks.end(), this); // if we are destroying the hook we should erase him from the hooks array 
				if (index != pVMTHooks.end()) {
					pVMTHooks.erase(index);
					delete this;
				}
			}
		private:
			CVMTHook(void* pVMT, int iMethodIndex, void* fnHook) :
				m_pVMT((void**)pVMT), m_pOriginalVMT(((void**)pVMT)[iMethodIndex]), m_iHookedMethod(iMethodIndex), m_fnHookFunction(fnHook) {
			}

			friend CVMTHook* Hook(void*, int, void*);
			friend void UnhookAll();
		};
		std::vector<CVMTHook*> CVMTHook::pVMTHooks;
		// ApplyHook() & DestroyHook() 
		CVMTHook* Hook(void* pVMT, int iMethodIndex, void* fnHook) {
			CVMTHook* hook = new CVMTHook(pVMT, iMethodIndex, fnHook);
			CVMTHook::pVMTHooks.push_back(hook);
			return hook;
		}
		void UnhookAll() {
			for (auto& hook : CVMTHook::pVMTHooks) {
				hook->DestroyHook();
			}
		}

	};


	namespace Patcher {
		class CPatch {
			BYTE* m_pOriginalBytes = 0;
			BYTE* m_pPatchBytes;
			const size_t m_uPatchSize;
			const LPVOID m_lpDestination;
			bool m_bIsNop = false;
			static std::vector<CPatch*> pPatches;
		public:
			// actually patching
			inline void ApplyPatch() {
				if (m_bIsNop) {
					Nop(m_lpDestination, m_uPatchSize); // fill with nops...
				}
				else {
					Patch(m_lpDestination, m_pPatchBytes, m_uPatchSize); // ...or with patch bytes
				}
			}
			// restore original bytes
			inline void RestorePatch() {
				Patch(m_lpDestination, m_pOriginalBytes, m_uPatchSize);
			}
			// restore original bytes and delete patch
			inline void DestroyPatch() {
				RestorePatch();
				VirtualFree(m_pOriginalBytes, NULL, MEM_RELEASE); // seems to be no memory leak :trolling:
				VirtualFree(m_pPatchBytes, NULL, MEM_RELEASE);

				auto index = std::find(pPatches.begin(), pPatches.end(), this); // if we are destroying the patch we should erase him from the patch array 
				if (index != pPatches.end()) {
					pPatches.erase(index);
					delete this;
				}
			}
		private:
			CPatch(void* destination, size_t patchSize, const void* patchBytes = 0)
				: m_lpDestination((void*)destination), m_uPatchSize(patchSize) {
				if (patchBytes) {
					m_pPatchBytes = (BYTE*)VirtualAlloc(0, m_uPatchSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
					if (!m_pPatchBytes) {
						return; // sad
					}
					memcpy(m_pPatchBytes, patchBytes, m_uPatchSize);
				}
				else {
					m_bIsNop = true; // we don't need to save patch bytes because we just fill with nops
				}
				CopyOriginBytes();
			}

			static void Patch(void* destination, const void* patchBytes, size_t patchSize) {
				DWORD oldProtect;
				VirtualProtect(destination, patchSize, PAGE_EXECUTE_READWRITE, &oldProtect);
				memcpy(destination, patchBytes, patchSize);
				VirtualProtect(destination, patchSize, oldProtect, &oldProtect);
			}
			static void Nop(void* destination, size_t patchSize) {
				DWORD oldProtect;
				VirtualProtect(destination, patchSize, PAGE_EXECUTE_READWRITE, &oldProtect);
				memset(destination, 0x90, patchSize);
				VirtualProtect(destination, patchSize, oldProtect, &oldProtect);
			}
			friend CPatch* Patch(uintptr_t, const void*, size_t, bool);
			friend CPatch* Nop(uintptr_t, size_t, bool);
			friend void UnpatchAll();

			void CopyOriginBytes() {
				m_pOriginalBytes = (BYTE*)VirtualAlloc(0, m_uPatchSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
				if (!m_pOriginalBytes) {
					return; // sad
				}
				memcpy(m_pOriginalBytes, m_lpDestination, m_uPatchSize);
			}

		};
		std::vector<CPatch*> CPatch::pPatches;

		// ApplyPatch() & RestorePatch() | isSimple = true -> immediate apply/no saving bytes, no restore ( including UnpatchAll() )
		CPatch* Patch(uintptr_t destination, const void* patchBytes, size_t patchSize, bool isSimple = false) {
			if (isSimple) {
				CPatch::Patch((void*)destination, patchBytes, patchSize);
				return nullptr;
			}
			else {
				CPatch* patch = new CPatch((void*)destination, patchSize, patchBytes);
				CPatch::pPatches.push_back(patch);
				return patch;
			}
		}

		// ApplyPatch() & RestorePatch() | isSimple = true -> immediate apply/no saving bytes, no restore ( including UnpatchAll() )
		CPatch* Nop(uintptr_t destination, size_t patchSize, bool isSimple = false) {
			if (isSimple) {
				CPatch::Nop((void*)destination, patchSize);
				return nullptr;
			}
			else {
				CPatch* patch = new CPatch((void*)destination, patchSize, 0);
				CPatch::pPatches.push_back(patch);
				return patch;
			}
		}
		void UnpatchAll() {
			for (auto& patch : CPatch::pPatches) {
				patch->DestroyPatch();
			}
		}
	};

	uintptr_t FindSignature(const char* moduleName, const void* signatureString, const char* mask) {

		unsigned char* signature = (unsigned char*)signatureString; // something is trolling us
		HMODULE moduleHandle = GetModuleHandleA(moduleName);
		if (!moduleHandle) {
			return NULL;
		}

		uintptr_t moduleBaseAddress = (uintptr_t)moduleHandle;


		uintptr_t moduleEndAddress = moduleBaseAddress + reinterpret_cast<IMAGE_NT_HEADERS*>
			(moduleBaseAddress + reinterpret_cast<IMAGE_DOS_HEADER*>(moduleBaseAddress)->e_lfanew)->OptionalHeader.SizeOfImage;

		for (uintptr_t address = moduleBaseAddress; address < moduleEndAddress; address++) {
			unsigned int signatureIterator = 0;
			while (mask[signatureIterator] != '\0') {
				if (mask[signatureIterator] != '?' && *reinterpret_cast<BYTE*>(address + signatureIterator) != signature[signatureIterator]) {
					break;
				}
				signatureIterator++;
			}
			if (!mask[signatureIterator]) {
				return address;
			}
		}


		return 0;
	}


	uintptr_t FindDMAAddy(uintptr_t baseAddress, std::vector<unsigned int> offsets) {
		for (auto offset : offsets) {
			baseAddress = *reinterpret_cast<uintptr_t*>(baseAddress);
			baseAddress += offset;
		}
		return baseAddress;
	}

	// undone all hooks (VMT / JMP etc.)/patches
	void UnpatchAll() {
		Patcher::UnpatchAll();
		VMTHooker::UnhookAll();
		Hooker::UnhookAll();
	}

}
