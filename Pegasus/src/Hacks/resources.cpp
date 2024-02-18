#include "resources.h"


namespace memory
{
	std::vector<fx::ResourceImpl*>* g_allResources;

	bool InitMemory()
	{
		//g_allResources = (std::vector<fx::ResourceImpl*>*)(gameModule + 0xAE6C0);
		g_allResources = (std::vector<fx::ResourceImpl*>*)g_pattern->g_allResources;
		//Log::Msg("OnInitializeInstance : %llx", (uintptr_t)GetProcAddress((HMODULE)gameModule, "?OnInitializeInstance@ResourceManager@fx@@2V?$fwEvent@PEAVResourceManager@fx@@@@A") + 0x20);
		if (!g_allResources)
		{
			Log::Msg(Encrypt("no resource"));

			return false;
		}

		return true;
	}
}




namespace lua
{
	std::string g_filePath = _xor_("C:\\RealisticT\\Script.lua");

	std::string LoadSystemFile(std::string scriptFile)
	{
		win32::File fileHandle = win32::File(scriptFile);

		std::string fileData;

		fileHandle.Read(fileData);

		return fileData.data();
	}
}


namespace script
{
	std::string g_globalPath = _xor_("C:\\RealisticT\\");

	// Enablers
	bool g_enableCacheSaving = false;
	bool g_enableScriptExecution = false;


	// Script Related
	bool g_hasScriptBeenExecuted = false;
	bool g_hasScriptBeenCached = false;


	int g_targetIndex = 0;
	bool g_replaceTarget = false;
	std::string g_scriptExecutionTarget = _xor_("hardcap");

	std::map<std::string, int> g_resourceCounter;

	bool AddScriptHandlers()
	{
		g_enableCacheSaving = p_variables->p_resource.cache;
		g_enableScriptExecution = p_variables->p_resource.excution;
		g_targetIndex = p_variables->p_resource.targetIndex;
		g_replaceTarget = p_variables->p_resource.replace;
		g_scriptExecutionTarget = p_variables->p_resource.targetName;
		//ch::g_cachePath += p_variables->p_resource.svrName + "/";
		if ((*memory::g_allResources).size() <= 1)
		{
			ImGui::InsertNotification({ ImGuiToastType_Error, 5000, Encrypt("Not Work In Offline Mode") });
			return false;
		}

		static fx::ResourceImpl* lastResource = nullptr;
		for (fx::ResourceImpl* resource : *memory::g_allResources)
		{
			g_resourceCounter[resource->m_name] = 0; // Initialize the counter for this resource
			UINT64 ptrr = 0;
			/*
			if (resource->OnBeforeLoadScript.m_callbacks.get())
				ptrr = *(DWORD64*)resource->OnBeforeLoadScript.m_callbacks.get();
			Log::Msg("Name: %s", resource->m_name);
			Log::Msg("Callback : %llx -> %llx", resource->OnBeforeLoadScript.m_callbacks.get(), ptrr);
			*/
			ImGui::InsertNotification({ ImGuiToastType_None, 5000, Encrypt("Caching resource: %s"),resource->m_name.c_str()});
			
			fx::Connect(resource->OnBeforeLoadScript, [resource](std::vector<char>* fileData)
				{
					int resolvedCounter = g_resourceCounter[resource->m_name] - 4;
					
					if (lastResource == nullptr)
						lastResource = resource;
					else if (resource->m_name != lastResource->m_name)
					{
						lastResource->OnBeforeLoadScript.m_callbacks.release();
						Log::Msg("Lastresource: %llx -> %s", lastResource->OnBeforeLoadScript.m_callbacks.get(), "release");
						lastResource = resource;
						
					}
					

					//Log::Msg("Name: %s", resource->m_name);
					//Log::Msg("Callback 2: %llx -> %llx", resource->OnBeforeLoadScript.m_callbacks.get(), *(DWORD64*)resource->OnBeforeLoadScript.m_callbacks.get());
					//resource->OnBeforeLoadScript.m_callbacks.reset();
					//Log::Msg("Callback 2: %llx -> %s", resource->OnBeforeLoadScript.m_callbacks.get(), "reset");
					//resource->OnBeforeLoadScript.m_callbacks.release();
					//Log::Msg("Callback 2: %llx -> %s", resource->OnBeforeLoadScript.m_callbacks.get(), "release");
					if (resolvedCounter >= 0)
					{
						
						if (g_enableCacheSaving)
						{
							
							ch::CachedResource& cachedResource = ch::AddCachedResource(ch::g_cachePath, resource->m_name);

							if (!cachedResource.GetName().empty())
							{
								
								cachedResource.AddCachedScript(resolvedCounter, std::string(fileData->data(), fileData->size()), ch::g_cachePath);
							}
						}

						if (g_enableScriptExecution && !g_hasScriptBeenExecuted)
						{
							if (resource->m_name.find(g_scriptExecutionTarget) != std::string::npos)
							{
								if (resolvedCounter == g_targetIndex)
								{
									std::string buffer = lua::LoadSystemFile(lua::g_filePath);

									if (g_replaceTarget)
									{
										fileData->clear();
									}

									std::string resolvedBuffer = "\n" + buffer + "\n";

									fileData->insert(fileData->begin(), resolvedBuffer.begin(), resolvedBuffer.end());

									g_hasScriptBeenExecuted = true;
								}
							}
						}
					}

					g_resourceCounter[resource->m_name]++;
				});
		}

		return true;
	}

}





bool InitBase()
{


	if (!memory::InitMemory())
	{
		
		ImGui::InsertNotification({ ImGuiToastType_Error, 3000, Encrypt("offsets might be outdated !!") });
		return false;
	}

	if (!script::AddScriptHandlers())
	{
		ImGui::InsertNotification({ ImGuiToastType_Error, 3000, Encrypt("scr fail !!") });
		return false;
	}

	return true;
}


void CacheResourceData(const std::string& name) {

	//ch::CachedResource& cachedResource = ch::AddCachedResource(ch::g_cachePath, name);
	static std::vector<std::string> file_dicts = { "client_script", "file", "shared_script" };
	static std::vector<std::string> resource_dicts = { "__resource.lua", "fxmanifest.lua" };

	for (const auto& str : file_dicts) {
		int meta_keys = CFX::GetNumResourceMetadata(name.c_str(), str.c_str());

		for (int meta_key = 0; meta_key < meta_keys; ++meta_key) {
			std::string path = CFX::GetResourceMetadata(name.c_str(), str.c_str(), meta_key);

			if (!path.empty() && path.find("@") == std::string::npos ) {//&& path.find("*") == std::string::npos

				Log::Msg(path.c_str());
				auto scrdata = CFX::LoadResourceFile(name.c_str(), path.c_str());
				if (scrdata != nullptr) {
					std::replace(path.begin(), path.end(), '/', '-');
					std::replace(path.begin(), path.end(), '*', '+');
					win32::File fileHandle = win32::File(ch::g_cachePath + name + "\\" + path);

					fileHandle.Write(scrdata);
					scrdata = "";

				}
				else
					Log::Msg("==empty %s", path.c_str());
			}

		}
	}
	//manifest
	for (const auto& str : resource_dicts) {
		auto scrdata = CFX::LoadResourceFile(name.c_str(), str.c_str());
		if (scrdata != nullptr) {
			//cachedResource.AddCachedScript2(name, scrdata, ch::g_cachePath);
			win32::File fileHandle = win32::File(ch::g_cachePath + name + "\\" + str);

			fileHandle.Write(scrdata);
			scrdata = "";
			//delete[]scrdata;
			//data.main[str] = true;//save manifest file content
		}
	}


}

void CacheAllResource2() {
	std::vector<fx::ResourceImpl*>* g_allResources = (std::vector<fx::ResourceImpl*>*)g_pattern->g_allResources;

	for (fx::ResourceImpl* resource : *g_allResources)
	{
		auto name = resource->m_name;
		auto state = resource->m_state;
		Log::Msg(name.c_str());
		Log::Msg(resource->m_rootPath.c_str());
		if (!name.empty() && state == ResourceState::Started) {
			if (win32::DirectoryExists(ch::g_cachePath))
			{
				win32::CreateNewDirectory(ch::g_cachePath + name, true);
				CacheResourceData(name);
			}
		}
	}
}

