class CMisc {
public:
	bool WorldToScreen(CVector pos, CVector2D& screen)
	{
		//float* matrix = Interfaces.Engine->WorldToScreenMatrix();

		//Matrix-vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords
		CVector4D clipCoords;
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
			if (playerPed->m_iTeamNum == localPed->m_iTeamNum) // same team
				return true;
		}
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

		for (int i = 0; i < vars::misc::specificEntities; i++) {
			if (!strcmp(vars::misc::specificEntitiesList[i], entityName))
				return true;
		}
		return false;
	}




	void GetBones(CPed* ped, int* cornerBones) {
#ifdef DEBUG
		debug("GetBones() \n");
#endif // DEBUG

		int distance = 99999;
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
		auto studioHdr = Interfaces.ModelInfo->GetStudiomodel(ped->m_modelInfo);

		for (int boneIndex = 0; boneIndex < studioHdr->numbones; boneIndex++)
		{
			auto bone = studioHdr->pBone(boneIndex);
			if (bone && bone->parent >= 0 && bone->flags & 256)
			{
				CVector2D screenPos;
				CVector bonePos = ped->GetBonePosition(boneIndex);

				if (bonePos != CVector(0.0, 0.0, 0.0) && bonePos.DistanceTo(ped->m_fPos) < 150.0 && WorldToScreen(bonePos, screenPos)) {
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

		auto pStudioHdr = Interfaces.ModelInfo->GetStudiomodel(ped->m_modelInfo);

		for (int boneIndex = 0; boneIndex < pStudioHdr->numbones; boneIndex++) {
			auto bone = pStudioHdr->pBone(boneIndex);
			if (bone) {
				if (!strcmp(bone->pszName(), pName))
					return boneIndex;

			}
		}
		return -1;
	}
	void SetupBones() {
#ifdef DEBUG
		debug("SetupBones() \n");
#endif // DEBUG
		
		static ULONGLONG ticks = 0;
		if (GetTickCount64() - ticks > vars::esp::setupBonesDelay) {
			ticks = GetTickCount64();
			for (int entityIndex = 0; entityIndex < 256; entityIndex++) {

				CPed* ped = Interfaces.ClientEntityList->GetClientEntity(entityIndex);
				if (ped && ped->isEntityIsPlayer() && ped->isAlive() && ped != localPed) {
					auto renderAble = ped->GetClientRenderable();
					if ((uintptr_t)renderAble > 0x1000) {// якобы предотвращает возможный краш
						renderAble->SetupBones(NULL, -1, 0x100, Interfaces.GlobalVars->curtime);

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
	void CustomSight() {
		ImDrawList* draw = ImGui::GetBackgroundDrawList();
		ImVec2 Pos2D((float)vars::resX / 2.0f, (float)vars::resY / 2.0f);
		draw->AddCircle(Pos2D, 3, vars::misc::sightColor, 20, 1.0f);
		draw->AddCircleFilled(Pos2D, 1.0, vars::misc::sightColor, 20);
	}
	void AutoUncuff(CUserCmd* CMD) {
		static bool isInAttack = false;
		CMD->m_buttons.IN_ATTACK2 = !isInAttack;
		isInAttack = !isInAttack;
	}

	void Unload(LPDIRECT3DDEVICE9 pDevice, HWND window, WNDPROC oWndProc) {
		/* Всё ниже обязательно для нормальной выгрузки */
		kiero::shutdown();
		pDevice->Release();
		SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)(oWndProc));
	}
	
}; CMisc* Misc;

