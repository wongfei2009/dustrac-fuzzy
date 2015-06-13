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

#include <QString>

/**
* Loads all plugins (shared libs with extension .dpl) at the
* specified path. Once the shared lib is loaded, its init function
* with signature init(Game&, int argc, char ** argv)  is called.
*
* argc and argv represent command line arguments.
**/
void loadPlugins(QString path, int argc, char ** argv);

#endif // LOADPLUGINS
