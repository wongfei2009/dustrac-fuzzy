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

#include <QDir>
#include <QFile>
#include <QStringList>
#include <MCLogger>

#ifdef __unix__
	#include <dlfcn.h>
#elif defined(_WIN32) || defined(WIN32)
	#include <windows.h>
#endif

void loadPlugins(QString path, int argc, char ** argv) {
	QStringList pluginPaths(QDir(path).entryList(QStringList("*.dpl")));

	for (QString pluginPath : pluginPaths)
    {
        pluginPath = path + QDir::separator() + pluginPath;

        #ifdef __unix__
            void* handle = dlopen(pluginPath.toStdString().c_str(), RTLD_LAZY | RTLD_GLOBAL);

            if (handle) {
                void (*init)(Game&, int, char**);
                *(void **)(&init) = dlsym(handle, "init");
                if(init) (*init)(Game::instance(), argc, argv);
                else MCLogger().error() << "Couldn't initialize plugin '" << pluginPath.toStdString() << "'.";
            } else {
                MCLogger().error() << "Couldn't load plugin '" << pluginPath.toStdString() << "'.";
            }
        #elif defined(_WIN32) || defined(WIN32)
            HINSTANCE handle = LoadLibrary(pluginPath.toStdString().c_str());

            if (handle) {
                typedef void (__stdcall *init_t)(Game&, int, char**);
                init_t init = (init_t) GetProcAddress(handle, "init");
                if(init) (*init)(Game::instance(), argc, argv);
                else MCLogger().error() << "Couldn't initialize plugin '" << pluginPath.toStdString() << "'.";
            } else {
                MCLogger().error() << "Couldn't load plugin '" << pluginPath.toStdString() << "'.";
            }
        #endif
    }
}

