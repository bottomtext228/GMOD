

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
				TAB_BHOP,
				TAB_ENTITIES,
				TAB_MISC

			};

			if (ImGui::Begin("GMOD Cheat", &vars::menu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings)) {

				ImVec2 cursorPos = ImGui::GetCursorPos();

				static ImVec2 sectionChildSize = { 150.0f, 75.0f * tabs }; // ????? ???????

				ImGui::BeginChild("Sections", { 150.0f, 73.0f * tabs }, true); // ???? ?????? ????? ??? ???? ?????

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
		
				/*
				static char inputBuffer[128];
				ImGui::InputText("CMD executor", inputBuffer, 128);
				if (ImGui::Button("Execute")) {
					Interfaces.Engine->ExecuteClientCmd(inputBuffer);
				}
				*/
			
				ImGui::SetCursorPosY(325);
				ImGui::Text("By bottom_text");
				if (ImGui::IsItemClicked()) {
					system("start https://www.blast.hk/members/217146/");
				}
		
				ImGui::SetCursorPos(ImVec2(cursorPos.x + 165, cursorPos.y));
				ImVec2 windowSize = ImGui::GetWindowSize();
				ImGui::BeginChild("Functions", ImVec2(windowSize.x - (cursorPos.x + 165) - 10, windowSize.y - 40), true);
				if (currentTab == mainTabs::TAB_VISUALS) {


					ImGui::Checkbox("Lines", &vars::esp::linesState);

		
					ImGui::Checkbox("3D BoxESP", &vars::esp::box3DESP);
					ImGui::Checkbox("2D BoxESP", &vars::esp::box2DESP);
					ImGui::Checkbox("Health and nicks", &vars::esp::renderHealthAndNick);
					ImGui::Checkbox("BoneESP", &vars::esp::boneESP);
					ImGui::Checkbox("SetupBones", &vars::esp::setupBones);
					ImGui::SliderInt("Setup delay", &vars::esp::setupBonesDelay, 0, 100);
					ImGui::Checkbox("Entities", &vars::esp::renderEntity);
					ImGui::Checkbox("Specific Entities only", &vars::esp::renderSpecificEntities);

				}
				else if (currentTab == mainTabs::TAB_AIM) {
					ImGui::Checkbox("TriggerBot", &vars::aim::triggerBot);
					ImGui::Checkbox("Hold or Click", &vars::aim::holdOrClick);
					ImGui::Checkbox("No Spread", &vars::aim::noSpread);
					ImGui::Checkbox("Rage", &vars::aim::rage);

					if (ImGui::Checkbox("Silent Aim", &vars::aim::silentAim))
						vars::aim::smoothAim = false; // ????? ????? ?????????????, ?? ? ??? ?????? ??? ?????? ????????????
					if (ImGui::Checkbox("Smooth Aim", &vars::aim::smoothAim))
						vars::aim::silentAim = false;


					ImGui::SliderFloat("AimForce", &vars::aim::aimForce, 0.0, 1.0);

					Combo();
					ImGui::Checkbox("Render FOV", &vars::aim::renderFOV);
					ImGui::SliderInt("FOV", &vars::aim::FOV, 0, 1000);
			
					ImGui::Checkbox("Ignore walls", &vars::aim::ignoreWalls);
					ImGui::Checkbox("Show target", &vars::aim::showTarget);

				}
				else if (currentTab == mainTabs::TAB_MISCELLANEOUS) {
					static int currentMiscTab = -1;
					ImGui::Spacing();
					constexpr float miscButtonSizeX = 56.5f;
					constexpr float miscButtonSizeY = 30.0f;
					if (ImGui::Button("Friends", ImVec2(miscButtonSizeX, miscButtonSizeY)))
						currentMiscTab = miscTabs::TAB_FRIENDS;
					ImGui::SameLine();
					if (ImGui::Button("BHop", ImVec2(miscButtonSizeX, miscButtonSizeY)))
						currentMiscTab = miscTabs::TAB_BHOP;
					ImGui::SameLine();
					if (ImGui::Button("Entities", ImVec2(miscButtonSizeX, miscButtonSizeY)))
						currentMiscTab = miscTabs::TAB_ENTITIES;
					ImGui::SameLine();
					if (ImGui::Button("Misc", ImVec2(miscButtonSizeX, miscButtonSizeY)))
						currentMiscTab = miscTabs::TAB_MISC;
					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					if (currentMiscTab == miscTabs::TAB_FRIENDS) {
						FriendList();
					}
					else if (currentMiscTab == miscTabs::TAB_BHOP) {
						ImGui::Checkbox("Auto Jump", &vars::bhop::autoJump);
					}
					else if (currentMiscTab == miscTabs::TAB_ENTITIES) {
						SpecificEntitiesList();
					}
					else if (currentMiscTab == miscTabs::TAB_MISC) {
						ImGui::Checkbox("Custom sight", &vars::misc::customSight);
						ImGui::Checkbox("Auto uncuff", &vars::misc::autoUncuff);
						ImGui::Checkbox("Ignore teammates", &vars::misc::ignoreLocalTeam);

						ImGui::SetCursorPosY(280);
						if (ImGui::Button("Unload")) {
							vars::misc::DLLUnload = true;
						}
					}


				}


				ImGui::EndChild();

			}

			ImGui::End();

		}
		/* ?????? ?????? ???????? */
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
		* if window == NULL then GetWindowRect return ????? ???????? ? ???????? ??????
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
			if (entity->m_modelInfo) { // ?? ?????? ?? ???????? ? getObjects() ???? ????? ????? ???? nullptr, ???? ?? ????????? visual studio 
				int entityIndex = object.second;
				char selectableText[128];
				sprintf_s(selectableText, "%s##%d", entity->m_modelInfo->m_ModelName, entityIndex);
				/*	? ImGui ? ???????? id ???????? ?? ???? ???????? ?? ???? ImGui::Button() ? ImGui::Selectable() ???????????? ?????.
				*	???? ?????????? ??????????, ?? ??????????? ???????? ? ????? ?? ????????? ?? ????? ????????? ????????,
				*	???????? ??? ????? ## + ????? ???????. ??? ?? ????? ????????????, ?? ????????????????? ????????
				*/
				if (ImGui::Selectable(selectableText))
					strcpy(selectedObjectName, entity->m_modelInfo->m_ModelName);
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
				if (entity->isEntityIsPlayer())
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
				if (!entity->isEntityIsPlayer() && entity->m_modelInfo) // m_modelInfo can be nullptr
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
		cursorPos.y += (sectionChildSize.y - 10) / (tabs * 2) ;

		ImDrawList* draw = ImGui::GetForegroundDrawList();
		constexpr float sightSize = 15.0f;
		draw->AddCircle(cursorPos, sightSize, color, 36);
		
		draw->AddLine(ImVec2(cursorPos.x, cursorPos.y - (sightSize / 2.0f)), ImVec2(cursorPos.x, cursorPos.y - (sightSize / 2.0f + sightSize)), color, 1.0f); // up
		draw->AddLine(ImVec2(cursorPos.x, cursorPos.y + (sightSize / 2.0f)), ImVec2(cursorPos.x, cursorPos.y + (sightSize / 2.0f + sightSize)), color, 1.0f); // down
		draw->AddLine(ImVec2(cursorPos.x - (sightSize / 2.0f + sightSize), cursorPos.y), ImVec2(cursorPos.x - (sightSize / 2.0f), cursorPos.y), color, 1.0f); // left
		draw->AddLine(ImVec2(cursorPos.x + (sightSize / 2.0f + sightSize), cursorPos.y), ImVec2(cursorPos.x + (sightSize / 2.0f), cursorPos.y), color, 1.0f); // right
		
		draw->AddCircleFilled(cursorPos, sightSize / 5, color, 36);
		 
	}
	void Combo() {
		/* ImGui::Combo xd*/
		ImGuiStyle& style = ImGui::GetStyle();
		float w = ImGui::CalcItemWidth();
		float spacing = style.ItemInnerSpacing.x;
		float button_sz = ImGui::GetFrameHeight();
		ImGui::PushItemWidth(w - spacing * 2.0f - button_sz * 2.0f);
		if (ImGui::BeginCombo("Aim target", vars::aim::aimTarget, ImGuiComboFlags_NoArrowButton))
		{
			for (int n = 0; n < IM_ARRAYSIZE(vars::aim::items); n++)
			{
				bool is_selected = (vars::aim::aimTarget == vars::aim::items[n]);
				if (ImGui::Selectable(vars::aim::items[n], is_selected))
					vars::aim::aimTarget = vars::aim::items[n];
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
		ImGui::SameLine(0, spacing);
		if (ImGui::ArrowButton("##l", ImGuiDir_Left))
		{
			int arraySize = IM_ARRAYSIZE(vars::aim::items);
			if (vars::aim::aimTarget == vars::aim::items[0])
				vars::aim::aimTarget = vars::aim::items[arraySize - 1];

			else {
				for (int n = 1; n < arraySize; n++)
				{
					bool is_selected = (vars::aim::aimTarget == vars::aim::items[n]);
					if (is_selected) {
						vars::aim::aimTarget = vars::aim::items[n - 1];
						break;
					}
				}
			}
		}
		ImGui::SameLine(0, spacing);
		if (ImGui::ArrowButton("##r", ImGuiDir_Right))
		{
			int arraySize = IM_ARRAYSIZE(vars::aim::items);
			if (vars::aim::aimTarget == vars::aim::items[arraySize - 1])
				vars::aim::aimTarget = vars::aim::items[0];
			else {
				for (int n = 0; n < (arraySize - 1); n++)
				{
					bool is_selected = (vars::aim::aimTarget == vars::aim::items[n]);
					if (is_selected) {
						vars::aim::aimTarget = vars::aim::items[n + 1];
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

