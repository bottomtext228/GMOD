
class CESP {
public:
	
	void Process() {

		if (vars::esp::boneESP || vars::esp::linesState || vars::esp::box3DESP ||
			vars::esp::box2DESP || vars::esp::renderHealthAndNick || vars::esp::renderEntity) { // ????? ?????? ???? ?? ???????? EntityList ?????? ??? 
			if (localPed) {

				for (int entityIndex = 0; entityIndex <= Interfaces.ClientEntityList->GetHighestEntityIndex(); entityIndex++) {

					CPed* ped = Interfaces.ClientEntityList->GetClientEntity(entityIndex);
					if (ped) {
						if (ped != localPed  && ped->isAlive() && ped->isEntityIsPlayer() && ped->m_modelInfo) {
							
							if (vars::esp::box3DESP) {
								DrawBoxESP(ped, entityIndex);
							}

							if (vars::esp::linesState) {
								DrawLines(ped, entityIndex);
							}

							if (vars::esp::boneESP) {
								DrawBoneESP(ped, entityIndex);
							}

							if (vars::esp::renderHealthAndNick) {
								DrawHealthAndNick(ped, entityIndex);						
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
	}

private:
	void DrawLines(CPed* ped, int playerIndex) {
#ifdef DEBUG
		debug("DrawLines() \n");
#endif // DEBUG

		CVector pos = ped->m_fPos;
		pos.z += 78;

		CVector2D screenPos;
		if (Misc->WorldToScreen(pos, screenPos)) {
			ImDrawList* draw = ImGui::GetBackgroundDrawList();
			draw->AddLine(ImVec2((float)vars::resX / 2.0f, 0.0f), ImVec2((float)screenPos.x, (float)screenPos.y), calcESPColor(ped->m_iHealth, playerIndex));
		}
	}
	void DrawHealthAndNick(CPed* ped, int playerIndex) {
#ifdef DEBUG
		debug("DrawHealthAndNick\n");
#endif // DEBUG

		CVector pos = ped->m_fPos;
		pos.z += 80;

		CVector2D screenPos;
		if (Misc->WorldToScreen(pos, screenPos)) {

			char espText[150];
			char playerNick[128];
			Interfaces.Engine->GetPlayerNick(playerNick, playerIndex);
			sprintf_s(espText, "[%d] %s", ped->m_iHealth, playerNick);
			ImVec2 textSize = ImGui::CalcTextSize(espText);
			ImDrawList* draw = ImGui::GetBackgroundDrawList();
			draw->AddText(ImVec2(screenPos.x - textSize.x / 2, screenPos.y - textSize.y), -1, espText);

		}
	}
	void DrawEntities(CPed* entity) {
#ifdef DEBUG
		debug("DrawEntities()\n");
#endif // DEBUG

		if (entity->m_iTeamNum == 0 && entity->m_modelInfo) {

			ImU32 espColor = 0xFFFF7300;
			if (Misc->isEntityInSpecificEntitiesList(entity->m_modelInfo->m_ModelName)) {
				espColor = 0xFF00FF22; // 0073FF  22FF00 <- rgb | abgr -> FFFF7300 FF00FF22
			
			}
			else {
				if (vars::esp::renderSpecificEntities)
					return;
			}
			CVector2D entityPos;
			if (Misc->WorldToScreen(entity->m_fPos, entityPos)) {
				ImDrawList* draw = ImGui::GetBackgroundDrawList();
				ImVec2 textSize = ImGui::CalcTextSize(entity->m_modelInfo->m_ModelName); 
				draw->AddText(ImVec2(entityPos.x - textSize.x / 2, entityPos.y - textSize.y), espColor, entity->m_modelInfo->m_ModelName);

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
		Misc->GetBones(ped, cornerBones);
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
		Misc->WorldToScreen(lowestBonePos, lowestBoneScreenPos);
		Misc->WorldToScreen(highestBonePos, highestBoneScreenPos);
		Misc->WorldToScreen(rightmostBonePos, rightmostBoneScreenPos);
		Misc->WorldToScreen(leftmostBonePos, leftmostBoneScreenPos);


		ImDrawList* draw = ImGui::GetBackgroundDrawList();
		draw->AddRect(
			ImVec2{ (float)rightmostBoneScreenPos.x, (float)highestBoneScreenPos.y },
			ImVec2{ (float)leftmostBoneScreenPos.x, (float)lowestBoneScreenPos.y },
			calcESPColor(ped->m_iHealth, playerIndex)
		);
					
	

	}

	void DrawBoneESP(CPed* ped, int playerIndex) {
#ifdef DEBUG
		debug("DrawBoneESP() \n");
#endif // DEBUG


		ImU32 espColor = calcESPColor(ped->m_iHealth, playerIndex);

		auto studioHdr = Interfaces.ModelInfo->GetStudiomodel(ped->m_modelInfo);
			for (int boneIndex = 0; boneIndex < studioHdr->numbones; boneIndex++)
			{
				auto bone = studioHdr->pBone(boneIndex);
				if (bone && bone->parent >= 0 && bone->flags & 256)
				{

					CVector normalBonePos = ped->GetBonePosition(boneIndex);
					CVector normalParentBonePos = ped->GetBonePosition(bone->parent);

					if (normalBonePos == CVector(0, 0, 0) || normalParentBonePos == CVector(0, 0, 0) 
						|| normalBonePos.DistanceTo(ped->m_fPos) > 150.0f || normalParentBonePos.DistanceTo(ped->m_fPos) > 150.f)
						continue;

					CVector2D bonePosFrom;
					CVector2D parentBonePos;
					if (!Misc->WorldToScreen(normalBonePos, bonePosFrom) || !Misc->WorldToScreen(normalParentBonePos, parentBonePos))
						continue;
					ImDrawList* draw = ImGui::GetBackgroundDrawList();
					draw->AddLine(ImVec2((float)bonePosFrom.x, (float)bonePosFrom.y), ImVec2((float)parentBonePos.x, (float)parentBonePos.y), espColor, 1);
					
				}
			}
	}

	void DrawBoxESP(CPed* ped, int playerIndex) {
#ifdef DEBUG
		debug("DrawBox ESP \n");
#endif // DEBUG

		CVector pos = ped->m_fPos;
		CVector2D screenPos[8];
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
		corners[UP_LEFT_NEAR] = { pos.x - vars::boxESPSize, pos.y - vars::boxESPSize, pos.z + ped->m_fViewOffset.z};
		corners[UP_RIGHT_NEAR] = { pos.x + vars::boxESPSize, pos.y - vars::boxESPSize, pos.z + ped->m_fViewOffset.z };
		corners[UP_LEFT_FAR] = { pos.x - vars::boxESPSize, pos.y + vars::boxESPSize, pos.z + ped->m_fViewOffset.z };
		corners[UP_RIGHT_FAR] = { pos.x + vars::boxESPSize, pos.y + vars::boxESPSize, pos.z + ped->m_fViewOffset.z };

		
		/* ???? ????? ?? ???????? WorldToScreen ?????? ??? */
		bool isAllPlayerInOurView = true;
		for (unsigned int i = 0; i < 8; i++) {
			if (!Misc->WorldToScreen(corners[i], screenPos[i])) { 
				isAllPlayerInOurView = false;
				break;
			}
		}
		if (!isAllPlayerInOurView)
			return;
		ImDrawList* draw = ImGui::GetBackgroundDrawList();
		ImU32 ESPColor = calcESPColor(ped->m_iHealth, playerIndex);
		
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
	
		for (auto& corner : cornersSerialNumber) {
			draw->AddLine(ImVec2( (float)screenPos[corner.first].x, (float)screenPos[corner.first].y), ImVec2((float)screenPos[corner.second].x, (float)screenPos[corner.second].y), ESPColor);
			
		}
		
	}
	

	ImU32 calcESPColor(const unsigned int health, int playerIndex) {
#ifdef DEBUG
		debug("calcESPColor() \n");
#endif // DEBUG

		if (Misc->isPlayerInFriendList(playerIndex))
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