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
	};

	

	float viewMatrix[16];
	namespace aim {
		bool showTarget = false;
		CVector newAngles = { 0.0, 0.0, 0.0 }; // мб костыль, для работы сайлента
		bool smoothAim = false;
		bool silentAim = false;
		bool rage = false;
		float aimForce = 0.1f;
		int FOV = 200;
		bool renderFOV = false;
		const char* items[] = { "Head", "Nearest bone", "Body" };
		const char* aimTarget = vars::aim::items[0];
		bool ignoreWalls = false;
		bool triggerBot = false;
		bool holdOrClick = false; // false = hold - триггербот зажимает атаку, true - зажимает и отжимает атаку; для корректной работы на 9мм пистолете
		bool noSpread = false;

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
	}



	bool menu = false;
	float boxESPSize = 15.0;
	int resX;
	int resY;
	DWORD client;
	DWORD engine;
};
