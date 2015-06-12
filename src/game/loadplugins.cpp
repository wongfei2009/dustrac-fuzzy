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

#include <dlfcn.h>

void loadPlugins(QString path) {
	QStringList pluginPaths(QDir(path).entryList(QStringList("*.dpl")));
	
	for (QString pluginPath : pluginPaths)
    {
        pluginPath = path + QDir::separator() + pluginPath;
		void* handle = dlopen(pluginPath.toStdString().c_str(), RTLD_LAZY);

		if (handle) {
			void (*init)(Game&);
			*(void **)(&init) = dlsym(handle, "init");
			if(init) (*init)(Game::instance());
			else MCLogger().error() << "Couldn't initialize plugin '" << pluginPath.toStdString() << "'.";
		} else {
			MCLogger().error() << "Couldn't load plugin '" << pluginPath.toStdString() << "'.";
		}

    }
}

