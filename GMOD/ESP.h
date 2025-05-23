
class CESP {
public:

	void Process() {
		if (!localPed) return;

		if (vars::esp::boneESP || vars::esp::linesState || vars::esp::box3DESP ||
			vars::esp::box2DESP || vars::esp::renderHealth || vars::esp::renderGameNick
			|| vars::esp::renderSteamNick || vars::esp::renderCurrentWeapon || vars::esp::renderWeapons || vars::esp::renderEntity) { // ����� ������ ���� �� ���������� EntityList ������ ��� 

			// if render entities we should do for loop until highest entity index, if only players we should use maxClients global var

			int maxIndex = vars::esp::renderEntity ? Interfaces.ClientEntityList->GetHighestEntityIndex() : Interfaces.GlobalVars->maxClients;

			for (int entityIndex = 0; entityIndex <= maxIndex; entityIndex++) {

				CPed* ped = Interfaces.ClientEntityList->GetClientEntity(entityIndex);
				if (ped) {

					if (vars::esp::dormantCheck && ped->IsDormant()) continue;
					if (vars::esp::distanceCheck && ped->GetVecOrigin().DistanceTo(localPed->GetVecOrigin()) > vars::esp::distance) continue;

					if (ped != localPed && ped->IsAlive() && ped->IsPlayer() && ped->GetModelInfo()) {

						// do not draw on spectated player
						if (Interfaces.ClientEntityList->GetClientEntityFromHandle(localPed->GetObserverTarget()) == ped) continue;
						
						if (vars::esp::box3DESP) {
							DrawBoxESP(ped, entityIndex);
						}

						if (vars::esp::linesState) {
							DrawLines(ped, entityIndex);
						}

						if (vars::esp::boneESP) {
							DrawBoneESP(ped, entityIndex);
						}

						if (vars::esp::renderHealth || vars::esp::renderGameNick || vars::esp::renderSteamNick || vars::esp::renderCurrentWeapon) {
							DrawPlayerInfo(ped, entityIndex);
						}

						if (vars::esp::box2DESP) {
							Draw2DBoxESP(ped, entityIndex);
						}
					}
					if (vars::esp::renderEntity) {
						DrawEntities(ped);
					}

				} // end of if (ped)

			}


		}
	}

private:
	void DrawLines(CPed* ped, int playerIndex) {
#ifdef DEBUG
		debug("DrawLines() \n");
#endif // DEBUG

		CVector pos = ped->GetVecOrigin();
		pos.z += 78;

		CVector2D screenPos;
		if (Utils::WorldToScreen(pos, screenPos)) {
			ImDrawList* draw = ImGui::GetBackgroundDrawList();
			draw->AddLine(ImVec2((float)vars::resX / 2.0f, 0.0f), screenPos.ToImVec2(), calcESPColor(ped->GetHealth(), playerIndex));
		}
	}
	void DrawPlayerInfo(CPed* ped, int playerIndex) {
#ifdef DEBUG
		debug("DrawPlayerInfo()\n");
#endif

		CVector pos = ped->GetVecOrigin();
		pos.z += 80;

		CVector2D screenPos;
		if (!Utils::WorldToScreen(pos, screenPos))
			return;

		std::string espText;

		if (vars::esp::renderHealth) {
			if (vars::esp::renderMaxHealth) espText += "[" + std::to_string(ped->GetHealth()) + "/" + std::to_string(ped->GetMaxHealth()) + "]\n";
			else espText += "[" + std::to_string(ped->GetHealth()) + "]\n";
		}

		char playerNick[128] = {};
		Interfaces.Engine->GetPlayerNick(playerNick, playerIndex);
		std::string nickStr(playerNick);

		if (vars::esp::renderGameNick) {
			auto& gameNick = g_Nicknames.at(playerIndex);
			if (!gameNick.empty()) espText += gameNick + "\n";
		}

		if (vars::esp::renderSteamNick) {
			espText += nickStr + "\n";
		}

		if (vars::esp::renderCurrentWeapon) {
			auto weapon = (C_BaseCombatWeapon*)Interfaces.ClientEntityList->GetClientEntityFromHandle(ped->GetActiveWeaponHandle());
			if (weapon)
				espText += std::string(weapon->GetName()) + "\n";
		}

		if (vars::esp::renderWeapons) {
			for (int i = 0; i < 256; i++) {
				auto weaponHandle = ped->GetWeaponHandle(i);
				if (weaponHandle != -1) {
					auto weapon = (C_BaseCombatWeapon*)Interfaces.ClientEntityList->GetClientEntityFromHandle(weaponHandle);
					if (weapon)
						espText += std::string(weapon->GetName()) + "\n";
				}
			}
		}

		if (!espText.empty()) {
			ImVec2 textSize = ImGui::CalcTextSize(espText.c_str());
			ImVec2 textPos(screenPos.x - textSize.x / 2, screenPos.y - textSize.y);
			ImDrawList* draw = ImGui::GetBackgroundDrawList();

			// Optional: text shadow
			draw->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 255), espText.c_str());
			draw->AddText(textPos, IM_COL32(255, 255, 255, 255), espText.c_str());
		}
	}
	void DrawEntities(CPed* entity) {
#ifdef DEBUG
		debug("DrawEntities()\n");
#endif // DEBUG

		if (entity->GetTeamNum() == 0 && entity->GetModelInfo()) {

			ImU32 espColor = 0xFFFF7300;
			if (Utils::isEntityInSpecificEntitiesList(entity->GetModelInfo()->m_ModelName)) {
				espColor = 0xFF00FF22; // 0073FF  22FF00 <- rgb | abgr -> FFFF7300 FF00FF22

			}
			else {
				if (vars::esp::renderSpecificEntities)
					return;
			}
			CVector2D entityPos;
			if (Utils::WorldToScreen(entity->GetVecOrigin(), entityPos)) {
				ImDrawList* draw = ImGui::GetBackgroundDrawList();
				ImVec2 textSize = ImGui::CalcTextSize(entity->GetModelInfo()->m_ModelName);
				draw->AddText(ImVec2(entityPos.x - textSize.x / 2, entityPos.y - textSize.y), espColor, entity->GetModelInfo()->m_ModelName);

			}
		}
	}
	void Draw2DBoxESP(CPed* ped, int playerIndex) {
#ifdef DEBUG
		debug("Draw2DBoxESP() \n");
#endif // DEBUG

		int lowestBone, highestBone,
			rightmostBone, leftmostBone;
		int cornerBones[4];
		Utils::GetCornerBones(ped, cornerBones);
		lowestBone = cornerBones[0];
		highestBone = cornerBones[1];
		rightmostBone = cornerBones[2];
		leftmostBone = cornerBones[3];
		if (lowestBone == -1 || highestBone == -1 || rightmostBone == -1 || leftmostBone == -1)
			return;
		CVector lowestBonePos = ped->GetBonePosition(lowestBone);
		CVector highestBonePos = ped->GetBonePosition(highestBone);
		CVector rightmostBonePos = ped->GetBonePosition(rightmostBone);
		CVector leftmostBonePos = ped->GetBonePosition(leftmostBone);

		CVector2D lowestBoneScreenPos, highestBoneScreenPos,
			rightmostBoneScreenPos, leftmostBoneScreenPos;
		Utils::WorldToScreen(lowestBonePos, lowestBoneScreenPos);
		Utils::WorldToScreen(highestBonePos, highestBoneScreenPos);
		Utils::WorldToScreen(rightmostBonePos, rightmostBoneScreenPos);
		Utils::WorldToScreen(leftmostBonePos, leftmostBoneScreenPos);


		ImDrawList* draw = ImGui::GetBackgroundDrawList();
		draw->AddRect(
			ImVec2{ (float)rightmostBoneScreenPos.x, (float)highestBoneScreenPos.y },
			ImVec2{ (float)leftmostBoneScreenPos.x, (float)lowestBoneScreenPos.y },
			calcESPColor(ped->GetHealth(), playerIndex)
		);



	}

	void DrawBoneESP(CPed* ped, int playerIndex) {
#ifdef DEBUG
		debug("DrawBoneESP() \n");
#endif // DEBUG


		ImU32 espColor = calcESPColor(ped->GetHealth(), playerIndex);

		assert(ped->GetModelInfo() != NULL);
		auto studioHdr = Interfaces.ModelInfo->GetStudiomodel(ped->GetModelInfo());
		assert(studioHdr != NULL);
		for (int boneIndex = 0; boneIndex < studioHdr->numbones; boneIndex++)
		{
			auto bone = studioHdr->pBone(boneIndex);
			if (bone && bone->parent >= 0 && bone->flags & 256)
			{

				CVector normalBonePos = ped->GetBonePosition(boneIndex);
				CVector normalParentBonePos = ped->GetBonePosition(bone->parent);

				if (normalBonePos == CVector(0, 0, 0) || normalParentBonePos == CVector(0, 0, 0)
					|| normalBonePos.DistanceTo(ped->GetVecOrigin()) > 150.0f || normalParentBonePos.DistanceTo(ped->GetVecOrigin()) > 150.f)
					continue;

				CVector2D bonePosFrom;
				CVector2D parentBonePos;
				if (!Utils::WorldToScreen(normalBonePos, bonePosFrom) || !Utils::WorldToScreen(normalParentBonePos, parentBonePos))
					continue;
				ImDrawList* draw = ImGui::GetBackgroundDrawList();
				draw->AddLine(bonePosFrom.ToImVec2(), parentBonePos.ToImVec2(), espColor, 1);

			}
		}
	}

	void DrawBoxESP(CPed* ped, int playerIndex) {
#ifdef DEBUG
		debug("DrawBox ESP \n");
#endif // DEBUG

		CVector pos = ped->GetVecOrigin();
		CVector2D screenPos[8]{};
		CVector corners[8];

		/*  https://cdn.discordapp.com/attachments/768531187110510602/959526118845743215/unknown.png */
		enum ESPCorners {
			DOWN_LEFT_NEAR,
			DOWN_RIGHT_NEAR,
			DOWN_LEFT_FAR,
			DOWN_RIGHT_FAR,
			UP_LEFT_NEAR,
			UP_RIGHT_NEAR,
			UP_LEFT_FAR,
			UP_RIGHT_FAR
		};
		/*																							 */


		corners[DOWN_LEFT_NEAR] = { pos.x - vars::boxESPSize, pos.y - vars::boxESPSize, pos.z };
		corners[DOWN_RIGHT_NEAR] = { pos.x + vars::boxESPSize, pos.y - vars::boxESPSize, pos.z };
		corners[DOWN_LEFT_FAR] = { pos.x - vars::boxESPSize, pos.y + vars::boxESPSize, pos.z };
		corners[DOWN_RIGHT_FAR] = { pos.x + vars::boxESPSize, pos.y + vars::boxESPSize, pos.z };
		corners[UP_LEFT_NEAR] = { pos.x - vars::boxESPSize, pos.y - vars::boxESPSize, pos.z + ped->GetVecViewOffset().z };
		corners[UP_RIGHT_NEAR] = { pos.x + vars::boxESPSize, pos.y - vars::boxESPSize, pos.z + ped->GetVecViewOffset().z };
		corners[UP_LEFT_FAR] = { pos.x - vars::boxESPSize, pos.y + vars::boxESPSize, pos.z + ped->GetVecViewOffset().z };
		corners[UP_RIGHT_FAR] = { pos.x + vars::boxESPSize, pos.y + vars::boxESPSize, pos.z + ped->GetVecViewOffset().z };


		/* ���� ����� �� �������� WorldToScreen ������ ��� */
		for (unsigned int i = 0; i < 8; i++) {
			if (!Utils::WorldToScreen(corners[i], screenPos[i])) {
				return;
			}
		}

		ImDrawList* draw = ImGui::GetBackgroundDrawList();
		ImU32 ESPColor = calcESPColor(ped->GetHealth(), playerIndex);

		constexpr std::array<std::pair<int, int>, 12> cornersSerialNumber = {
		   std::make_pair(DOWN_LEFT_NEAR, DOWN_RIGHT_NEAR),
		   std::make_pair(DOWN_LEFT_NEAR, DOWN_LEFT_FAR),
		   std::make_pair(DOWN_LEFT_NEAR, UP_LEFT_NEAR),
		   std::make_pair(DOWN_RIGHT_NEAR, DOWN_RIGHT_FAR),
		   std::make_pair(DOWN_RIGHT_NEAR, UP_RIGHT_NEAR),
		   std::make_pair(DOWN_LEFT_FAR, DOWN_RIGHT_FAR),
		   std::make_pair(DOWN_LEFT_FAR, UP_LEFT_FAR),
		   std::make_pair(DOWN_RIGHT_FAR, UP_RIGHT_FAR),
		   std::make_pair(UP_LEFT_NEAR, UP_RIGHT_NEAR),
		   std::make_pair(UP_LEFT_NEAR, UP_LEFT_FAR),
		   std::make_pair(UP_RIGHT_NEAR, UP_RIGHT_FAR),
		   std::make_pair(UP_LEFT_FAR, UP_RIGHT_FAR)

		};

		for (auto& [from, to] : cornersSerialNumber) {
			draw->AddLine(screenPos[from].ToImVec2(), screenPos[to].ToImVec2(), ESPColor);

		}

	}


	ImU32 calcESPColor(const unsigned int health, int playerIndex) {
#ifdef DEBUG
		debug("calcESPColor() \n");
#endif // DEBUG

		if (Utils::isPlayerInFriendList(playerIndex))
			return 0xFFCC0099; // <- ABGR | RGB -> #9900cc

		if (health >= 100.0f)
			return  0xFF00FF00;


		float clrG = health / 100.0f;
		float clrR = (100.0f - health) / 100.0f;
		ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(clrR, clrG, 0.0f, 1.0f));
		return color;
	}

};

CESP* ESP;