
class CAim {
public:
	void Process(CUserCmd* CMD) {
		if ((vars::aim::smoothAim && CMD->m_buttons.IN_ATTACK2) || (vars::aim::silentAim && CMD->m_buttons.IN_ATTACK)) {
			int boneId;
			CVector localPos = localPed->GetEyePosition() + Misc->predictEntityOffsetPosition(localPed->m_fVelocity);
			CPed* ped = getBoneAndNearestPed(localPos, boneId);
			if (ped && isPedInFOV(ped)) {
				matrix3x4_t boneMatrix[128];
				if (!ped->GetClientRenderable()->SetupBones(boneMatrix, 128, 0x100, Interfaces.GlobalVars->curtime))
					return;
				CVector bonePos = { boneMatrix[boneId].m[0][3], boneMatrix[boneId].m[1][3], boneMatrix[boneId].m[2][3] };
				constexpr float tickOffset = 0.5f;
				bonePos += Misc->predictEntityOffsetPosition(ped->m_fVelocity) * tickOffset;
				CVector2DF aimAngles = calcAimAngle(localPos, bonePos);
				if (vars::aim::smoothAim) {
					CVector2DF smoothAngles = calcSmoothAngles(CVector2DF{ CMD->m_viewangles.y, CMD->m_viewangles.x }, aimAngles);
					CMD->m_viewangles.y = smoothAngles.x;
					CMD->m_viewangles.x = smoothAngles.y;
					Interfaces.Engine->SetViewAngles(CMD->m_viewangles);
				}
				if (vars::aim::silentAim) {
					CMD->m_viewangles.y = aimAngles.x;
					CMD->m_viewangles.x = aimAngles.y;
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

		auto studioHdr = Interfaces.ModelInfo->GetStudiomodel(ped->m_modelInfo);
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

		CVector2DF smoothAngles;
		if (!vars::aim::rage && vars::aim::aimForce < cmd.DistanceTo(aimAngle)) {
			CVector2DF vec = cmd - aimAngle;
			vec.Normalize();
			smoothAngles.x = cmd.x - vec.x * vars::aim::aimForce;
			smoothAngles.y = cmd.y - vec.y * vars::aim::aimForce;
		}
		else {
			smoothAngles = aimAngle;
		}
		return smoothAngles;

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
			if (ped && ped != localPed && ped->isAlive() && ped->isEntityIsPlayer() && ped->m_modelInfo && !Misc->isPlayerInFriendList(entityIndex)) {
				CVector2D screenPos;
				CVector pedPos = ped->m_fPos + ped->m_fViewOffset;

				if (Misc->WorldToScreen(pedPos, screenPos)) {
					int aimBone = -1;
					aimBone = getTargetedBone(localPos, ped);

					if (aimBone != -1) {

						float newDistance = CVector2D{ vars::resX / 2, vars::resY / 2 }.DistanceTo(screenPos);
						if (newDistance < distance) {
							CVector bonePos1 = ped->GetBonePosition(aimBone);
							if (!vars::aim::ignoreWalls && !isPointVisible(bonePos1))
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

		int getTargetedBone(CVector localPos, CPed* ped) {
#ifdef DEBUG
			debug("getTargetedBone() \n");
#endif // DEBUG

			if (vars::aim::aimTarget == vars::aim::items[0]) // head
				return Misc->BoneIndexByName(ped, mainBones[Bones::BONE_HEAD]);
			if (vars::aim::aimTarget == vars::aim::items[1]) // nearest bone
				return getNearestBone(localPos, ped);
			if (vars::aim::aimTarget == vars::aim::items[2]) // body
				return Misc->BoneIndexByName(ped, mainBones[Bones::BONE_SPINE1]);
			return -1;
		}

	int getNearestBone(CVector localPos, CPed* ped) { // получение ближайшей к прицелу(середина экрана) кости
		
#ifdef DEBUG
		debug("getNearestBone() \n");
#endif // DEBUG

		int nearestBone = -1;
		float distance = 99999;


		auto studioHdr = Interfaces.ModelInfo->GetStudiomodel(ped->m_modelInfo);

		for (int boneIndex = 0; boneIndex < studioHdr->numbones; boneIndex++) {

			auto bone = studioHdr->pBone(boneIndex);
			if (bone && bone->parent >= 0 && bone->flags & 256) {

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
	
public:

	void ShowTarget() {
#ifdef DEBUG
		debug("ShowTarget() \n");
#endif // DEBUG

		if (!localPed)
			return;
		CVector localPos = localPed->m_fPos + localPed->m_fViewOffset;
		int boneId;
		CPed* ped = getBoneAndNearestPed(localPos, boneId);
		if (ped && isPedInFOV(ped)) {
			CVector2D screenPos;
			CVector bonePos = ped->GetBonePosition(boneId);
			if (Misc->WorldToScreen(bonePos, screenPos)) {
				ImDrawList* draw = ImGui::GetBackgroundDrawList();
				draw->AddLine(ImVec2((float)vars::resX / 2.0f, 0), ImVec2((float)screenPos.x, (float)screenPos.y), 0x77FF1E00); // blue
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