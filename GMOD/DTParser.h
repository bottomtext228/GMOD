#pragma once

class DTValue {
public:
    const char* GetName() {
        return *(const char**)((uintptr_t)this + 0x0);
    }
    int GetOffset() {
        return *(int*)((uintptr_t)this + 0x2C);
    }
};

class DT {
public:
    void** m_pValues;
    int m_iValuesCount;
    int m_iZero;
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
        return (DTValue*)((uintptr_t)m_pValues + index * 0x3C);
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
        return (DTElement*)((uintptr_t)this + index * 8);
    }
};

namespace DTManager {
    DT* GetDTByName(const char* name) {
        static DTStore* g_pDTStore = **(DTStore***)(SignatureManager.pDTStore + 0x5);
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