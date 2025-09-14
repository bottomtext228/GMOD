#pragma once

class DTValue {
public:
	const char* GetName() {
		return *(const char**)((uintptr_t)this + 0x0);
	}
	int GetOffset() {
#ifndef _WIN64
		return *(int*)((uintptr_t)this + 0x2C);
#else
		return *(int*)((uintptr_t)this + 0x48);
#endif
	}
};

class DT {
public:
	void** m_pValues;
#ifndef _WIN64
	int m_iValuesCount;
	int m_iZero;
#else
	int64_t m_iValuesCount;
	int64_t m_iZero;
#endif
	const char* m_pszName;
	DTValue* GetValueByName(const char* name) {
		for (int i = 0; i < m_iValuesCount; i++) {
			DTValue* value = GetValueByIndex(i);
			if (!strcmp(value->GetName(), name)) {
				return value;
			}
		}
		return nullptr;
	}
	DTValue* GetValueByIndex(int index) {
#ifndef _WIN64
		return (DTValue*)((uintptr_t)m_pValues + index * 0x3C);
#else
		return (DTValue*)((uintptr_t)m_pValues + index * 0x60);
#endif     
	}
};

class DTElement {
public:
	DT* m_DT;
	uint16_t m_unk;
	uint16_t m_index;
};

class DTStore {
public:
	DTElement* GetDTElementByIndex(int index) {
#ifndef _WIN64
		return (DTElement*)((uintptr_t)this + index * 8);
#else
		return (DTElement*)((uintptr_t)this + index * 0x10);
#endif
	}
};

namespace DTManager {
	DT* GetDTByName(const char* name) {
		static DTStore* g_pDTStore = (DTStore*)(SignatureManager.pDTStore);
		int i = 0;

		while (true) {
			auto pDTElement = g_pDTStore->GetDTElementByIndex(i);
			if (!_stricmp(pDTElement->m_DT->m_pszName, name))
				return pDTElement->m_DT;
			if (pDTElement->m_index == 0xFFFF)
				return nullptr;
			i++;
		}
		return nullptr;

	}
	uintptr_t GetOffset(const char* tableName, const char* valueName) {
		auto dt = GetDTByName(tableName);
		if (!dt) return 0;
		auto value = dt->GetValueByName(valueName);
		if (!value) return 0;

		return value->GetOffset();
	}
}