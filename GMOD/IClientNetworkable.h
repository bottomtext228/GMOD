#pragma once

class IClientNetworkable {
public:
	int GetEntityIndex() {
		typedef int(__thiscall* fn)(void*);
		return VMT.getvfunc<fn>(this, 9)(this);
	}
};