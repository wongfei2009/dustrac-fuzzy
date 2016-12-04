// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef COMMON_CONFIG_H
#define COMMON_CONFIG_H

#include <QString>

//! Config variables for editor and for the game.
namespace Config
{
    class Common
    {
    public:
        //! The base data path given by -DDATA_PATH.
        static const QString dataPath;

        static const QString COPYRIGHT;

        //! "Company" name used in QSettings.
        static const QString QSETTINGS_COMPANY_NAME;

        /*! Path used to search "3rd party" race tracks under the home dir:
         *  ~/TRACK_SEARCH_PATH/ */
        static const QString TRACK_SEARCH_PATH;

        static const QString WEB_SITE_URL;
    };

    class Editor
    {
    public:
        static const QString EDITOR_NAME;

        static const QString EDITOR_VERSION;

        static const QString MODEL_CONFIG_FILE_NAME;

        static const QString QSETTINGS_SOFTWARE_NAME;

        static const QString SELECT_ICON_PATH;

        static const QString ERASE_ICON_PATH;

        static const QString CLEAR_ICON_PATH;
    };

    class Game
    {
    public:
        static const QString GAME_NAME;

        static const QString GAME_VERSION;

 	static const QString QSETTINGS_SOFTWARE_NAME;

	//! Default window width.
        static const unsigned int WINDOW_WIDTH;

        //! Default window height.
        static const unsigned int WINDOW_HEIGHT;

	//! The path to plugins.
	static const char * pluginPath;
    };

} // Config

#endif // COMMON_CONFIG_H

