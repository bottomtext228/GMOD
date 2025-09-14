class VPanelWrapper
{
public:
	const char* GetName(uintptr_t VPanel) {
		typedef const char*(__thiscall* fn)(void*, uintptr_t);
		return VMT.getvfunc<fn>(this, 36)(this, VPanel);
	}
	
};