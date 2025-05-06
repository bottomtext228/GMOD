
class CTriggerBot {
public:
	void Process(CUserCmd* CMD) {
#if USE_ENGINE_PREDICTION
		CVector localPos = localPed->GetEyePosition();
#else
		CVector localPos = localPed->GetEyePosition() + Utils::predictEntityOffsetPosition(localPed->GetVecVelocity());
#endif // USE_ENGINE_PREDICTION	

		trace_t trace = Interfaces.Trace->TraceRayWrapper(localPos, Utils::GetPointOfSight(localPos, CMD->m_viewangles), localPed); // trace.hitbox == 0 - head maybe;
		CPed* ped = trace.ped;

		if (ped && ped != localPed && ped->IsAlive() && ped->IsPlayer() && !Utils::isPlayerInFriendList(Utils::GetPlayerIndexByPed(ped))) {

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

			vars::misc::sightColor = 0xFF0000FF; // Red. ����� � ����������� �������, ������ ���� ������� �� �������

		}
		else {
			vars::misc::sightColor = 0xFF00FF00; // Green. � ������� ������, ������ ���� ������� �� ������
		}
	}
};
