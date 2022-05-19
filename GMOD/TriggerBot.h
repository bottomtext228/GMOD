
class CTriggerBot {
public:
	void Process(CUserCmd* CMD) {
	
		constexpr float MAX_TRACE_LENGTH = 56754.0f; // https://developer.valvesoftware.com/wiki/UTIL_TraceLine

		CVector localPos = localPed->GetEyePosition() + Misc->predictEntityOffsetPosition(localPed->m_fVelocity);

		CVector targetPos = { localPed->m_fPos.x + MAX_TRACE_LENGTH * cosf(Misc->degreesToRadians(CMD->m_viewangles.y)),
			localPed->m_fPos.y + MAX_TRACE_LENGTH * sinf(Misc->degreesToRadians(CMD->m_viewangles.y)),
			localPed->m_fPos.z + localPed->m_fViewOffset.z - MAX_TRACE_LENGTH * tanf(Misc->degreesToRadians(CMD->m_viewangles.x)) };

		trace_t trace = Interfaces.Trace->TraceRayWrapper(localPos, targetPos, localPed); // trace.hitbox == 0 - head maybe;
		CPed* ped = trace.ped;

		if (ped && ped != localPed && ped->isAlive() && ped->isEntityIsPlayer() && !Misc->isPlayerInFriendList(GetPlayerIndexByPed(ped))) {

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
