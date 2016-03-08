// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

//------------------------------------------------------------------------------
//-----auxiliary macros used to export symbols to shared library interface------
//------------------------------------------------------------------------------

#include "export.hpp"

//! Config data for editor and for the game.
class DUSTCOMMON_API Config
{
public:

    //! Common config data.
    class DUSTCOMMON_API Common
    {
    public:

        //! The base data path.
        static const char * dataPath;

        //! "Company" name used in QSettings.
        static const char * QSETTINGS_COMPANY_NAME;

        //! Path used to search "3rd party" race tracks under the home dir:
        //! ~/TRACK_SEARCH_PATH/
        static const char * TRACK_SEARCH_PATH;
    };

    //! Editor-specific config data.
    class DUSTCOMMON_API Editor
    {
    public:

        static const char * EDITOR_NAME;
        static const char * EDITOR_VERSION;
        static const char * MODEL_CONFIG_FILE_NAME;
        static const char * QSETTINGS_SOFTWARE_NAME;

        //! Path to the "select" icon.
        static const char * SELECT_PATH;

        //! Path to the "erase" icon.
        static const char * ERASE_PATH;

        //! Path to the "clear" tile.
        static const char * CLEAR_PATH;
    };

    //! Game-specific config data.
    class DUSTCOMMON_API Game
    {
    public:

        static const char * GAME_NAME;
        static const char * GAME_VERSION;
        static const char * QSETTINGS_SOFTWARE_NAME;

        //! Default window width.
        static const unsigned int WINDOW_WIDTH;

        //! Default window height.
        static const unsigned int WINDOW_HEIGHT;

		//! The path to plugins.
		static const char * pluginPath;
    };
};
