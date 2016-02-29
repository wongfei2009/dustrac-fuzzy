#include <game.hpp>
#include <string>
#include <functional>
#include <memory>

#include "../common/pathresolver.hpp"

struct PluginInfo {
	void* handle = nullptr;
	std::string name;
	std::string path;
	std::string filename;

	std::function<void(void*)> releaseHandle;

	~PluginInfo() {
		if(handle && releaseHandle) releaseHandle(handle);
	}
};

