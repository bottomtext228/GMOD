#pragma once

class CLuaManager {
public:
	std::vector<std::pair<LuaInterfaceType, std::string>> m_luaQueue;
	std::mutex m_luaMutex;
	void PushToQueue(LuaInterfaceType type, std::string code) {
		const std::lock_guard<std::mutex> lock(LuaManager.m_luaMutex);
		m_luaQueue.push_back({ type, code });
	}

	void ExecuteQueue() {
		const std::lock_guard<std::mutex> lock(LuaManager.m_luaMutex);
		for (auto& [type, code] : m_luaQueue) {
			auto pLuaInterface = Interfaces.LuaShared->GetLuaInterface(type);
			if (pLuaInterface) {
				ExecuteCode(pLuaInterface, code);
			}
		}
		m_luaQueue.clear();
	}


	void DumpFiles() {

		std::filesystem::path serverFolderFullPath;

		static TCHAR path[MAX_PATH + 1];
		SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_DESKTOP, FALSE); // get desktop path

		serverFolderFullPath = path;

		std::string hostname = Utils::GetHostName();
		serverFolderFullPath /= "Lua Dumper";
		serverFolderFullPath /= hostname;

#ifdef DEBUG
		std::cout << "[DUMBER]: Dumping to " << serverFolderFullPath << '\n';
#endif
		
		std::filesystem::create_directories(serverFolderFullPath);

		for (int i = 0; i < Interfaces.LuaShared->m_iFilesInCache; i++) {
			CLuaFile* luaFile = Interfaces.LuaShared->m_pFiles[i];
			std::string fileName(luaFile->m_pFileName);
			std::replace(fileName.begin(), fileName.end(), '/', ' ');

#ifdef DEBUG
			std::cout << "[DUMPER]: Lua file: " << fileName << '\n';
#endif
			std::ofstream file;
			file.open(serverFolderFullPath / fileName);
			file << luaFile->m_pContent;
			file.close();

		}

	}

private:
	void ExecuteCode(CLuaInterface* pLuaInterface, std::string code) {
		pLuaInterface->ExecuteCode(code.c_str());
	}

} LuaManager;