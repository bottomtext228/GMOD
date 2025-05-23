
// all of these are for ExtraMouseSample smoothing
static CVector m_vecStartedVA;
static CVector m_vecWantedVA;
static bool m_shouldSmoothMouse;
static CPed* m_target;


class CAim {
public:
	void Process(CUserCmd* cmd) { 
		if (((vars::aim::smoothAim) || (vars::aim::silentAim && cmd->m_buttons.IN_ATTACK)) && vars::aim::aimKeyBind.isDown()) {

#if USE_ENGINE_PREDICTION
			CVector localPos = localPed->GetEyePosition();
#else
			CVector localPos = localPed->GetEyePosition() + Utils::predictEntityOffsetPosition(localPed->GetVecVelocity());
#endif // USE_ENGINE_PREDICTION	

			mstudiobbox_t* hitbox = nullptr;
			int boneId = -1;
			CPed* ped = nullptr;
			if (vars::aim::currentAimTargetType == vars::aim::targetTypes[0]) { // hitbox
				ped = getHitboxAndNearestPed(localPos, hitbox);
			}
			if (vars::aim::currentAimTargetType == vars::aim::targetTypes[1]) { // bone
				ped = getBoneAndNearestPed(localPos, boneId);
			}

			if (ped && isPedInFOV(ped)) {
				if (!vars::aim::stickyAim) {

					trace_t trace = Interfaces.Trace->TraceRayWrapper(localPos, Utils::GetPointOfSight(localPos, cmd->m_viewangles), localPed); // trace.hitbox == 0 - head maybe;
					if (ped == trace.ped)
						return;
				}
				matrix3x4_t boneMatrix[128];
				if (!ped->GetClientRenderable()->SetupBones(boneMatrix, 128, 0x100, Interfaces.GlobalVars->curtime))
					return;
				CVector targetPos;
				if (vars::aim::currentAimTargetType == vars::aim::targetTypes[0]) {
					targetPos = GetAABBCenter(boneMatrix[hitbox->bone], hitbox->bbmin, hitbox->bbmax);
				}
				if (vars::aim::currentAimTargetType == vars::aim::targetTypes[1]) {
					targetPos = { boneMatrix[boneId].m[0][3], boneMatrix[boneId].m[1][3], boneMatrix[boneId].m[2][3] };
				}
				constexpr float tickOffset = 0.5f; // experimently obtained
				targetPos += Utils::predictEntityOffsetPosition(ped->GetVecVelocity()) * tickOffset;
				CVector aimAngles = calcAimAngle(localPos, targetPos);
				if (vars::aim::smoothAim) {
					m_vecStartedVA = cmd->m_viewangles;
					CVector smoothAngles = calcSmoothAngles(cmd->m_viewangles, aimAngles);

					cmd->m_viewangles.y = smoothAngles.y;

					m_vecWantedVA = CVector(aimAngles.x, aimAngles.y, 0.0f);
					if (!vars::aim::ignoreZ) {
						cmd->m_viewangles.x = smoothAngles.x;
					}


					// if we don't smooth aim via ExtraMouseSample uncomment this
					//Interfaces.Engine->SetViewAngles(cmd->m_viewangles);

					m_shouldSmoothMouse = true;
					m_target = ped;
				}
				if (vars::aim::silentAim) {
					cmd->m_viewangles.y = aimAngles.y;
					if (!vars::aim::ignoreZ) { // does silent aim really need this?
						cmd->m_viewangles.x = aimAngles.x;
					}
					vars::aim::newAngles = cmd->m_viewangles;
				}
			}
			else { m_shouldSmoothMouse = false; } // if target is not found
		}
		else { m_shouldSmoothMouse = false; } // if aim is not active
	}
private:
	bool isPedInFOV(CPed* ped) { // ����� ����������� ��������� � ������������� �� ������ ������( ������, ���� � �.�.), ��� ����� �� ������ ���� ��� ��������� FOV
#ifdef DEBUG
		debug("isPedInFOV() \n");
#endif // DEBUG

		auto studioHdr = Interfaces.ModelInfo->GetStudiomodel(ped->GetModelInfo());
		for (int boneIndex = 0; boneIndex < studioHdr->numbones; boneIndex++) {
			auto bone = studioHdr->pBone(boneIndex);
			if (bone && bone->parent >= 0 && bone->flags & 256) {
				CVector2D screenPos;
				CVector bonePos = ped->GetBonePosition(boneIndex);
				if (Utils::WorldToScreen(bonePos, screenPos)) {
					float distance = CVector2D{ vars::resX / 2, vars::resY / 2 }.DistanceTo(screenPos);
					if (distance < vars::aim::FOV) {
						return true;
					}
				}
			}
		}


		return false;
	}
	bool inline isPointVisible(CVector pointPos) { // �������� �� ���������� �����������
#ifdef DEBUG
		debug("isPointVisible() \n");
#endif // DEBUG

		/*trace_t trace = Interfaces.Trace->TraceRayWrapper(ourView, pointPos, localPed);
		if (trace.ped && trace.ped == targetPed)
			return true;
		return false;
		*/
		return localPed->IsLineOfSightClear(pointPos, CPed::LineOfSightCheckType::IGNORE_ACTORS, localPed);
	}
	CVector calcAimAngle(CVector localPos, CVector bonePos) { // ������������ ����� ����� �������� ���������, ��� �������� ��� �� �����
#ifdef DEBUG
		debug("calcAimAngle() \n");
#endif // DEBUG

		float multiplX = sqrtf(powf(localPos.x - bonePos.x, 2) + powf(localPos.y - bonePos.y, 2));
		float angleY = ((Utils::radiansToDegrees(asinf((localPos.x - bonePos.x) / multiplX))) + 90.0f);
		if (localPos.y - bonePos.y > 0) { // I am more white than you
			angleY = -angleY;
		}
		float distance = localPos.DistanceTo(bonePos);
		float angleX = Utils::radiansToDegrees(asinf((localPos.z - bonePos.z) / distance));
		return { angleX, angleY, 0.0f };
	}
	CVector calcSmoothAngles(CVector cmd, CVector aimAngle) { // ������� �������
#ifdef DEBUG
		debug("calcSmoothAngles() \n");
#endif // DEBUG

		CVector smoothAngles{};
		if (!vars::aim::rage) {
			CVector vec = cmd - aimAngle;
			if (vec.y > 180) { // aim angle can be only in [-180, 180] range, so when it should be > 180 it's actually goes to -180.
				vec.y -= 360;
			}
			if (vec.y < -180) {
				vec.y += 360;
			}

			if (vars::aim::currentAimMode == vars::aim::aimModes[0]) { // smooth steps
				smoothAngles.x = cmd.x - vec.x * (1 / vars::aim::aimSmoothSteps);
				smoothAngles.y = cmd.y - vec.y * (1 / vars::aim::aimSmoothSteps);
			}
			if (vars::aim::currentAimMode == vars::aim::aimModes[1]) { // angle speed
				float delta = Interfaces.GlobalVars->interval_per_tick * vars::aim::aimAngleSpeed;
				if (delta < cmd.DistanceTo(aimAngle)) {
					vec.Normalize();
					smoothAngles.x = cmd.x - vec.x * delta;
					smoothAngles.y = cmd.y - vec.y * delta;
				}
				else {
					smoothAngles = aimAngle;
				}
			}

		}
		else {
			smoothAngles = aimAngle;
		}
		return smoothAngles;

	}
public:
	CVector calcSmoothAnglesFrame(CVector cmd, CVector aimAngle, float timedelta) { // ������� �������
#ifdef DEBUG
		debug("calcSmoothAngles() \n");
#endif // DEBUG

		CVector smoothAngles{};
		if (!vars::aim::rage) {
			CVector vec = cmd - aimAngle;
			if (vec.y > 180) { // aim angle can be only in [-180, 180] range, so when it should be > 180 it's actually goes to -180.
				vec.y -= 360;
			}
			if (vec.y < -180) {
				vec.y += 360;
			}

			if (vars::aim::currentAimMode == vars::aim::aimModes[0]) { // smooth steps
				smoothAngles = cmd - vec * (1 / vars::aim::aimSmoothSteps);
			}
			if (vars::aim::currentAimMode == vars::aim::aimModes[1]) { // angle speed
				float delta = timedelta * vars::aim::aimAngleSpeed;
				if (delta < cmd.DistanceTo(aimAngle)) {
					vec.Normalize();
					smoothAngles = cmd - vec * delta;
				}
				else {
					smoothAngles = aimAngle;
				}
			}

		}
		else {
			smoothAngles = aimAngle;
		}
		return smoothAngles;

	}
	CPed* getHitboxAndNearestPed(CVector localPos, mstudiobbox_t*& hitbox) {
		CPed* nearestPed = 0;
		float distance = 99999;

		for (int entityIndex = 0; entityIndex < Interfaces.GlobalVars->maxClients; entityIndex++) {
			CPed* ped = Interfaces.ClientEntityList->GetClientEntity(entityIndex);
			if (ped && ped != localPed && ped->IsAlive() && ped->IsPlayer() && ped->GetModelInfo() && !Utils::isPlayerInFriendList(entityIndex)) {

				// I know that it's strange to see esp vars in aim functions but I don't want more nearly useless vars, checkbox, sliders for that
				if (vars::esp::dormantCheck && ped->IsDormant()) continue;
				if (vars::esp::distanceCheck && ped->GetVecOrigin().DistanceTo(localPos) > vars::esp::distance) continue;

				CVector2D screenPos;
				CVector pedPos = ped->GetVecOrigin() + ped->GetVecViewOffset();
				if (Utils::WorldToScreen(pedPos, screenPos)) {
					float newDistance = CVector2D{ vars::resX / 2, vars::resY / 2 }.DistanceTo(screenPos);
					if (newDistance < distance) {
						mstudiobbox_t* aimHitbox = getTargetedHitbox(ped);
						if (aimHitbox) {
							CVector hitboxPos = GetAABBCenter(ped->GetBoneMatrix(aimHitbox->bone), aimHitbox->bbmin, aimHitbox->bbmax);
							if (!vars::aim::ignoreWalls && !isPointVisible(hitboxPos))
								continue;
							nearestPed = ped;
							distance = newDistance;
							hitbox = aimHitbox;
						}
					}
				}
			}
		}
		return nearestPed;
	}


	CPed* getBoneAndNearestPed(CVector localPos, int& boneId) { // ��������� ���������� � �������(�������� ������) ���� � �����
		// �������� ���������� ��� ���������� �������� �� �����������
#ifdef DEBUG
		debug("getBoneAndNearestPed() \n");
#endif // DEBUG

		CPed* nearestPed = 0;
		float distance = 99999;

		for (int entityIndex = 0; entityIndex < Interfaces.GlobalVars->maxClients; entityIndex++) {
			CPed* ped = Interfaces.ClientEntityList->GetClientEntity(entityIndex);
			if (ped && ped != localPed && ped->IsAlive() && ped->IsPlayer() && ped->GetModelInfo() && !Utils::isPlayerInFriendList(entityIndex)) {

				// I know that it's strange to see esp vars in aim functions but I don't want more nearly useless vars, checkbox, sliders for that
				if (vars::esp::dormantCheck && ped->IsDormant()) continue;
				if (vars::esp::distanceCheck && ped->GetVecOrigin().DistanceTo(localPos) > vars::esp::distance) continue;

				CVector2D screenPos;
				CVector pedPos = ped->GetVecOrigin() + ped->GetVecViewOffset();
				if (Utils::WorldToScreen(pedPos, screenPos)) {
					float newDistance = CVector2D{ vars::resX / 2, vars::resY / 2 }.DistanceTo(screenPos);
					if (newDistance < distance) {
						int aimBone = getTargetedBone(ped);
						if (aimBone != -1) {
							CVector bonePos = ped->GetBonePosition(aimBone);
							if (!vars::aim::ignoreWalls && !isPointVisible(bonePos))
								continue;
							nearestPed = ped;
							distance = newDistance;
							boneId = aimBone;
						}

					}
				}
			}
		}
		return nearestPed;
	}

	int getTargetedBone(CPed* ped) {
#ifdef DEBUG
		debug("getTargetedBone() \n");
#endif // DEBUG

		if (vars::aim::currentAimTarget == vars::aim::aimTargets[0]) { // head
			auto boneId = Utils::BoneIndexByName(ped, "head");
			if (boneId != -1)
				return boneId;
		}
		if (vars::aim::currentAimTarget == vars::aim::aimTargets[2]) {// body
			auto boneId = Utils::BoneIndexByName(ped, "spine1");
			if (boneId != -1)
				return boneId;
		}
		return getNearestBone(ped); // if don't find needed bone( hello to the worst models in the existance created by gmod community ) we find the nearest bone

	}

	mstudiobbox_t* getTargetedHitbox(CPed* ped) {
#ifdef DEBUG
		debug("getTargetedBone() \n");
#endif // DEBUG

		if (vars::aim::currentAimTarget == vars::aim::aimTargets[0]) { // head
			auto hitbox = Utils::HitboxByBoneName(ped, "head");
			if (hitbox)
				return hitbox;
		}
		if (vars::aim::currentAimTarget == vars::aim::aimTargets[2]) {// body
			auto hitbox = Utils::HitboxByBoneName(ped, "spine2");
			if (hitbox)
				return hitbox;
		}
		return getNearestHitbox(ped);

	}

	int getNearestBone(CPed* ped) { // ��������� ��������� � �������(�������� ������) �����

#ifdef DEBUG
		debug("getNearestBone() \n");
#endif // DEBUG

		int nearestBone = -1;
		float distance = 99999;

		auto studioHdr = Interfaces.ModelInfo->GetStudiomodel(ped->GetModelInfo());

		for (int boneIndex = 0; boneIndex < studioHdr->numbones; boneIndex++) {

			auto bone = studioHdr->pBone(boneIndex);
			if (bone && bone->parent >= 0 && bone->flags & 256) {
				if (vars::aim::ignoreLimbs) {
					char boneName[64];
					strcpy(boneName, bone->pszName());
					for (int i = 0; boneName[i]; i++) {
						boneName[i] = tolower(boneName[i]);
					}
					if (!strstr(boneName, "head") && !strstr(boneName, "spine"))
						continue;
				}
				CVector bonePos = ped->GetBonePosition(boneIndex);
				if (!vars::aim::ignoreWalls && !isPointVisible(bonePos))
					continue;
				CVector2D screenPos;
				if (Utils::WorldToScreen(bonePos, screenPos)) {

					float newDistance = CVector2D{ vars::resX / 2, vars::resY / 2 }.DistanceTo(screenPos);
					if (newDistance < distance) {
						nearestBone = boneIndex;
						distance = newDistance;
					}
				}
			}
		}
		return nearestBone;

	}
	mstudiobbox_t* getNearestHitbox(CPed* ped) { // ��������� ��������� � �������(�������� ������) �����

#ifdef DEBUG
		debug("getNearestBone() \n");
#endif // DEBUG

		mstudiobbox_t* nearestHitbox = nullptr;
		float distance = 99999;


		auto studioHdr = Interfaces.ModelInfo->GetStudiomodel(ped->GetModelInfo());


		auto set = studioHdr->pHitboxSet(0);
		for (int hitboxIndex = 0; hitboxIndex < set->numhitboxes; hitboxIndex++) {
			auto hitbox = set->pHitbox(hitboxIndex);
			auto bone = studioHdr->pBone(hitbox->bone);

			if (vars::aim::ignoreLimbs) {
				char boneName[64];
				strcpy(boneName, bone->pszName());
				for (int i = 0; boneName[i]; i++) {
					boneName[i] = tolower(boneName[i]);
				}
				if (!strstr(boneName, "head") && !strstr(boneName, "spine2") && !strstr(boneName, "pelvis"))
					continue;
			}

			auto matrix = ped->GetBoneMatrix(hitbox->bone);
			CVector hitboxPos = GetAABBCenter(matrix, hitbox->bbmin, hitbox->bbmax);
			if (!vars::aim::ignoreWalls && !isPointVisible(hitboxPos))
				continue;
			CVector2D screenPos;
			if (Utils::WorldToScreen(hitboxPos, screenPos)) {

				float newDistance = CVector2D{ vars::resX / 2, vars::resY / 2 }.DistanceTo(screenPos);
				if (newDistance < distance) {
					nearestHitbox = hitbox;
					distance = newDistance;
				}
			}
		}
		return nearestHitbox;


	}

public:
	void ShowTarget() {
#ifdef DEBUG
		debug("ShowTarget() \n");
#endif // DEBUG

		if (!localPed || !localPed->IsAlive())
			return;

		CVector localPos = localPed->GetVecOrigin() + localPed->GetVecViewOffset();
		mstudiobbox_t* hitbox = nullptr;
		int boneId = -1;
		CPed* ped = nullptr;
		if (vars::aim::currentAimTargetType == vars::aim::targetTypes[0]) { // hitbox
			ped = getHitboxAndNearestPed(localPos, hitbox);
		}
		if (vars::aim::currentAimTargetType == vars::aim::targetTypes[1]) { // bone
			ped = getBoneAndNearestPed(localPos, boneId);
		}
		if (ped && isPedInFOV(ped)) {
			CVector2D screenPos;
			CVector targetPos;
			if (vars::aim::currentAimTargetType == vars::aim::targetTypes[0]) {
				targetPos = GetAABBCenter(ped->GetBoneMatrix(hitbox->bone), hitbox->bbmin, hitbox->bbmax);
			}
			if (vars::aim::currentAimTargetType == vars::aim::targetTypes[1]) {
				targetPos = ped->GetBonePosition(boneId);
			}
			if (Utils::WorldToScreen(targetPos, screenPos)) {
				ImDrawList* draw = ImGui::GetBackgroundDrawList();
				draw->AddLine(ImVec2((float)vars::resX / 2.0f, 0), screenPos.ToImVec2(), 0x77FF1E00); // blue
			}
		}

	}

	void RenderFOV() {
#ifdef DEBUG
		debug("RenderFOV() \n");
#endif // DEBUG

		ImDrawList* draw = ImGui::GetBackgroundDrawList();
		draw->AddCircle(ImVec2((float)vars::resX / 2.0f, (float)vars::resY / 2.0f), (float)vars::aim::FOV, -1, 360);
	}

};