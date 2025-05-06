class CMenu {
public:

	void Render() {
		if (vars::menu) {
			ImGui::SetNextWindowPos(ImVec2((float)vars::resX / 2.0f, (float)vars::resY / 2.0f), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(450.0, 350.0), ImGuiCond_FirstUseEver);
			static int currentTab = 0;
			constexpr int tabs = 3;
			enum mainTabs {
				TAB_VISUALS,
				TAB_AIM,
				TAB_MISCELLANEOUS
			};
			enum miscTabs {
				TAB_FRIENDS,
				TAB_LUA,
				TAB_ENTITIES,
				TAB_MISC
			};

			if (ImGui::Begin("GMOD Cheat", &vars::menu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings)) {

				ImVec2 cursorPos = ImGui::GetCursorPos();

				static ImVec2 sectionChildSize = { 150.0f, 75.0f * tabs }; // говно говнище

				ImGui::BeginChild("Sections", { 150.0f, 73.0f * tabs }, true); // чуть меньше чтобы всё было ровно

				DrawVisuals(sectionChildSize, tabs);
				if (ImGui::Button("##Visuals", ImVec2(sectionChildSize.x - 16.0f, sectionChildSize.y / tabs - 10.0f)))
					currentTab = mainTabs::TAB_VISUALS;

				static ImU32 sightColor = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
				DrawSight(sectionChildSize, tabs, sightColor);
				if (ImGui::Button("##Aim", ImVec2(sectionChildSize.x - 16.0f, sectionChildSize.y / tabs - 10.0f)))
					currentTab = mainTabs::TAB_AIM;
				DrawMisc(sectionChildSize, tabs, 6.0f, 12.0f);
				if (ImGui::Button("##Misc", ImVec2(sectionChildSize.x - 16.0f, sectionChildSize.y / tabs - 10.0f)))
					currentTab = mainTabs::TAB_MISCELLANEOUS;

				ImGui::EndChild();


				ImGui::SetCursorPosY(325);
				ImGui::Text("By bottom_text");
				if (ImGui::IsItemClicked()) {
					system("start https://www.blast.hk/members/217146/");
				}

				ImGui::SetCursorPos(ImVec2(cursorPos.x + 165, cursorPos.y));
				ImVec2 windowSize = ImGui::GetWindowSize();
				ImGui::BeginChild("Functions", ImVec2(windowSize.x - (cursorPos.x + 165) - 10, windowSize.y - 40), true);

				switch (currentTab) {
				case mainTabs::TAB_VISUALS:



					ImGui::Checkbox("Lines", &vars::esp::linesState);


					ImGui::Checkbox("3D BoxESP", &vars::esp::box3DESP);
					ImGui::Checkbox("2D BoxESP", &vars::esp::box2DESP);
					ImGui::Checkbox("Health", &vars::esp::renderHealth);
					ImGui::Checkbox("Max health", &vars::esp::renderMaxHealth);
					ImGui::Checkbox("Game nick", &vars::esp::renderGameNick);
					ImGui::Checkbox("Steam nick", &vars::esp::renderSteamNick);
					ImGui::Checkbox("Current weapon", &vars::esp::renderCurrentWeapon);
					ImGui::Checkbox("All weapons", &vars::esp::renderWeapons);
					ImGui::Checkbox("BoneESP", &vars::esp::boneESP);
					ImGui::Checkbox("Dormant check", &vars::esp::dormantCheck);
					ImGui::Checkbox("Distance check", &vars::esp::distanceCheck);
					ImGui::SliderFloat("Distance", &vars::esp::distance, 1000.0f, 15000.0f);
					ImGui::Checkbox("SetupBones", &vars::esp::setupBones);
					ImGui::SliderInt("Setup delay", &vars::esp::setupBonesDelay, 0, 100);
					ImGui::Checkbox("Entities", &vars::esp::renderEntity);
					ImGui::Checkbox("Specific Entities only", &vars::esp::renderSpecificEntities);
					break;

				case mainTabs::TAB_AIM: {
					ImGui::Checkbox("TriggerBot", &vars::aim::triggerBot);
					ImGui::Checkbox("Hold or Click", &vars::aim::holdOrClick);
					ImGui::Checkbox("No Spread", &vars::aim::noSpread);
					ImGui::Checkbox("Rage", &vars::aim::rage);

					if (ImGui::Checkbox("Silent Aim", &vars::aim::silentAim))
						vars::aim::smoothAim = false; // иначе будут конфликтовать, да и нет смысла всё вместе использовать
					if (ImGui::Checkbox("Smooth Aim", &vars::aim::smoothAim))
						vars::aim::silentAim = false;

					ImGui::Text("Angle speed:");
					ImGui::SliderFloat("##aimAngleSpeed", &vars::aim::aimAngleSpeed, 1.0f, 100.0f);
					ImGui::Text("Smooth steps:");
					ImGui::SliderFloat("##aimSmoothSteps", &vars::aim::aimSmoothSteps, 1.0f, 50.0f);
					ComboWithArrows("Aim mode", vars::aim::currentAimMode, vars::aim::aimModes, IM_ARRAYSIZE(vars::aim::aimModes));
					ComboWithArrows("Aim target", vars::aim::currentAimTarget, vars::aim::aimTargets, IM_ARRAYSIZE(vars::aim::aimTargets));
					ImGui::Checkbox("Render FOV", &vars::aim::renderFOV);
					ImGui::SliderInt("FOV", &vars::aim::FOV, 0, 1000);

					ImGui::Checkbox("Ignore walls", &vars::aim::ignoreWalls);
					ImGui::Checkbox("Show target", &vars::aim::showTarget);

					ImGui::Checkbox("Ignore limbs", &vars::aim::ignoreLimbs);
					ImGui::Checkbox("Sticky Aim", &vars::aim::stickyAim);
					ImGui::Checkbox("Ignore Z", &vars::aim::ignoreZ);
					ComboWithArrows("Target type", vars::aim::currentAimTargetType, vars::aim::targetTypes, IM_ARRAYSIZE(vars::aim::targetTypes));
					ImGui::Hotkey("Keybind:", vars::aim::aimKeyBind, 100.0f);
					break;
				}
				case mainTabs::TAB_MISCELLANEOUS:
					static int currentMiscTab = -1;
					ImGui::Spacing();
					constexpr float miscButtonSizeX = 56.5f;
					constexpr float miscButtonSizeY = 30.0f;
					if (ImGui::Button("Friends", ImVec2(miscButtonSizeX, miscButtonSizeY)))
						currentMiscTab = miscTabs::TAB_FRIENDS;
					ImGui::SameLine();
					if (ImGui::Button("Lua", ImVec2(miscButtonSizeX, miscButtonSizeY)))
						currentMiscTab = miscTabs::TAB_LUA;
					ImGui::SameLine();
					if (ImGui::Button("Entities", ImVec2(miscButtonSizeX, miscButtonSizeY)))
						currentMiscTab = miscTabs::TAB_ENTITIES;
					ImGui::SameLine();
					if (ImGui::Button("Misc", ImVec2(miscButtonSizeX, miscButtonSizeY)))
						currentMiscTab = miscTabs::TAB_MISC;
					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					switch (currentMiscTab) {
					case miscTabs::TAB_FRIENDS:
						FriendList();
						break;
					case miscTabs::TAB_LUA:
						Lua();
						break;
					case miscTabs::TAB_ENTITIES:
						SpecificEntitiesList();
						break;
					case miscTabs::TAB_MISC:
						ImGui::Checkbox("Custom sight", &vars::misc::customSight);
						ImGui::Checkbox("Auto uncuff", &vars::misc::autoUncuff);
						ImGui::Checkbox("Ignore teammates", &vars::misc::ignoreLocalTeam);
						ImGui::Checkbox("Auto Jump", &vars::misc::autoJump);
						ImGui::Checkbox("Edge Jump", &vars::misc::edgeJump);
						ImGui::Hotkey("Edge Jump Key:", vars::misc::edgeJumpKeyBind, 115.0f);
						ImGui::SetCursorPosY(280);
						if (ImGui::Button("Unload")) {
							vars::misc::DLLUnload = true;
						}
						break;
					}

					break;

				}


				ImGui::EndChild();

			}

			ImGui::End();

		}
		/* рендер всяких приколюх */
		if (vars::aim::renderFOV)
			Aim->RenderFOV();
		if (vars::misc::customSight)
			Misc->CustomSight();
		if (vars::aim::showTarget)
			Aim->ShowTarget();
		/*                        */

	}
	bool Init(LPDIRECT3DDEVICE9 pDevice, HWND window) {
		/*
		* this fix IM_ASSERT(g.IO.DisplaySize.x >= 0.0f && g.IO.DisplaySize.y >= 0.0f  && "Invalid DisplaySize value!");
		* if window == NULL then GetWindowRect return говно значения и выдаётся ассерт
		*/
		if (!window) {
			return false;
		}
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
		io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 14, 0, io.Fonts->GetGlyphRangesCyrillic());
		io.Fonts->Build();
		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX9_Init(pDevice);
		setImGuiStyle();
		loadFriendList();
		loadSpecificEntitiesList();
		vars::init = true;
		return true;
	}
private:

	// opens Explorer.exe and allows to pick file, returns content of the file
	static std::string openFile() {
		std::string filename;
		filename.resize(MAX_PATH);

		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFilter = L"Lua files (.lua)" "\0"  "*.LUA" "\0"
			"Any File"   "\0"  "*.*"   "\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFile = (wchar_t*)filename.c_str();
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_CREATEPROMPT | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

		if (!GetOpenFileName(&ofn))
		{
			return "";
		}

		std::ifstream f(ofn.lpstrFile);
		if (!f)
		{
			return "";
		}

		std::string content((std::istreambuf_iterator<char>(f)),
			(std::istreambuf_iterator<char>()));
		return content;
	}


	void Lua() {

		if (ImGui::Button("Dump Lua", ImVec2(251.0f, 0.0f))) {
			LuaManager.RequestDumpingFiles();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		static const char* luaInterfaceType[] = { "Client", "Server", "Menu" };
		static LuaInterfaceType currentType;
		ImGuiStyle& style = ImGui::GetStyle();
		float w = ImGui::CalcItemWidth();
		float spacing = style.ItemInnerSpacing.x;
		float button_sz = ImGui::GetFrameHeight();
		ImGui::PushItemWidth(w - spacing * 2.0f - button_sz * 2.0f);

		if (ImGui::BeginCombo("Lua Interface", luaInterfaceType[(int)currentType], ImGuiComboFlags_NoArrowButton)) {
			for (int n = 0; n < IM_ARRAYSIZE(luaInterfaceType); n++) {
				bool is_selected = ((int)currentType == n);
				if (ImGui::Selectable(luaInterfaceType[n], is_selected))
					currentType = (LuaInterfaceType)n;
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::PopItemWidth();

		static std::mutex bufferMutex;
		// 267 size child
		if (ImGui::Button("Browse files", ImVec2(122.f, 0.0f)) && Interfaces.Engine->IsInGame()) {

			std::thread thread([] {
				std::string fileContent = openFile();
				if (!fileContent.empty()) {
					LuaManager.PushToQueue(currentType, fileContent);
				}
				});

			thread.detach();

		}

		ImGui::SameLine();

		static char inputBuffer[32768];

		if (ImGui::Button("Execute", ImVec2(122.0f, 0.0f))) {
			LuaManager.PushToQueue(currentType, std::string(inputBuffer));
		}

		ImGui::InputTextMultiline("", inputBuffer, sizeof(inputBuffer), ImVec2(252.0f, 160.0f));
	}

	void SpecificEntitiesList() {

		static char inputBuffer[128];

		ImGui::InputText("Entity list", inputBuffer, 128);

		if (ImGui::Button("Add entity")) {
			addEntity(inputBuffer);
		}

		if (vars::misc::specificEntities > 0) {
			ImGui::SameLine();
			if (ImGui::Button("Delete")) {
				deleteLastEntity();
			}
		}

		ImGui::Spacing();
		ImGui::Separator();

		for (int i = 0; i < vars::misc::specificEntities; i++) {
			ImGui::Text(vars::misc::specificEntitiesList[i]);
			ImGui::Separator();
		}

		ImGui::Spacing();

		static char selectedObjectName[128] = "none";
		ImGui::Text("Selected entity: %s", selectedObjectName);
		if (ImGui::Button("Add entity##")) {
			addEntity(selectedObjectName);
		}

		ImGui::BeginChild("ObjectList", ImVec2(252, 100), true, ImGuiWindowFlags_HorizontalScrollbar);
		for (auto& object : getObjects()) {

			CPed* entity = object.first;
			if (entity->GetModelInfo()) { // не смотря на проверку в getObjects() даже здесь может быть nullptr, судя по отладчику visual studio 
				int entityIndex = object.second;
				char selectableText[128];
				sprintf_s(selectableText, "%s##%d", entity->GetModelInfo()->m_ModelName, entityIndex);
				/*	В ImGui в качестве id элемента во всех функциях по типу ImGui::Button() и ImGui::Selectable() используется текст.
				*	Если передавать одинаковый, то последующие элементы с таким же названием не будут нормально работать,
				*	фиксится это через ## + любые символы. Они не будут отображаться, но работоспособность вернётся
				*/
				if (ImGui::Selectable(selectableText))
					strcpy(selectedObjectName, entity->GetModelInfo()->m_ModelName);
			}

		}

		ImGui::EndChild();
	}

	void loadSpecificEntitiesList() {
		std::ifstream loadSpecificEntitiesListFile("SpecificEntitiesList.txt");
		if (loadSpecificEntitiesListFile.is_open()) {
			std::string buff;
			while (getline(loadSpecificEntitiesListFile, buff)) {

				strcpy_s(vars::misc::specificEntitiesList[vars::misc::specificEntities], buff.c_str());
				vars::misc::specificEntities++;
			}
			loadSpecificEntitiesListFile.close();
		}
	}
	void deleteLastEntity() {
		memset(vars::misc::specificEntitiesList[vars::misc::specificEntities], 0, 128);
		std::ofstream deleteEntity("SpecificEntitiesList.txt");
		for (int currentEntity = 0; currentEntity < vars::misc::specificEntities - 1; currentEntity++) {
			deleteEntity << vars::misc::friendList[currentEntity] << std::endl;
		}
		deleteEntity.close();
		vars::misc::specificEntities--;
	}

	void addEntity(const char* entityName) {
		bool isAdded = false;
		for (int i = 0; i < vars::misc::specificEntities; i++) {
			if (!strcmp(vars::misc::specificEntitiesList[i], entityName)) {
				isAdded = true;
			}
		}
		if (!isAdded) {
			strcpy(vars::misc::specificEntitiesList[vars::misc::specificEntities], entityName);
			vars::misc::specificEntities++;

			std::ofstream saveEntity("SpecificEntitiesList.txt");
			for (int currentEntity = 0; currentEntity < vars::misc::specificEntities; currentEntity++) {
				saveEntity << vars::misc::specificEntitiesList[currentEntity] << std::endl;
			}

			saveEntity.close();
		}
	}

	void FriendList() {

		static char inputBuffer[128];

		ImGui::InputText("Friend list", inputBuffer, 128);

		if (ImGui::Button("Add friend")) {
			addFriend(inputBuffer);
		}

		if (vars::misc::friendPlayers > 0) {
			ImGui::SameLine();
			if (ImGui::Button("Delete")) {
				deleteLastFriend();

			}
		}

		ImGui::Spacing();
		ImGui::Separator();

		for (int i = 0; i < vars::misc::friendPlayers; i++) {
			ImGui::Text(vars::misc::friendList[i]);
			ImGui::Separator();

		}

		ImGui::Spacing();
		static char selectedPlayerName[128] = "none";
		ImGui::Text("Selected player: %s", selectedPlayerName);
		if (ImGui::Button("Add friend##")) {
			addFriend(selectedPlayerName);
		}

		ImGui::BeginChild("PlayerList", ImVec2(252, 100), true);

		for (auto& player : getPlayers()) {

			char playerNick[128];
			CPed* playerPed = player.first;
			int playerIndex = player.second;
			Interfaces.Engine->GetPlayerNick(playerNick, playerIndex);

			if (ImGui::Selectable(playerNick)) {

				strcpy(selectedPlayerName, playerNick);
			}



		}

		ImGui::EndChild();
	}

	std::vector<std::pair<CPed*, int>> getPlayers() {
		std::vector<std::pair<CPed*, int>> playersList;
		for (int entityIndex = 0; entityIndex < Interfaces.ClientEntityList->GetHighestEntityIndex(); entityIndex++) {
			CPed* entity = Interfaces.ClientEntityList->GetClientEntity(entityIndex);
			if (entity && entity != localPed) {
				if (entity->IsPlayer())
					playersList.push_back(std::make_pair(entity, entityIndex));
			}
		}
		return playersList;
	}

	std::vector<std::pair<CPed*, int>> getObjects() {

		std::vector<std::pair<CPed*, int>> objectsList;
		for (int entityIndex = 0; entityIndex <= Interfaces.ClientEntityList->GetHighestEntityIndex(); entityIndex++) {
			CPed* entity = Interfaces.ClientEntityList->GetClientEntity(entityIndex);
			if (entity && entity != localPed) {
				if (!entity->IsPlayer() && entity->GetModelInfo()) // GetModelInfo() can be nullptr
					objectsList.push_back(std::make_pair(entity, entityIndex));

			}
		}
		return objectsList;

	}

	void deleteLastFriend() {
		memset(vars::misc::friendList[vars::misc::friendPlayers], 0, 128);
		std::ofstream deleteFriend("friendList.txt");
		for (int currentPlayer = 0; currentPlayer < vars::misc::friendPlayers - 1; currentPlayer++) {
			deleteFriend << vars::misc::friendList[currentPlayer] << std::endl;
		}
		deleteFriend.close();
		vars::misc::friendPlayers--;
	}

	void addFriend(const char* friendName) {
		bool isAdded = false;
		for (int i = 0; i < vars::misc::friendPlayers; i++) {
			if (!strcmp(vars::misc::friendList[i], friendName)) {
				isAdded = true;
			}
		}
		if (!isAdded) {
			strcpy(vars::misc::friendList[vars::misc::friendPlayers], friendName);
			vars::misc::friendPlayers++;

			std::ofstream saveFriend("friendList.txt");
			for (int currentPlayer = 0; currentPlayer < vars::misc::friendPlayers; currentPlayer++) {
				saveFriend << vars::misc::friendList[currentPlayer] << std::endl;
			}

			saveFriend.close();
		}
	}

	void loadFriendList() {
		std::ifstream loadFriendList("friendList.txt");
		if (loadFriendList.is_open()) {
			std::string buff;
			while (getline(loadFriendList, buff)) {

				strcpy_s(vars::misc::friendList[vars::misc::friendPlayers], buff.c_str());
				vars::misc::friendPlayers++;
			}
			loadFriendList.close();
		}
	}
	void DrawMisc(ImVec2 sectionChildSize, int tabs, float size, float distance) {
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		cursorPos.x += (sectionChildSize.x - 16) / 2;
		cursorPos.y += (sectionChildSize.y - 10) / (tabs * 2);
		ImDrawList* draw = ImGui::GetForegroundDrawList();
		draw->AddCircleFilled(cursorPos, size, -1, 36);
		draw->AddCircleFilled(ImVec2(cursorPos.x - distance, cursorPos.y - distance), size, -1, 36);
		draw->AddCircleFilled(ImVec2(cursorPos.x + distance, cursorPos.y + distance), size, -1, 36);
	}
	void DrawVisuals(ImVec2 sectionChildSize, int tabs) { // eye 

		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		cursorPos.x += (sectionChildSize.x - 16) / 2;
		cursorPos.y += (sectionChildSize.y - 10) / (tabs * 2);
		ImDrawList* draw = ImGui::GetForegroundDrawList();
		AddEllipse(cursorPos, ImVec2(30.0f, 25.0f), ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), 1.0f);
		draw->AddCircle(cursorPos, 10.0f, -1, 36, 3.0f);
		draw->AddCircleFilled(cursorPos, 4.0f, ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));
	}

	void AddEllipse(ImVec2 cursorPos, ImVec2 ellipseSize, ImU32 color, float thickness) {
		ImDrawList* draw = ImGui::GetForegroundDrawList();
		draw->AddBezierCurve(ImVec2(cursorPos.x - ellipseSize.x, cursorPos.y), ImVec2(cursorPos.x - ellipseSize.x, cursorPos.y - ellipseSize.y),
			ImVec2(cursorPos.x + ellipseSize.x, cursorPos.y - ellipseSize.y), ImVec2(cursorPos.x + ellipseSize.x, cursorPos.y), color, thickness);
		draw->AddBezierCurve(ImVec2(cursorPos.x - ellipseSize.x, cursorPos.y), ImVec2(cursorPos.x - ellipseSize.x, cursorPos.y + ellipseSize.y),
			ImVec2(cursorPos.x + ellipseSize.x, cursorPos.y + ellipseSize.y), ImVec2(cursorPos.x + ellipseSize.x, cursorPos.y), color, thickness);
	}

	void DrawSight(ImVec2 sectionChildSize, int tabs, ImU32 color) {
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		cursorPos.x += (sectionChildSize.x - 16) / 2;
		cursorPos.y += (sectionChildSize.y - 10) / (tabs * 2);

		ImDrawList* draw = ImGui::GetForegroundDrawList();
		constexpr float sightSize = 15.0f;
		draw->AddCircle(cursorPos, sightSize, color, 36);

		draw->AddLine(ImVec2(cursorPos.x, cursorPos.y - (sightSize / 2.0f)), ImVec2(cursorPos.x, cursorPos.y - (sightSize / 2.0f + sightSize)), color, 1.0f); // up
		draw->AddLine(ImVec2(cursorPos.x, cursorPos.y + (sightSize / 2.0f)), ImVec2(cursorPos.x, cursorPos.y + (sightSize / 2.0f + sightSize)), color, 1.0f); // down
		draw->AddLine(ImVec2(cursorPos.x - (sightSize / 2.0f + sightSize), cursorPos.y), ImVec2(cursorPos.x - (sightSize / 2.0f), cursorPos.y), color, 1.0f); // left
		draw->AddLine(ImVec2(cursorPos.x + (sightSize / 2.0f + sightSize), cursorPos.y), ImVec2(cursorPos.x + (sightSize / 2.0f), cursorPos.y), color, 1.0f); // right

		draw->AddCircleFilled(cursorPos, sightSize / 5, color, 36);

	}

	void ComboWithArrows(const char* name, const char*& currentValue, const char** values, int arraySize) {
		ImGuiStyle& style = ImGui::GetStyle();
		float w = ImGui::CalcItemWidth();
		float spacing = style.ItemInnerSpacing.x;
		float button_sz = ImGui::GetFrameHeight();
		ImGui::PushItemWidth(w - spacing * 2.0f - button_sz * 2.0f);
		if (ImGui::BeginCombo(name, currentValue, ImGuiComboFlags_NoArrowButton))
		{
			for (int n = 0; n < arraySize; n++)
			{
				bool is_selected = (currentValue == values[n]);
				if (ImGui::Selectable(values[n], is_selected))
					currentValue = values[n];
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
		ImGui::SameLine(0, spacing);
		char uniqueArrowButtonId[64];
		strcat(uniqueArrowButtonId, name);
		if (ImGui::ArrowButton(strcat(uniqueArrowButtonId, "##l"), ImGuiDir_Left)) {
			if (currentValue == values[0])
				currentValue = values[arraySize - 1];
			else {
				for (int n = 1; n < arraySize; n++)
				{
					bool is_selected = (currentValue == values[n]);
					if (is_selected) {
						currentValue = values[n - 1];
						break;
					}
				}
			}
		}
		ImGui::SameLine(0, spacing);
		if (ImGui::ArrowButton(strcat(uniqueArrowButtonId, "##r"), ImGuiDir_Right)) {
			if (currentValue == values[arraySize - 1])
				currentValue = values[0];
			else {
				for (int n = 0; n < (arraySize - 1); n++)
				{
					bool is_selected = (currentValue == values[n]);
					if (is_selected) {
						currentValue = values[n + 1];
						break;
					}
				}
			}
		}
	}

	void AimTargetCombo() {
		/* ImGui::Combo xd*/
		ImGuiStyle& style = ImGui::GetStyle();
		float w = ImGui::CalcItemWidth();
		float spacing = style.ItemInnerSpacing.x;
		float button_sz = ImGui::GetFrameHeight();
		ImGui::PushItemWidth(w - spacing * 2.0f - button_sz * 2.0f);
		if (ImGui::BeginCombo("Aim target", vars::aim::currentAimTarget, ImGuiComboFlags_NoArrowButton))
		{
			for (int n = 0; n < IM_ARRAYSIZE(vars::aim::aimTargets); n++)
			{
				bool is_selected = (vars::aim::currentAimTarget == vars::aim::aimTargets[n]);
				if (ImGui::Selectable(vars::aim::aimTargets[n], is_selected))
					vars::aim::currentAimTarget = vars::aim::aimTargets[n];
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
		ImGui::SameLine(0, spacing);
		if (ImGui::ArrowButton("##l", ImGuiDir_Left)) {
			int arraySize = IM_ARRAYSIZE(vars::aim::aimTargets);
			if (vars::aim::currentAimTarget == vars::aim::aimTargets[0])
				vars::aim::currentAimTarget = vars::aim::aimTargets[arraySize - 1];
			else {
				for (int n = 1; n < arraySize; n++)
				{
					bool is_selected = (vars::aim::currentAimTarget == vars::aim::aimTargets[n]);
					if (is_selected) {
						vars::aim::currentAimTarget = vars::aim::aimTargets[n - 1];
						break;
					}
				}
			}
		}
		ImGui::SameLine(0, spacing);
		if (ImGui::ArrowButton("##r", ImGuiDir_Right)) {
			int arraySize = IM_ARRAYSIZE(vars::aim::aimTargets);
			if (vars::aim::currentAimTarget == vars::aim::aimTargets[arraySize - 1])
				vars::aim::currentAimTarget = vars::aim::aimTargets[0];
			else {
				for (int n = 0; n < (arraySize - 1); n++)
				{
					bool is_selected = (vars::aim::currentAimTarget == vars::aim::aimTargets[n]);
					if (is_selected) {
						vars::aim::currentAimTarget = vars::aim::aimTargets[n + 1];
						break;
					}
				}
			}
		}
	}

	void AimTargetTypeCombo() {
		/* ImGui::Combo xd*/
		ImGuiStyle& style = ImGui::GetStyle();
		float w = ImGui::CalcItemWidth();
		float spacing = style.ItemInnerSpacing.x;
		float button_sz = ImGui::GetFrameHeight();
		ImGui::PushItemWidth(w - spacing * 2.0f - button_sz * 2.0f);
		if (ImGui::BeginCombo("Target type", vars::aim::currentAimTargetType, ImGuiComboFlags_NoArrowButton)) {
			for (int n = 0; n < IM_ARRAYSIZE(vars::aim::targetTypes); n++)
			{
				bool is_selected = (vars::aim::currentAimTargetType == vars::aim::targetTypes[n]);
				if (ImGui::Selectable(vars::aim::targetTypes[n], is_selected))
					vars::aim::currentAimTargetType = vars::aim::targetTypes[n];
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::PopItemWidth();
		ImGui::SameLine(0, spacing);

		if (ImGui::ArrowButton("###l", ImGuiDir_Left)) {
			int arraySize = IM_ARRAYSIZE(vars::aim::targetTypes);
			if (vars::aim::currentAimTargetType == vars::aim::targetTypes[0])
				vars::aim::currentAimTargetType = vars::aim::targetTypes[arraySize - 1];

			else {
				for (int n = 1; n < arraySize; n++)
				{
					bool is_selected = (vars::aim::currentAimTargetType == vars::aim::targetTypes[n]);
					if (is_selected) {
						vars::aim::currentAimTargetType = vars::aim::targetTypes[n - 1];
						break;
					}
				}
			}
		}
		ImGui::SameLine(0, spacing);
		if (ImGui::ArrowButton("###r", ImGuiDir_Right))
		{
			int arraySize = IM_ARRAYSIZE(vars::aim::targetTypes);
			if (vars::aim::currentAimTargetType == vars::aim::targetTypes[arraySize - 1])
				vars::aim::currentAimTargetType = vars::aim::targetTypes[0];
			else {
				for (int n = 0; n < (arraySize - 1); n++)
				{
					bool is_selected = (vars::aim::currentAimTargetType == vars::aim::targetTypes[n]);
					if (is_selected) {
						vars::aim::currentAimTargetType = vars::aim::targetTypes[n + 1];
						break;
					}
				}
			}
		}
	}

	void setImGuiStyle() {
		ImGuiStyle& style = ImGui::GetStyle();
		style.Alpha = 1.0;
		style.ChildRounding = 3.0f;
		style.WindowRounding = 3.0f;
		style.GrabRounding = 5.0f;
		style.GrabMinSize = 8.0f;
		style.FrameRounding = 3.0f;
		style.FrameBorderSize = 1.0f;
		style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.00f, 0.40f, 0.41f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 1.00f, 1.00f, 0.65f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.44f, 0.80f, 0.80f, 0.18f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.44f, 0.80f, 0.80f, 0.27f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.44f, 0.81f, 0.86f, 0.66f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.18f, 0.21f, 0.73f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.13f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.22f, 0.29f, 0.30f, 0.71f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.44f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 1.00f, 0.68f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.36f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.00f, 0.65f, 0.65f, 0.46f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.01f, 1.00f, 1.00f, 0.43f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.62f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 1.00f, 1.00f, 0.33f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.42f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 1.00f, 1.00f, 0.22f);
		style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.04f, 0.10f, 0.09f, 0.51f);
	}
};

CMenu* Menu;

