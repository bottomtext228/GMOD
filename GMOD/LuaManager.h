#pragma once

class CLuaManager {
public:
	std::vector<std::pair<LuaInterfaceType, std::string>> m_luaQueue;
	std::mutex m_luaMutex;
	bool m_shouldDumpFiles;
	void PushToQueue(LuaInterfaceType type, std::string code) {
		const std::lock_guard<std::mutex> lock(LuaManager.m_luaMutex);
		m_luaQueue.push_back({ type, code });
	}

	void ExecuteQueue() {
		if (m_shouldDumpFiles) {
			m_shouldDumpFiles = false;
			DumpFiles();
		}

		const std::lock_guard<std::mutex> lock(LuaManager.m_luaMutex);
		for (auto& [type, code] : m_luaQueue) {
			auto pLuaInterface = Interfaces.LuaShared->GetLuaInterface(type);
			if (pLuaInterface) {
				ExecuteCode(pLuaInterface, code);
			}
		}
		m_luaQueue.clear();
	}

	void RequestDumpingFiles() {
		m_shouldDumpFiles = true;
	}

	void DumpFiles() {
		if (!Interfaces.Engine->IsInGame()) return;

		std::filesystem::path serverFolderFullPath;

		static TCHAR path[MAX_PATH + 1];
		SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_DESKTOP, FALSE); // get desktop path

		serverFolderFullPath = path;

		std::string hostname = Utils::GetHostName();
		if (hostname.empty()) return;

		serverFolderFullPath /= "Lua Dumper";
		serverFolderFullPath /= Utils::sanitize_utf8_path(hostname);

#ifdef DEBUG
		std::cout << "[DUMPER]: Dumping to " << serverFolderFullPath.string() << '\n';
#endif
		try {
			std::filesystem::create_directories(Utils::utf8_to_utf16(serverFolderFullPath.string()));
		}
		catch (std::exception e) {
			std::cerr << "Failed to create dump folder:\n" << e.what() << '\n';
			return;
		}

		for (int i = 0; i < Interfaces.LuaShared->m_iFilesInCache; i++) {
			CLuaFile* luaFile = Interfaces.LuaShared->m_pFiles[i];
			if ((uintptr_t)luaFile->m_pContent == 0x2D2D) continue; // I don't know why pointer can be equal to 0x2D2D 
			std::string fileName(luaFile->m_pFileName);
			std::replace(fileName.begin(), fileName.end(), '/', ' ');

#ifdef DEBUG
			std::cout << "[DUMPER]: Lua file: " << fileName << '\n';
#endif
			try {
				std::ofstream file;
				file.open(Utils::utf8_to_utf16((serverFolderFullPath / fileName).string()));
				file << luaFile->m_pContent;
				file.close();
			}
			catch (std::exception e) {
				std::cerr << "Failed to dump file:\n" << e.what() << '\n';
			}
		}

	}

private:
	void ExecuteCode(CLuaInterface* pLuaInterface, std::string code) {
		pLuaInterface->ExecuteCode(code.c_str());
	}

} LuaManager;