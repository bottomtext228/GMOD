
class CTriggerBot {
public:
	void Process(CUserCmd* CMD) {
#if USE_ENGINE_PREDICTION
		CVector localPos = localPed->GetEyePosition();
#else
		CVector localPos = localPed->GetEyePosition() + Misc->predictEntityOffsetPosition(localPed->GetVecVelocity());
#endif // USE_ENGINE_PREDICTION	
		CVector targetDir = {
				localPed->GetVecOrigin().x + MAX_TRACE_LENGTH * cosf(Misc->degreesToRadians(CMD->m_viewangles.y)),
				localPed->GetVecOrigin().y + MAX_TRACE_LENGTH * sinf(Misc->degreesToRadians(CMD->m_viewangles.y)),
				localPed->GetVecOrigin().z + localPed->GetVecViewOffset().z - MAX_TRACE_LENGTH * tanf(Misc->degreesToRadians(CMD->m_viewangles.x))
		};
		trace_t trace = Interfaces.Trace->TraceRayWrapper(localPos, targetDir, localPed); // trace.hitbox == 0 - head maybe;
		CPed* ped = trace.ped;

		if (ped && ped != localPed && ped->IsAlive() && ped->IsPlayer() && !Misc->isPlayerInFriendList(GetPlayerIndexByPed(ped))) {

			if (vars::aim::triggerBot) {
				if (vars::aim::holdOrClick) {
					static bool isInAttack = false;
					CMD->m_buttons.IN_ATTACK = !isInAttack;
					isInAttack = !isInAttack;
				}
				else {
					CMD->m_buttons.IN_ATTACK = true;
				}
			}

			vars::misc::sightColor = 0xFF0000FF; // Red. Игрок в перекрестии прицела, меняем цвет прицела на красный

		}
		else {
			vars::misc::sightColor = 0xFF00FF00; // Green. В прицеле ничего, меняем цвет прицела на зелёный
		}
	}
	int GetPlayerIndexByPed(CPed* playerPed) { // ужас, так как цикл фор на куча итераций и только ради работы френдлиста
		for (int i = 0; i <= Interfaces.ClientEntityList->GetHighestEntityIndex(); i++) {
			CPed* ped = Interfaces.ClientEntityList->GetClientEntity(i);
			if (ped == playerPed)
				return i;
		}
		return -1;
	}
};
