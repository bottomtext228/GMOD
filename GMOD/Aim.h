
class CAim {
public:
	void Process(CUserCmd* CMD) {
		if (((vars::aim::smoothAim) || (vars::aim::silentAim && CMD->m_buttons.IN_ATTACK)) && vars::aim::aimKeyBind.isDown()) {
#if USE_ENGINE_PREDICTION
			CVector localPos = localPed->GetEyePosition();
#else
			CVector localPos = localPed->GetEyePosition() + Misc->predictEntityOffsetPosition(localPed->GetVecVelocity());
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
					CVector targetDir = {
						localPed->GetVecOrigin().x + MAX_TRACE_LENGTH * cosf(Misc->degreesToRadians(CMD->m_viewangles.y)),
						localPed->GetVecOrigin().y + MAX_TRACE_LENGTH * sinf(Misc->degreesToRadians(CMD->m_viewangles.y)),
						localPed->GetVecOrigin().z + localPed->GetVecViewOffset().z - MAX_TRACE_LENGTH * tanf(Misc->degreesToRadians(CMD->m_viewangles.x))
					};

					trace_t trace = Interfaces.Trace->TraceRayWrapper(localPos, targetDir, localPed); // trace.hitbox == 0 - head maybe;
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
				targetPos += Misc->predictEntityOffsetPosition(ped->GetVecVelocity()) * tickOffset;
				CVector2DF aimAngles = calcAimAngle(localPos, targetPos);
				if (vars::aim::smoothAim) {
					CVector2DF smoothAngles = calcSmoothAngles(CVector2DF{ CMD->m_viewangles.y, CMD->m_viewangles.x }, aimAngles);
					CMD->m_viewangles.y = smoothAngles.x;
					if (!vars::aim::ignoreZ) {
						CMD->m_viewangles.x = smoothAngles.y;
					}
					Interfaces.Engine->SetViewAngles(CMD->m_viewangles);
				}
				if (vars::aim::silentAim) {
					CMD->m_viewangles.y = aimAngles.x;
					if (!vars::aim::ignoreZ) { // does silent aim really need this?
						CMD->m_viewangles.x = aimAngles.y;
					}
					vars::aim::newAngles = CMD->m_viewangles;
				}
			}
		}
	}
private:
	bool isPedInFOV(CPed* ped) { // чтобы триггерился одинаково в независимости от режима работы( голова, тело и т.д.), без этого не удобно было при маленьком FOV
#ifdef DEBUG
		debug("isPedInFOV() \n");
#endif // DEBUG

		auto studioHdr = Interfaces.ModelInfo->GetStudiomodel(ped->GetModelInfo());
		for (int boneIndex = 0; boneIndex < studioHdr->numbones; boneIndex++) {
			auto bone = studioHdr->pBone(boneIndex);
			if (bone && bone->parent >= 0 && bone->flags & 256) {
				CVector2D screenPos;
				CVector bonePos = ped->GetBonePosition(boneIndex);
				if (Misc->WorldToScreen(bonePos, screenPos)) {
					float distance = CVector2D{ vars::resX / 2, vars::resY / 2 }.DistanceTo(screenPos);
					if (distance < vars::aim::FOV) {
						return true;
					}
				}
			}
		}


		return false;
	}
	bool inline isPointVisible(CVector pointPos) { // проверка на отсутствие препятствий
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
	CVector2DF calcAimAngle(CVector localPos, CVector bonePos) { // высчитывание таких углов поворота персонажа, что направит его на кость
#ifdef DEBUG
		debug("calcAimAngle() \n");
#endif // DEBUG

		float multiplX = sqrtf(powf(localPos.x - bonePos.x, 2) + powf(localPos.y - bonePos.y, 2));
		float angleX = ((Misc->radiansToDegrees(asinf((localPos.x - bonePos.x) / multiplX))) + 90.0f);
		if (localPos.y - bonePos.y > 0) { // I am more white than you
			angleX = -angleX;
		}
		float distance = localPos.DistanceTo(bonePos);
		float angleY = Misc->radiansToDegrees(asinf((localPos.z - bonePos.z) / distance));
		return { angleX, angleY };
	}
	CVector2DF calcSmoothAngles(CVector2DF cmd, CVector2DF aimAngle) { // плавная наводка
#ifdef DEBUG
		debug("calcSmoothAngles() \n");
#endif // DEBUG

		CVector2DF smoothAngles{};
		if (!vars::aim::rage/* && vars::aim::aimForce < cmd.DistanceTo(aimAngle)*/) {
			CVector2DF vec = cmd - aimAngle;
			if (vec.x > 180) { // aim angle can be only in [-180, 180] range, so when it should be > 180 it's actually goes to -180.
				vec.x -= 360;
			}
			if (vec.x < -180) {
				vec.x += 360;
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
	CPed* getHitboxAndNearestPed(CVector localPos, mstudiobbox_t*& hitbox) {
		CPed* nearestPed = 0;
		float distance = 99999;

		for (int entityIndex = 0; entityIndex < 256; entityIndex++) {
			CPed* ped = Interfaces.ClientEntityList->GetClientEntity(entityIndex);
			if (ped && ped != localPed && ped->IsAlive() && ped->IsPlayer() && ped->GetModelInfo() && !Misc->isPlayerInFriendList(entityIndex)) {
				CVector2D screenPos;
				CVector pedPos = ped->GetVecOrigin() + ped->GetVecViewOffset();
				if (Misc->WorldToScreen(pedPos, screenPos)) {
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


	CPed* getBoneAndNearestPed(CVector localPos, int& boneId) { // получение ближайшего к прицелу(середина экрана) педа и кости
		// пришлось объединить для адекватной проверки на препятствие
#ifdef DEBUG
		debug("getBoneAndNearestPed() \n");
#endif // DEBUG

		CPed* nearestPed = 0;
		float distance = 99999;

		for (int entityIndex = 0; entityIndex < 256; entityIndex++) {
			CPed* ped = Interfaces.ClientEntityList->GetClientEntity(entityIndex);
			if (ped && ped != localPed && ped->IsAlive() && ped->IsPlayer() && ped->GetModelInfo() && !Misc->isPlayerInFriendList(entityIndex)) {
				CVector2D screenPos;
				CVector pedPos = ped->GetVecOrigin() + ped->GetVecViewOffset();
				if (Misc->WorldToScreen(pedPos, screenPos)) {
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
			auto boneId = Misc->BoneIndexByName(ped, "head");
			if (boneId != -1)
				return boneId;
		}
		if (vars::aim::currentAimTarget == vars::aim::aimTargets[2]) {// body
			auto boneId = Misc->BoneIndexByName(ped, "spine1");
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
			auto hitbox = Misc->HitboxByBoneName(ped, "head");
			if (hitbox)
				return hitbox;
		}
		if (vars::aim::currentAimTarget == vars::aim::aimTargets[2]) {// body
			auto hitbox = Misc->HitboxByBoneName(ped, "spine2");
			if (hitbox)
				return hitbox;
		}
		return getNearestHitbox(ped);

	}


	int getNearestBone(CPed* ped) { // получение ближайшей к прицелу(середина экрана) кости

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
				if (Misc->WorldToScreen(bonePos, screenPos)) {

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
	mstudiobbox_t* getNearestHitbox(CPed* ped) { // получение ближайшей к прицелу(середина экрана) кости

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
			if (Misc->WorldToScreen(hitboxPos, screenPos)) {

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

	//	void ShowTarget() {
	//#ifdef DEBUG
	//		debug("ShowTarget() \n");
	//#endif // DEBUG
	//
	//		if (!localPed)
	//			return;
	//		CVector localPos = localPed->GetVecOrigin() + localPed->GetVecViewOffset();
	//		int boneId;
	//		CPed* ped = getBoneAndNearestPed(localPos, boneId);
	//		if (ped && isPedInFOV(ped)) {
	//			CVector2D screenPos;
	//			CVector bonePos = ped->GetBonePosition(boneId);
	//			if (Misc->WorldToScreen(bonePos, screenPos)) {
	//				ImDrawList* draw = ImGui::GetBackgroundDrawList();
	//				draw->AddLine(ImVec2((float)vars::resX / 2.0f, 0), screenPos.ToImVec2(), 0x77FF1E00); // blue
	//			}
	//		}
	//
	//	}

	void ShowTarget() {
#ifdef DEBUG
		debug("ShowTarget() \n");
#endif // DEBUG

		if (!localPed)
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
			if (Misc->WorldToScreen(targetPos, screenPos)) {
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