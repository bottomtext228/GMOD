#pragma once


class ILocalize {
public:
	const char* Find(const char* name) {
		// converts strings like #Valve_Hud_HEALTH to localized version used in HUD (!NOTE!: returned string is utf-16)
		typedef const char* (__thiscall* fn)(void*, const char*);
		return VMT.getvfunc<fn>(this, 2)(this, name);
	}
};
// sig: 
// BTMemory::FindSignature("client.dll", "\x8B\x0D\x00\x00\x00\x00\x8B\x01\xFF\x50\x08\x85\xC0\x8D\x4F\x2C\xBA\x00\x00\x00\x00\x0F\x45\xD0", "xx????xxxxxxxxxxx????xxx") + 2;