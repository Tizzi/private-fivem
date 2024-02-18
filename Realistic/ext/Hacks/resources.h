#pragma once
#include <Includes.h>
#include <FileManager.h>
#include <invoker.hpp>

#include <functional>
#include <memory>
#include <atomic>
#include <type_traits>

#include <msgpack.hpp>
#include <msgpack_util.h>
#include <rapidjson/rapidjson.h>
//#include <nlohmann/json.hpp>
//using json = nlohmann::json;

namespace fx
{
	template<typename... Args>
	class fwEvent
	{
	public:
		using TFunc = std::function<bool(Args...)>;

	public:
		struct callback
		{
			TFunc function;
			std::unique_ptr<callback> next = nullptr;
			int order = 0;
			size_t cookie = -1;

			callback(TFunc func)
				: function(func)
			{
			}
		};

		std::unique_ptr<callback> m_callbacks;
		std::atomic<size_t> m_connectCookie = 0;
	};


	class Resource
	{
	public:
		char pad_040[0x40];
		fx::fwEvent<std::vector<char>*> OnBeforeLoadScript;
		fx::fwEvent<> OnStart;
		fx::fwEvent<> OnStop;
		fx::fwEvent<> OnEnter;
		fx::fwEvent<> OnLeave;
		fx::fwEvent<> OnCreate;
		fx::fwEvent<> OnActivate;
		fx::fwEvent<> OnDeactivate;
		fx::fwEvent<> OnRemove;
	};


	class ResourceImpl : public Resource
	{
	public:
		std::string m_name;
		std::string m_rootPath;
		uintptr_t* m_manager;
		ResourceState m_state;
	};

	class NetLibrary
	{
	public:
		char pad[0x0f8];
		std::string m_currentServerUrl;

	};


	template<typename... Args>
	inline size_t ConnectInternal(fx::fwEvent<Args...>& event, typename fx::fwEvent<Args...>::TFunc func, int order)
	{
		if (!func)
			return -1;

		auto cookie = event.m_connectCookie++;
		auto cb = std::unique_ptr<typename fx::fwEvent<Args...>::callback>(new typename fx::fwEvent<Args...>::callback(func));
		cb->order = order;
		cb->cookie = cookie;

		if (!event.m_callbacks)
		{
			event.m_callbacks = std::move(cb);
		}
		else
		{
			auto cur = &event.m_callbacks;
			typename fx::fwEvent<Args...>::callback* last = nullptr;

			while (*cur && order >= (*cur)->order)
			{
				last = cur->get();
				cur = &(*cur)->next;
			}

			cb->next = std::move(*cur);
			(!last ? event.m_callbacks : last->next) = std::move(cb);
		}

		return cookie;
	}


	template<typename... Args, typename T>
	inline auto Connect(fx::fwEvent<Args...>& event, T func)
	{
		return Connect(event, func, 99999);
	}

	template<typename... Args, typename T>
	inline auto Connect(fx::fwEvent<Args...>& event, T func, int order)
	{
		if constexpr (std::is_same_v<std::invoke_result_t<T, Args...>, bool>)
		{
			return ConnectInternal(event, func, order);
		}
		else
		{
			return ConnectInternal(event, [func](Args&&... args)
				{
					std::invoke(func, args...);
					return true;
				},
				order);
		}
	}

}

namespace ch
{
	inline std::string g_cachePath = _xor_("C:\\RealisticT\\Saves\\");

	class CachedScript
	{
	public:
		void SetIndex(int index) { m_index = index; }
		void SetData(const std::string& data) { m_data = data; }

	public:
		int GetIndex() { return m_index; }
		std::string GetData() { return m_data; }

	private:
		int m_index;
		std::string m_data;
	};

	class CachedResource
	{
	public:
		bool AddCachedScript(int index, const std::string& data, const std::string& directoryPath)
		{
			auto it = std::find_if(m_cachedScripts.begin(), m_cachedScripts.end(),
				[=](CachedScript& cs) { return cs.GetIndex() == index || (cs.GetData().find(data) != std::string::npos); });

			if (it != m_cachedScripts.end()) { return false; }

			CachedScript cachedScript;

			cachedScript.SetIndex(index);
			cachedScript.SetData(data);

			m_cachedScripts.push_back(cachedScript);

			win32::File fileHandle = win32::File(directoryPath + this->GetName() +
				"\\scrDump_" + std::to_string(index) + ".lua");

			fileHandle.Write(data);

			return true;
		}
		bool AddCachedScript2(const std::string& name, const std::string& data, const std::string& directoryPath)
		{
			win32::File fileHandle = win32::File(directoryPath + this->GetName() + name);

			fileHandle.Write(data);

			return true;
		}
	public:
		void SetName(const std::string& name) { m_name = name; }
	public:
		std::string GetName() { return m_name; }
		std::vector<CachedScript> GetCachedScripts() { return m_cachedScripts; }

	private:
		std::string m_name;
		std::vector<CachedScript> m_cachedScripts;
	};

	inline std::vector<CachedResource> g_cachedResources;

	inline CachedResource& AddCachedResource(const std::string& path, const std::string& name)
	{
		auto it = std::find_if(g_cachedResources.begin(), g_cachedResources.end(),
			[&name](CachedResource& cr) { return cr.GetName() == name; });

		// if we already have it
		if (it != g_cachedResources.end()) { return *it; }

		CachedResource cachedResource;
		cachedResource.SetName(name);

		if (win32::DirectoryExists(path))
		{
			win32::CreateNewDirectory(path + name, true);
		}

		g_cachedResources.push_back(cachedResource);

		return g_cachedResources.back();
	}
}
bool InitBase();


void CacheResourceData(const std::string& name);
inline void CacheAllResource() {

	for (int i = 1; i <= CFX::GetNumResources() - 1; ++i) {
		std::string name = CFX::GetResourceByFindIndex(i);
		Log::Msg(name.c_str());
		if (!name.empty() && CFX::GetResourceState(name.c_str()) == "started") {
			if (win32::DirectoryExists(ch::g_cachePath))
			{
				win32::CreateNewDirectory(ch::g_cachePath + name, true);
				CacheResourceData(name);
			}
			//resource_list[name] = true;
			//resource_data.emplace(name, GetResourceData(name));
		}
	}

}
void CacheAllResource2();

inline void appendToOutput(std::ostringstream& oss) { }

template <typename T, typename... Args>
inline void appendToOutput(std::ostringstream& oss, T first, Args... args) {

	if constexpr (std::is_same_v<T, const char*> || std::is_same_v<T, char*>)//if a string
	{
		if (std::strlen(first) > 0)//not empty
		{
			std::istringstream iss(first);
			double value = -1;

			if (iss >> value)//if a number
				oss << first;
			else if (std::strcmp(first, "false") == 0)//boolean
				oss << 0;
			else if (std::strcmp(first, "true") == 0)
				oss << 1;
			else//String
				oss << "\"" << first << "\"";
		}
		else
			oss << "\"\"";//input empty string
	}
	else {//number
		oss << first;
	}

	if (sizeof...(args) > 0) {
		oss << ", ";
	}
	appendToOutput(oss, args...);  // Recursive call
}

template <typename... Args>
void TriggerEvent(bool isServer, const char* EventName, Args... args)
{
	std::ostringstream oss;
	appendToOutput(oss, args...);
	std::string jsons = "[" + oss.str() + "]";
	//Log::Msg(jsons.c_str());
	rapidjson::Document document;
	document.Parse(jsons.c_str());//jsonArray.dump(2).c_str()
	msgpack::sbuffer* sbuffer = new msgpack::sbuffer;;

	msgpack::packer<msgpack::sbuffer> packer(*sbuffer);
	keyvi::util::JsonToMsgPack(document, &packer, 1);
	if (p_variables->misc.TESTER)
	{
		if (isServer)
			CFX::TriggerServerEventInternal(EventName, sbuffer->data(), sbuffer->size());
		else
			CFX::TriggerEventInternal(EventName, sbuffer->data(), sbuffer->size());
	}
	
	oss.str("");
	delete sbuffer;
}