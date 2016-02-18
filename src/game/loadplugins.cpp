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

std::vector<std::pair<void*, PluginInfo*> > PluginRegister;

QStringList makeArgs(QString appName, QString argstr) {
	QStringList args = QStringList(appName) << argstr.split(" ", QString::SkipEmptyParts);
	return args;
}

void loadPlugins(QString path) {
	QStringList pluginPaths(QDir(path).entryList(QStringList("*.dpl")));

	for (QString pluginPath : pluginPaths)
    {
        pluginPath = path + QDir::separator() + pluginPath;

        #ifdef __unix__
            void* handle = dlopen(pluginPath.toStdString().c_str(), RTLD_LAZY | RTLD_GLOBAL);
			PluginInfo* info = nullptr;

            if (handle) {
				PluginInfo* (*pluginInfo)();
                *(void **)(&pluginInfo) = dlsym(handle, "pluginInfo");
                if(pluginInfo) {
					info = (*pluginInfo)();
					if(!info) MCLogger().warning() << "Plugin '" << pluginPath.toStdString() <<
						"' returns null plugin info. It will be skipped.";
					else PluginRegister.push_back(std::pair<void*, PluginInfo*>(handle, info));
                } else MCLogger().error() << "Couldn't get plugin's info '" << pluginPath.toStdString() << "'.";
            } else {
                MCLogger().error() << "Couldn't load plugin '" << pluginPath.toStdString() << "'.";
            }
        #elif defined(_WIN32) || defined(WIN32)
            HINSTANCE handle = LoadLibrary(pluginPath.toStdString().c_str());

            if (handle) {
                typedef PluginInfo* (__stdcall *pluginInfo_t)();
                pluginInfo_t pluginInfo = (pluginInfo_t) GetProcAddress(handle, "pluginInfo");
                if(pluginInfo) {
					info = (*pluginInfo)(Game::instance(), argc, argv);
					if(!info) MCLogger().warning() << "Plugin '" << pluginPath.toStdString() <<
						"' returns null plugin info. It will be skipped.";
					else PluginRegister.push_back(std::pair<void*, PluginInfo*>(handle, info));
                } else MCLogger().error() << "Couldn't initialize plugin '" << pluginPath.toStdString() << 
            } else {
                MCLogger().error() << "Couldn't load plugin '" << pluginPath.toStdString() << "'.";
            }
        #endif
    }
}

void initPlugin(void* handle, const PluginInfo& pluginInfo, const QStringList& args) {
	#ifdef __unix__
        if (handle) {
            void (*init)(Game&, const QStringList& args);
            *(void **)(&init) = dlsym(handle, "init");
            if(init) (*init)(Game::instance(), args);
            else MCLogger().error() << "Couldn't initialize plugin '" << pluginInfo.name << "'.";
        } else {
            MCLogger().error() << "Couldn't load plugin '" << pluginInfo.name << "'.";
        }
    #elif defined(_WIN32) || defined(WIN32)
        if (handle) {
            typedef void (__stdcall *init_t)(Game&, const QStringList& args);
            init_t init = (init_t) GetProcAddress(handle, "init");
            if(init) (*init)(Game::instance(), args);
            else MCLogger().error() << "Couldn't initialize plugin '" << pluginInfo.name << "'.";
        } else {
            MCLogger().error() << "Couldn't load plugin '" << pluginInfo.name << "'.";
        }
    #endif
}

