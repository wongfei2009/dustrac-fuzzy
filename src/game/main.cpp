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

#include <MCException>
#include <MCLogger>

#include <iostream>
#include <vector>
#include <string>

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
        QApplication app(argc, argv);
        QCoreApplication::setApplicationName(Config::Game::GAME_NAME);
        QCoreApplication::setApplicationVersion(Config::Game::GAME_VERSION);

        // command line options parser
        QCommandLineParser parser;
		parser.setApplicationDescription(QCoreApplication::translate("main",
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

		QCommandLineOption controllerType(QStringList() << "c" << "controller-type", QCoreApplication::translate("main", "Type of the controller to use (user|fuzzy|pid)."), "type", "user");
		parser.addOption(controllerType);

		QCommandLineOption controllerPath(QStringList() << "p" << "controller-path", QCoreApplication::translate("main", "Path to the controller file (if any)."), "file");
		parser.addOption(controllerPath);

		QCommandLineOption gameMode(QStringList() << "m" << "game-mode", QCoreApplication::translate("main", "Sets the game mode (OnePlayerRace|TwoPlayerRace|TimeTrial|Duel)."), "mode", "OnePlayerRace");
		parser.addOption(gameMode);

		QCommandLineOption customTrackFile(QStringList() << "t" << "custom-track-file", QCoreApplication::translate("main", "Sets the custom track file (only used with menus disabled)."), "file", "infinity.trk");
		parser.addOption(customTrackFile);

		// parse the options
		parser.process(app);

		Settings& settings = Settings::instance();
		settings.setMenusDisabled(parser.isSet(disableMenus));
		settings.setControllerType(parser.value(controllerType));
		settings.setControllerPath(parser.value(controllerPath));
		settings.setGameMode(parser.value(gameMode));
		settings.setCustomTrackFile(parser.value(customTrackFile));

        QTranslator appTranslator;
        initLogger();
        initTranslations(appTranslator, app, parser.value(langOption));
        checkOpenGLVersion();

        // Create the game object and set the renderer
        MCLogger().info() << "Creating game object.";
        Game game(parser.isSet(vsyncOption));

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
