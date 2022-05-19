class CBunnyHop {
public:
	void Process(CUserCmd* CMD) {
		if (CMD->m_buttons.IN_JUMP && localPed->m_iMoveType != localPed->MOVETYPE_NOCLIP && localPed->m_iMoveType != localPed->MOVETYPE_LADDER) {
			if (!localPed->m_flags.ON_GROUND && !localPed->m_flags.IN_WATER)
				CMD->m_buttons.IN_JUMP = false;
		}
	}
};