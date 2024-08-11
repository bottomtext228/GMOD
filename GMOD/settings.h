namespace vars { 
	bool init = false;

	namespace esp {
		bool linesState = false;
		bool box3DESP = false;
		bool box2DESP = false; 
		bool renderHealthAndNick = false;
		bool renderEntity = false;
		bool renderSpecificEntities = false;
		bool boneESP = false;
		bool setupBones = false;
		int setupBonesDelay = 0;
		bool dormantCheck = false;
		bool distanceCheck = false;
		float distance = 2500.0f;
	};

	

	float viewMatrix[16];
	namespace aim {
		bool showTarget = false;
		CVector newAngles = { 0.0, 0.0, 0.0 }; // мб костыль, для работы сайлента
		bool smoothAim = false;
		bool silentAim = false;
		bool rage = false;
		float aimAngleSpeed = 30.0f; // angles per second
		float aimSmoothSteps = 10.0f;
		const char* aimModes[] = {"Smooth steps", "Angle speed"};
		const char* currentAimMode = aimModes[0];
		int FOV = 200;
		bool renderFOV = false;
		const char* aimTargets[] = { "Head", "Nearest", "Body"};
		const char* targetTypes[] = {"Hitbox", "Bone"};
		bool ignoreLimbs = true;
		const char* currentAimTarget = vars::aim::aimTargets[0];
		const char* currentAimTargetType = vars::aim::targetTypes[0];
		bool ignoreWalls = false;
		bool triggerBot = false;
		bool holdOrClick = false; // false = hold - триггербот зажимает атаку, true - зажимает и отжимает атаку; для корректной работы на 9мм пистолете
		bool noSpread = false;
		KeyBindToggle aimKeyBind = KeyBindToggle(KeyBind::KeyCode::MOUSE5); 
		bool ignoreZ = false;
		bool stickyAim = true;
	};

	namespace bhop {
		bool autoJump = false;
	}
	namespace misc {
		/* Friend List */
		char friendList[20][128];
		int friendPlayers = 0;
		/* Entity list */
		char specificEntitiesList[20][128];
		int specificEntities = 0;
		/* Misc */
		bool customSight = false;
		int sightColor = 0xFF00FF00; // a b g  r
		bool autoUncuff = false;
		bool ignoreLocalTeam = false;
		bool DLLUnload = false;
		bool edgeJump = false;
		KeyBindToggle edgeJumpKeyBind = KeyBindToggle(KeyBind::KeyCode::N);
	}


	bool* bSendPacket = nullptr;
	bool menu = false;
	float boxESPSize = 15.0;
	int resX;
	int resY;
	DWORD client;
	DWORD engine;
};
