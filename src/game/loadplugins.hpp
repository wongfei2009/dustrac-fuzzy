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

#ifndef LOADPLUGINS
#define LOADPLUGINS

#include <map>
#include <string>
#include <QString>

#include "plugininterface.hpp"

extern std::map<std::string, std::shared_ptr<PluginInfo> > PluginRegister;

/**
* Loads all plugins (shared libs with extension .dpl) at the
* specified path and store their handles along with their PluginInfo in the
* plugin register.
**/
void loadPlugins(const QStringList& paths);

/**
* Splits an argument string into its standard argc, argv form.
**/
QStringList makeArgs(QString appName, QString argstr);

/**
* Initializes the plugin by calling init(Game&, const QStringList&).
* Argument args is the list of command line arguments.
**/
void initPlugin(const PluginInfo& pluginInfo, const QStringList& args);

#endif // LOADPLUGINS
