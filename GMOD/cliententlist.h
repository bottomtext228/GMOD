class CClientEntityList
{

public:

	CPed* GetClientEntity(int Index) {
		typedef CPed* (__thiscall* Fn)(void*, int);
		return VMT.getvfunc<Fn>(this, 8)(this, Index);
	}
	int GetHighestEntityIndex() {
		typedef int(__thiscall* Fn)(void*);
		return  VMT.getvfunc<Fn>(this, 9)(this);
	}
	CPed* GetClientEntityFromHandle(HANDLE Handle) {
		typedef CPed* (__thiscall* Fn)(void*, HANDLE);
		return  VMT.getvfunc<Fn>(this, 10)(this, Handle);
	}
};