#include <game.hpp>
#include <string>
#include <functional>
#include <memory>

#include "../common/pathresolver.hpp"

#ifdef __unix__
	#include <dlfcn.h>
  typedef void* HandleType;
#elif defined(_WIN32) || defined(WIN32)
	#include <windows.h>
  typedef HINSTANCE HandleType;
#endif

struct PluginInfo {
  HandleType handle = nullptr;
  
	std::string name;
	std::string path;
	std::string filename;

	std::function<void(void*)> releaseHandle;

	~PluginInfo() {
		if(handle && releaseHandle) releaseHandle(handle);
	}
};

