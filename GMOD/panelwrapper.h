class VPanelWrapper
{
public:
	const char* GetName(unsigned int VPanel) {
		typedef const char*(__thiscall* fn)(void*, unsigned int); // фиксим варнинг
		return VMT.getvfunc<fn>(this, 36)(this, VPanel);
	}
	
};