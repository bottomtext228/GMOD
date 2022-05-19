struct CMDButtons {
	bool IN_ATTACK : 1;			 // (1 << 0)
	bool IN_JUMP : 1;			 // (1 << 1)
	bool IN_DUCK : 1;			 // (1 << 2)
	bool IN_FORWARD : 1;		 // (1 << 3)
	bool IN_BACK : 1;			 // (1 << 4)
	bool IN_USE : 1;			 // (1 << 5)
	bool IN_CANCEL : 1;			 // (1 << 6)
	bool IN_LEFT : 1;			 // (1 << 7)
	bool IN_RIGHT : 1;			 // (1 << 8)
	bool IN_MOVELEFT : 1;		 // (1 << 9)
	bool IN_MOVERIGHT : 1;		 // (1 << 10)
	bool IN_ATTACK2 : 2;		 // (1 << 11)
	bool IN_RUN : 1;			 // (1 << 12)
	bool IN_RELOAD : 1;			 // (1 << 13)
	bool IN_ALT1 : 1;			 // (1 << 14)
	bool IN_ALT2 : 1;			 // (1 << 15)
	bool IN_SCORE : 1;			 // (1 << 16)
	bool IN_SPEED : 1;			 // (1 << 17)
	bool IN_WALK : 1;			 // (1 << 18)
	bool IN_ZOOM : 1;			 // (1 << 19)
	bool IN_WEAPON1 : 1;		 // (1 << 20)
	bool IN_WEAPON2 : 1;		 // (1 << 21)
	bool IN_BULLRUSH : 1;		 // (1 << 22)
	bool IN_GRENADE1 : 1;		 // (1 << 23)
	bool IN_GRENADE2 : 1;		 // (1 << 24)
	bool IN_ATTACK3 : 1;		 // (1 << 25)
};



class CUserCmd {
public:
	int		m_cmd_nr;
	int		m_tick_count;
	CVector	m_viewangles;
	float	m_forwardmove;
	float	m_sidemove;
	float	m_upmove;
	union {
		int m_ibuttons;
		CMDButtons m_buttons;
	};

	uint8_t m_impulse;
	int		m_weaponselect;
	int		m_weaponsubtype;
	int		m_random_seed;
	short	m_mousedx;
	short	m_mousedy;
	bool	m_predicted;
};




/*

#define IN_ATTACK					(1 << 0)
#define IN_JUMP						(1 << 1)
#define IN_DUCK						(1 << 2)
#define IN_FORWARD					(1 << 3)
#define IN_BACK						(1 << 4)
#define IN_USE						(1 << 5)
#define IN_CANCEL					(1 << 6)
#define IN_LEFT						(1 << 7)
#define IN_RIGHT					(1 << 8)
#define IN_MOVELEFT					(1 << 9)

#define IN_MOVERIGHT				(1 << 10)
#define IN_ATTACK2					(1 << 11)
#define IN_RUN						(1 << 12)
#define IN_RELOAD					(1 << 13)
#define IN_ALT1						(1 << 14)
#define IN_ALT2						(1 << 15)
#define IN_SCORE					(1 << 16)
#define IN_SPEED					(1 << 17)
#define IN_WALK						(1 << 18)
#define IN_ZOOM						(1 << 19)
#define IN_WEAPON1					(1 << 20)
#define IN_WEAPON2					(1 << 21)
#define IN_BULLRUSH					(1 << 22)
#define IN_GRENADE1					(1 << 23)
#define IN_GRENADE2					(1 << 24)
#define	IN_ATTACK3					(1 << 25)

*/

