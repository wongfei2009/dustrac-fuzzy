// This file is part of Dust Racing 2D.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#include "loadplugins.hpp"
#include "game.hpp"

#include <iostream>

#include <QDir>
#include <QFile>
#include <QStringList>
#include <MCLogger>

#ifdef __unix__
	#include <dlfcn.h>
#elif defined(_WIN32) || defined(WIN32)
	#include <windows.h>
#endif

std::map<std::string, std::shared_ptr<PluginInfo> > PluginRegister;

QStringList makeArgs(QString appName, QString argstr) {
	QStringList args = QStringList(appName) << argstr.split(" ", QString::SkipEmptyParts);
	return args;
}

void loadPlugins(const QStringList& paths) {
	for(auto& path: paths) { // for each path in paths
		QDir pathDir(path);
		pathDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

		for(auto& pluginDir: pathDir.entryList()) { // for each subdirectory in the path
			QString pluginDirPath = path +  QDir::separator() + pluginDir;
			QStringList pluginPaths(QDir(pluginDirPath).entryList(QStringList("*.dpl")));

			for (QString pluginName : pluginPaths) // for each plugin in the subdirectory
			{
				QString pluginFilename = pluginDirPath + QDir::separator() + pluginName;
				
				#ifdef __unix__
				    void* handle = dlopen(pluginFilename.toStdString().c_str(), RTLD_LAZY | RTLD_GLOBAL);
					std::shared_ptr<PluginInfo> info;

				    if (handle) {
						std::shared_ptr<PluginInfo> (*pluginInfo)();
				        *(void **)(&pluginInfo) = dlsym(handle, "pluginInfo");
				        if(pluginInfo) {
							info = (*pluginInfo)();
							if(!info) MCLogger().warning() << "Plugin '" << pluginFilename.toStdString() <<
								"' returns null plugin info. It will be skipped.";
							else {
								info->handle = handle;
//								info->releaseHandle = [](void* handle) {dlclose(handle);};
								info->path = pluginDirPath.toStdString();
								info->filename = pluginFilename.toStdString();

								// doing the registration this way makes sure we replace
								// anything registered under the same name 
								PluginRegister[info->name] = info;
							}
				        } else MCLogger().error() << "Couldn't get plugin's info '" << pluginFilename.toStdString() << "'.";
				    } else {
				        MCLogger().error() << "Couldn't load plugin '" << pluginFilename.toStdString() << "'.";
				    }
				#elif defined(_WIN32) || defined(WIN32)
				    HINSTANCE handle = LoadLibrary(pluginFilename.toStdString().c_str());

				    if (handle) {
				        typedef PluginInfo* (__stdcall *pluginInfo_t)();
				        pluginInfo_t pluginInfo = (pluginInfo_t) GetProcAddress(handle, "pluginInfo");
				        if(pluginInfo) {
							info = (*pluginInfo)(Game::instance(), argc, argv);
							if(!info) MCLogger().warning() << "Plugin '" << pluginFilename.toStdString() <<
								"' returns null plugin info. It will be skipped.";
							else {
								info->handle = handle;
//								info->releaseHandle = [](void* handle) {FreeLibrary(handle);};
								info->path = pluginDirPath.toStdString();
								info->filename = pluginFilename.toStdString();

								// doing the registration this way makes sure we replace
								// anything registered under the same name 
								PluginRegister[info->name] = info;
							}
				        } else MCLogger().error() << "Couldn't initialize plugin '" << pluginFilename.toStdString() << 
				    } else {
				        MCLogger().error() << "Couldn't load plugin '" << pluginFilename.toStdString() << "'.";
				    }
				#endif
			}
		}
	}
}

void initPlugin(const PluginInfo& pluginInfo, const QStringList& args) {
	auto handle = pluginInfo.handle;

	#ifdef __unix__
        if (handle) {
            void (*init)(Game&, const PluginInfo& pluginInfo, const QStringList& args);
            *(void **)(&init) = dlsym(handle, "init");
            if(init) (*init)(Game::instance(), pluginInfo, args);
            else MCLogger().error() << "Couldn't initialize plugin '" << pluginInfo.name << "'.";
        } else {
            MCLogger().error() << "Couldn't load plugin '" << pluginInfo.name << "'.";
        }
    #elif defined(_WIN32) || defined(WIN32)
        if (handle) {
            typedef void (__stdcall *init_t)(Game&, const PluginInfo& pluginInfo, const QStringList& args);
            init_t init = (init_t) GetProcAddress(handle, "init");
            if(init) (*init)(Game::instance(), pluginInfo, args);
            else MCLogger().error() << "Couldn't initialize plugin '" << pluginInfo.name << "'.";
        } else {
            MCLogger().error() << "Couldn't load plugin '" << pluginInfo.name << "'.";
        }
    #endif
}

