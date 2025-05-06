#pragma once

std::array<std::string, 128> g_Nicknames;

namespace Utils {
	bool WorldToScreen(CVector pos, CVector2D& screen) {
		//Matrix-vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords
		CVector4D clipCoords{};
		clipCoords.x = pos.x * vars::viewMatrix[0] + pos.y * vars::viewMatrix[1] + pos.z * vars::viewMatrix[2] + vars::viewMatrix[3];
		clipCoords.y = pos.x * vars::viewMatrix[4] + pos.y * vars::viewMatrix[5] + pos.z * vars::viewMatrix[6] + vars::viewMatrix[7];
		clipCoords.z = pos.x * vars::viewMatrix[8] + pos.y * vars::viewMatrix[9] + pos.z * vars::viewMatrix[10] + vars::viewMatrix[11];
		clipCoords.w = pos.x * vars::viewMatrix[12] + pos.y * vars::viewMatrix[13] + pos.z * vars::viewMatrix[14] + vars::viewMatrix[15];

		if (clipCoords.w < 0.1f)
			return false;

		//perspective division, dividing by clip.W = Normalized Device Coordinates
		CVector NDC;
		NDC.x = clipCoords.x / clipCoords.w;
		NDC.y = clipCoords.y / clipCoords.w;
		NDC.z = clipCoords.z / clipCoords.w;

		screen.x = (int)((vars::resX / 2 * NDC.x) + (NDC.x + vars::resX / 2));
		screen.y = (int)(-(vars::resY / 2 * NDC.y) + (NDC.y + vars::resY / 2));
		return true;
	}

	inline float radiansToDegrees(float radians) {
		return (radians * (180 / M_PI));
	}

	inline float degreesToRadians(float degrees) {

		return (degrees * (M_PI / 180));
	}

	bool isPlayerInFriendList(int playerIndex) {
#ifdef DEBUG
		debug("isPlayerInFriendList() \n");
#endif // DEBUG

		if (vars::misc::ignoreLocalTeam) {
			CPed* playerPed = Interfaces.ClientEntityList->GetClientEntity(playerIndex);
			if (playerPed->GetTeamNum() == localPed->GetTeamNum()) // same team
				return true;
		}

		if (vars::misc::friendPlayers == 0) return false; // optimize it a bit

		char playerNick[128];
		Interfaces.Engine->GetPlayerNick(playerNick, playerIndex);
		for (int i = 0; i < vars::misc::friendPlayers; i++) {
			if (!strcmp(vars::misc::friendList[i], playerNick))
				return true;
		}
		return false;
	}

	bool isEntityInSpecificEntitiesList(const char* entityName) {
#ifdef DEBUG
		debug("isEntityInSpecificEntitiesList() \n");
#endif // DEBUG

		if (vars::misc::specificEntities == 0) return false; // optimize it a bit

		for (int i = 0; i < vars::misc::specificEntities; i++) {
			if (!strcmp(vars::misc::specificEntitiesList[i], entityName))
				return true;
		}
		return false;
	}

	void GetCornerBones(CPed* ped, int* cornerBones) {
#ifdef DEBUG
		debug("GetBones() \n");
#endif // DEBUG

		CVector2D lowestBoneScreenPos, highestBoneScreenPos,
			rightmostBoneScreenPos, leftmostBoneScreenPos;
		lowestBoneScreenPos = { 0, 0 };
		highestBoneScreenPos = { 0, vars::resY };


		leftmostBoneScreenPos = { vars::resX, 0 };
		rightmostBoneScreenPos = { 0, 0 };

		int lowestBone, highestBone,
			rightmostBone, leftmostBone;
		lowestBone = highestBone =
			rightmostBone = leftmostBone = -1;
		auto studioHdr = Interfaces.ModelInfo->GetStudiomodel(ped->GetModelInfo());

		for (int boneIndex = 0; boneIndex < studioHdr->numbones; boneIndex++)
		{
			auto bone = studioHdr->pBone(boneIndex);
			if (bone && bone->parent >= 0 && bone->flags & BONE_USED_BY_HITBOX)
			{
				CVector2D screenPos;
				CVector bonePos = ped->GetBonePosition(boneIndex);

				if (bonePos != CVector(0.0, 0.0, 0.0) && bonePos.DistanceTo(ped->GetVecOrigin()) < 150.0 && WorldToScreen(bonePos, screenPos)) {
					if (screenPos.y > lowestBoneScreenPos.y) {
						lowestBoneScreenPos.y = screenPos.y;
						lowestBone = boneIndex;
					}

					if (screenPos.y < highestBoneScreenPos.y) {
						highestBoneScreenPos.y = screenPos.y;
						highestBone = boneIndex;
					}


					if (screenPos.x < leftmostBoneScreenPos.x) {
						leftmostBoneScreenPos.x = screenPos.x;
						leftmostBone = boneIndex;
					}
					if (screenPos.x > rightmostBoneScreenPos.x) {
						rightmostBoneScreenPos.x = screenPos.x;
						rightmostBone = boneIndex;
					}

				}
			}
		}
		cornerBones[0] = lowestBone;
		cornerBones[1] = highestBone;
		cornerBones[2] = rightmostBone;
		cornerBones[3] = leftmostBone;

	}

	int BoneIndexByName(CPed* ped, char const* pName) {
#ifdef DEBUG
		debug("BoneIndexByName() \n");
#endif // DEBUG

		auto pStudioHdr = Interfaces.ModelInfo->GetStudiomodel(ped->GetModelInfo());

		for (int boneIndex = 0; boneIndex < pStudioHdr->numbones; boneIndex++) {
			auto bone = pStudioHdr->pBone(boneIndex);
			if (bone) {
				char boneName[64]; // can bone name be more then 64 symbols? :trollge:
				strcpy(boneName, bone->pszName());
				for (int i = 0; boneName[i]; i++) { // to lower case https://stackoverflow.com/a/2661788
					boneName[i] = tolower(boneName[i]);
				}
				if (strstr(boneName, pName)) // so we make case insenstive find now because of some bad models
					return boneIndex;
			}
		}
		return -1;
	}
	mstudiobbox_t* HitboxByBoneName(CPed* ped, char const* pName) {
		auto pStudioHdr = Interfaces.ModelInfo->GetStudiomodel(ped->GetModelInfo());
		auto set = pStudioHdr->pHitboxSet(0);
		for (int hitboxIndex = 0; hitboxIndex < set->numhitboxes; hitboxIndex++) {
			auto hitbox = set->pHitbox(hitboxIndex);
			auto bone = pStudioHdr->pBone(hitbox->bone);

			char boneName[64]; // can bone name be more then 64 symbols? :trollge:
			strcpy(boneName, bone->pszName());
			for (int i = 0; boneName[i]; i++) { // to lower case https://stackoverflow.com/a/2661788
				boneName[i] = tolower(boneName[i]);
			}
			if (strstr(boneName, pName)) // so we make case insenstive find now because of some bad models
				return hitbox;
		}
		return nullptr;
	}

	void SetupBones() {
#ifdef DEBUG
		debug("SetupBones() \n");
#endif // DEBUG

		static ULONGLONG ticks = 0;
		if (GetTickCount64() - ticks > vars::esp::setupBonesDelay) {
			ticks = GetTickCount64();
			for (int entityIndex = 0; entityIndex < Interfaces.GlobalVars->maxClients; entityIndex++) {
				CPed* ped = Interfaces.ClientEntityList->GetClientEntity(entityIndex);
				if (ped && ped->IsPlayer() && ped->IsAlive() && ped != localPed) {
					auto renderAble = ped->GetClientRenderable();
					if ((uintptr_t)renderAble > 0x1000) {// якобы предотвращает возможный краш
						renderAble->SetupBones(NULL, -1, BONE_USED_BY_HITBOX, Interfaces.GlobalVars->curtime);
					}
				}
			}
		}
	}

	CVector predictEntityOffsetPosition(CVector velocity) {
		/*  вычисление смещения из-за скорости персонажа, очень сильно повышает точность при быстром движении
			При вычислении смещения другого игрока-цели необходимо результат умножить на 0.5 (поделить на 2)
			((((подобрано экспериментально и вроде неплохо работает)))))
			При вычислении смещения локального васяна ничего делать не надо
																											*/
		return velocity * Interfaces.GlobalVars->interval_per_tick;
	}

	int GetPlayerIndexByPed(CPed* playerPed) { // ужас, так как цикл фор на куча итераций и только ради работы френдлиста
		for (int i = 0; i <= Interfaces.ClientEntityList->GetHighestEntityIndex(); i++) {
			CPed* ped = Interfaces.ClientEntityList->GetClientEntity(i);
			if (ped == playerPed)
				return i;
		}
		return -1;
	}

	const char* GetHostName() {
		auto LUA = Interfaces.LuaShared->GetLuaInterface(LuaInterfaceType::CLIENT);
		LUA->PushSpecial(CLuaInterface::SPECIAL_GLOB);
		LUA->GetField(-1, "GetHostName");
		LUA->Call(0, 1);
		const char* hostName = LUA->GetString(-1);
		LUA->Pop(2); // pop the global table and the return value
		return hostName;
	}

	std::string GetLuaNickname(int playerIndex) {
		// should be called only in the game thread (PaintTraverse, CreateMove, etc)

		auto Lua = Interfaces.LuaShared->GetLuaInterface(LuaInterfaceType::CLIENT);
		if (!Lua) return "";

		Lua->PushSpecial(CLuaInterface::SPECIAL_GLOB); // [Global{}]

		Lua->GetField(-1, "Entity"); // [Global{}, Entity()]

		Lua->PushNumber(playerIndex); // [Global{}, Entity(), playerIndex]

		Lua->Call(1, 1); // [Global{}, player]

		Lua->GetField(-1, "Nick");  // [Global{}, player, player.Nick()]

		if (!Lua->IsType(-1, LuaType::Function)) {
			Lua->Pop(3);
			return "";
		}

		Lua->Push(-2); // [Global{}, player, player.Nick(), player]

		Lua->Call(1, 1); // [Global{}, player, nick]

		const char* pNick = Lua->GetString(-1); // [Global{}, player, nick]

		std::string nick(pNick);

		Lua->Pop(3); // []


		return nick;
	}

	void UpdateLuaNicknames() {
		// gets player:Nick() value and stores it in g_Nicknames map. This is the only chance to get Lua game name.
		// should be called only in the game thread (PaintTraverse, CreateMove, etc)

		static ULONGLONG ticks = 0;
		constexpr ULONGLONG delay = 1000; // let's "optimize" it a bit
		if (GetTickCount64() - ticks > delay) {
			ticks = GetTickCount64();

			// entity indices reserved for players from 1 to maxClients
			for (int playerIndex = 1; playerIndex <= Interfaces.GlobalVars->maxClients; playerIndex++) {
				CPed* ped = Interfaces.ClientEntityList->GetClientEntity(playerIndex);
				if (ped && ped != localPed && ped->IsPlayer()) {
					g_Nicknames.at(playerIndex) = GetLuaNickname(playerIndex);
				}
			}
		}
	}


	CVector PointFromAngles(CVector& start, CVector& angle, float distance) {
		return {
				start.x + distance * cosf(Utils::degreesToRadians(angle.y)),
				start.y + distance * sinf(Utils::degreesToRadians(angle.y)),
				start.z - distance * tanf(Utils::degreesToRadians(angle.x))
		};
	}

	CVector GetPointOfSight(CVector& start, CVector& angle) {
		return PointFromAngles(start, angle, MAX_TRACE_LENGTH);
	}

	std::wstring utf8_to_utf16(const std::string& utf8) {
		if (utf8.empty()) return {};

		int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
		if (size_needed == 0) {
			throw std::runtime_error("MultiByteToWideChar failed");
		}

		std::wstring utf16(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &utf16[0], size_needed);

		// Remove null terminator added by MultiByteToWideChar
		utf16.pop_back();
		return utf16;
	}

	std::string sanitize_utf8_path(const std::string& path, char replacement = '_') {
		std::string sanitized;
		static const std::string invalid_chars_win = "<>:\"/\\|?*";
		static const std::string invalid_chars_posix = "/";

#ifdef _WIN32
		const std::string& invalid_chars = invalid_chars_win;
#else
		const std::string& invalid_chars = invalid_chars_posix;
#endif

		for (char c : path) {
			// Note: This only filters *ASCII* invalid characters. UTF-8 multi-byte chars are preserved.
			if (static_cast<unsigned char>(c) < 0x80 && invalid_chars.find(c) != std::string::npos) {
				sanitized += replacement;
			}
			else {
				sanitized += c;
			}
		}

		// Optional: trim trailing dots or spaces (Windows restriction)
#ifdef _WIN32
		while (!sanitized.empty() && (sanitized.back() == ' ' || sanitized.back() == '.')) {
			sanitized.pop_back();
		}
#endif

		return sanitized;
	}

}