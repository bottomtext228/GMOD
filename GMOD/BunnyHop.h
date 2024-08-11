class CBunnyHop {
public:
	void Process(CUserCmd* CMD) {
		if (CMD->m_buttons.IN_JUMP && localPed->GetMoveType() != localPed->MOVETYPE_NOCLIP && localPed->GetMoveType() != localPed->MOVETYPE_LADDER) {
			if (!(localPed->GetFlags() & FL_ONGROUND) && !(localPed->GetFlags() & FL_INWATER))
				CMD->m_buttons.IN_JUMP = false;
		}
	}
};