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

#include <QApplication>
#include <QGLFormat>
#include <QDir>
#include <QLocale>
#include <QMessageBox>
#include <QTranslator>

#include <QCommandLineOption>
#include <QCommandLineParser>

#include "game.hpp"
#include "../common/config.hpp"
#include "loadplugins.hpp"

#include <MCException>
#include <MCLogger>

#include <iostream>
#include <vector>
#include <string>
#include <cstring>

static const char * INIT_ERROR = "Initing the game failed!";

static void initLogger()
{
    QString logPath = QDir::tempPath() + QDir::separator() + "dustrac.log";
    MCLogger::init(logPath.toStdString().c_str());
    MCLogger::setEchoMode(true);
    MCLogger::setDateTime(true);
    MCLogger().info() << "Dust Racing 2D version " << VERSION;
}

static void checkOpenGLVersion()
{
#ifdef __MC_GLES__
    if (QGLFormat::openGLVersionFlags() < QGLFormat::OpenGL_ES_Version_2_0)
    {
        QString versionError = QObject::tr("At least OpenGL ES 2.0 is required!");
        QMessageBox::critical(nullptr, QObject::tr("Cannot start Dust Racing 2D"), versionError);
        throw MCException(versionError.toStdString());
    }
#elif defined (__MC_GL30__)
    if (QGLFormat::openGLVersionFlags() < QGLFormat::OpenGL_Version_3_0)
    {
        QString versionError = QObject::tr("At least OpenGL 3.0 is required!");
        QMessageBox::critical(nullptr, QObject::tr("Cannot start Dust Racing 2D"), versionError);
        throw MCException(versionError.toStdString());
    }
#else
    if (QGLFormat::openGLVersionFlags() < QGLFormat::OpenGL_Version_2_1)
    {
        QString versionError = QObject::tr("At least OpenGL 2.1 is required!");
        QMessageBox::critical(nullptr, QObject::tr("Cannot start Dust Racing 2D"), versionError);
        throw MCException(versionError.toStdString());
    }
#endif
}

static void initTranslations(QTranslator & appTranslator, QApplication & app, QString lang = "")
{
    if (lang == "")
    {
        lang = QLocale::system().name();
    }

    if (appTranslator.load(QString(DATA_PATH) + "/translations/dustrac-game_" + lang))
    {
        app.installTranslator(&appTranslator);
    }
    else
    {
        MCLogger().info() << "Failed to load translations for " << QLocale::system().name().toStdString();
    }
}

int main(int argc, char ** argv)
{
    try
    {
		// split the argument list into two parts: before -- and after --
		int splitpos = -1;
		for(int i = 0; i < argc; i++) {
			if(!std::strcmp(argv[i], "--")) {
				splitpos = i;
				break;
			}
		}

		int argc1, argc2;
		char ** argv1, ** argv2;

		if(splitpos >= 0) {
			argc1 = splitpos;
			argv1 = argv;

			argc2 = argc - splitpos;
			argv2 = argv + splitpos;
		} else {
			argc1 = argc;
			argv1 = argv;
			argc2 = 0;
			argv2 = nullptr;
		}

        QApplication app(argc1, argv1);
        QCoreApplication::setApplicationName(Config::Game::GAME_NAME);
        QCoreApplication::setApplicationVersion(Config::Game::GAME_VERSION);

        // command line options parser
        QCommandLineParser parser;
		parser.setApplicationDescription(QCoreApplication::translate("main",
			"Plugin options may be entered, but must be separated using --.\n"
			"%1 %2\n"
			"Copyright (c) 2011-2015 Jussi Lind."
		).arg(Config::Game::GAME_NAME).arg(Config::Game::GAME_VERSION));
		parser.addHelpOption();
		parser.addVersionOption();

		// add the options
		QCommandLineOption vsyncOption(QStringList() << "n" << "no-vsync", QCoreApplication::translate("main", "Force vsync off."));
		parser.addOption(vsyncOption);

		QCommandLineOption langOption(QStringList() << "l" << "lang", QCoreApplication::translate("main", "Set the language."), "");
		parser.addOption(langOption);

		QCommandLineOption disableMenus(QStringList() << "d" << "disable-menus", QCoreApplication::translate("main", "Disable all menus and hop directly into the game."));
		parser.addOption(disableMenus);

		QCommandLineOption controllerType(QStringList() << "c" << "controller-type", QCoreApplication::translate("main", "Type of the controller to use (user|user1|user2|pid|from plugins...)."), "type", "type");
		parser.addOption(controllerType);

		QCommandLineOption gameMode(QStringList() << "m" << "game-mode", QCoreApplication::translate("main", "Sets the game mode (OnePlayerRace|TwoPlayerRace|TimeTrial|Duel)."), "mode", "OnePlayerRace");
		parser.addOption(gameMode);

		QCommandLineOption customTrackFile(QStringList() << "t" << "custom-track-file", QCoreApplication::translate("main", "Sets the custom track file (only used with menus disabled)."), "file", "infinity.trk");
		parser.addOption(customTrackFile);

		QCommandLineOption fullscreenOpt(QStringList() << "f" << "fullscreen", QCoreApplication::translate("main", "Starts the game in fullscreen mode."));
		parser.addOption(fullscreenOpt);

		QCommandLineOption windowedOpt(QStringList() << "w" << "windowed", QCoreApplication::translate("main", "Starts the game in windowed mode."));
		parser.addOption(windowedOpt);

		QCommandLineOption hresOpt(QStringList() << "hres", QCoreApplication::translate("main", "Sets horizontal resolution."), "resolution");
		parser.addOption(hresOpt);

		QCommandLineOption vresOpt(QStringList() << "vres", "vertical-resolution", QCoreApplication::translate("main", "Sets vertical resolution."), "resolution");
		parser.addOption(vresOpt);

		QCommandLineOption lapCount(QStringList() << "lap-count", QCoreApplication::translate("main", "Sets the number of laps."), "laps", "5");
		parser.addOption(lapCount);

		QCommandLineOption disableRendering(QStringList() << "disable-rendering", QCoreApplication::translate("main", "Disables rendering."));
		parser.addOption(disableRendering);

		// parse the options
		parser.process(app);

		Settings& settings = Settings::instance();
		settings.setMenusDisabled(parser.isSet(disableMenus));
		settings.setControllerType(parser.value(controllerType));

		settings.setGameMode(parser.value(gameMode));
		settings.setCustomTrackFile(parser.value(customTrackFile));
		settings.setLapCount(parser.value(lapCount).toInt());
		settings.setDisableRendering(parser.isSet(disableRendering));

		if(parser.isSet(fullscreenOpt) || parser.isSet(windowedOpt) || parser.isSet(hresOpt) || parser.isSet(vresOpt)) {
			int hRes, vRes;
			bool native, fullscreen;
			settings.loadResolution(hRes, vRes, native, fullscreen);

			if(parser.isSet(fullscreenOpt)) fullscreen = true;
			if(parser.isSet(windowedOpt)) fullscreen = false;
			if(parser.isSet(hresOpt)) hRes = parser.value(hresOpt).toUInt();
			if(parser.isSet(vresOpt)) vRes = parser.value(vresOpt).toUInt();

			settings.setTermResolution(hRes, vRes, fullscreen);
			settings.setUseTermResolution(true);
		}

        QTranslator appTranslator;
        initLogger();
        initTranslations(appTranslator, app, parser.value(langOption));
        checkOpenGLVersion();

        // Create the game object and set the renderer
        MCLogger().info() << "Creating game object.";
        Game game(parser.isSet(vsyncOption));

		// loads plugins
		loadPlugins(QString(Config::Game::pluginPath), argc2, argv2);

        // Initialize and start the game
        if (game.init())
        {
            game.start();
        }
        else
        {
            MCLogger().fatal() << INIT_ERROR;
            return EXIT_FAILURE;
        }

        return app.exec();
    }
    // Catch some errors during game initialization e.g.
    // a vertex shader not found.
    catch (MCException & e)
    {
        MCLogger().fatal() << e.what();
        MCLogger().fatal() << INIT_ERROR;
        return EXIT_FAILURE;
    }
}
