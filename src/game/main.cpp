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

#include <QDir>
#include <QLocale>
#include <QMessageBox>
#include <QSettings>
#include <QTranslator>

#include <QCommandLineOption>
#include <QCommandLineParser>

#include "../common/config.hpp"
#include "application.hpp"
#include "game.hpp"
#include "../common/config.hpp"
#include "loadplugins.hpp"

#include <MCLogger>

#include <iostream>
#include <vector>
#include <string>
#include <cstring>

static void initLogger()
{
    QString logPath = QDir::tempPath() + QDir::separator() + "dustrac.log";
    MCLogger::init(logPath.toStdString().c_str());
    MCLogger::setEchoMode(true);
    MCLogger::setDateTime(true);
    MCLogger().info() << "Dust Racing 2D version " << VERSION;
    MCLogger().info() << "Compiled against Qt version " << QT_VERSION_STR;
}

static void initTranslations(QTranslator & appTranslator, QGuiApplication & app, QString lang = "")
{
    if (lang == "")
    {
        lang = QLocale::system().name();
    }

    if (appTranslator.load(QString(DATA_PATH) + "/translations/dustrac-game_" + lang))
    {
        app.installTranslator(&appTranslator);
        MCLogger().info() << "Loaded translations for " << lang.toStdString();
    }
    else
    {
        MCLogger().warning() << "Failed to load translations for " << lang.toStdString();
    }
}

int main(int argc, char ** argv)
{
    QApplication::setOrganizationName(Config::Common::QSETTINGS_COMPANY_NAME);
    QApplication::setApplicationName(Config::Game::QSETTINGS_SOFTWARE_NAME);
#ifdef Q_OS_WIN32
    QSettings::setDefaultFormat(QSettings::IniFormat);
#endif

    Application app(argc, argv);
    QTranslator appTranslator;
    QString lang = "";

	initLogger();

	// command line options parser
    QCommandLineParser parser;
	parser.setApplicationDescription(QCoreApplication::translate("main",
		"Plugin options are entered inside --PluginName \"-options\".\n"
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

	QCommandLineOption controllerType(QStringList() << "c" << "controller-type", QCoreApplication::translate("main", "Type of the controller to use (user|user1|user2|pid|from plugins...)."), "type", "user");
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

	QCommandLineOption disableSounds(QStringList() << "disable-sounds", QCoreApplication::translate("main", "Disables sounds."));
	parser.addOption(disableSounds);

	QCommandLineOption stuckPlayerCheck(QStringList() << "s" << "stuck-player", QCoreApplication::translate("main", "Enables checking whether players' cars are stuck."));
	parser.addOption(stuckPlayerCheck);

	QCommandLineOption cameraSmoothing(QStringList() << "camera-smoothing", QCoreApplication::translate("main", "Sets camera smoothing."), "(0, 1]", "0.05");
	parser.addOption(cameraSmoothing);

	MCLogger().info() << "Checking for plugins in path: '" << Config::Game::pluginPath << "'.";

	// load plugins
	loadPlugins(QString(Config::Game::pluginPath).split(PATH_SEP, QString::KeepEmptyParts));
	
	// for every plugin, add a CLI argument
	for(auto& plugin: PluginRegister) {
		MCLogger().info() << "Loaded plugin " << plugin.second->name << ".";

		QCommandLineOption pluginOpt(QStringList() << plugin.second->name.c_str(), QCoreApplication::translate("main", 					"Arguments for a plugin."), "\"arguments\"");
		parser.addOption(pluginOpt);
	}

	// parse the options
	parser.process(app);

	Settings& settings = Settings::instance();
	settings.setMenusDisabled(parser.isSet(disableMenus));
	settings.setControllerType(parser.value(controllerType));

	settings.setGameMode(parser.value(gameMode));
	settings.setCustomTrackFile(parser.value(customTrackFile));
	settings.setLapCount(parser.value(lapCount).toInt());
	settings.setDisableRendering(parser.isSet(disableRendering));
	settings.setResetStuckPlayer(parser.isSet(stuckPlayerCheck));
	settings.setCameraSmoothing(parser.value(cameraSmoothing).toFloat());

	if(parser.isSet(fullscreenOpt) || parser.isSet(windowedOpt) || parser.isSet(hresOpt) || parser.isSet(vresOpt)) {
		int hRes, vRes;
		bool fullscreen;
		settings.loadResolution(hRes, vRes, fullscreen);

		if(parser.isSet(fullscreenOpt)) fullscreen = true;
		if(parser.isSet(windowedOpt)) fullscreen = false;
		if(parser.isSet(hresOpt)) hRes = parser.value(hresOpt).toUInt();
		if(parser.isSet(vresOpt)) vRes = parser.value(vresOpt).toUInt();

		settings.setTermResolution(hRes, vRes, fullscreen);
		settings.setUseTermResolution(true);
	}

    initTranslations(appTranslator, app, parser.value(langOption));

    // Create the main game object. The game loop starts immediately after
    // the Renderer has been initialized.
    MCLogger().info() << "Creating game object..";
    Game game(parser.isSet(vsyncOption), parser.isSet(disableSounds));

	MCLogger().info() << "Initializing loaded plugins...'";
	// initialize all plugins
	for(auto& plugin: PluginRegister) {
		QStringList args = makeArgs(argv[0], parser.value(QString(plugin.second->name.c_str())));
		initPlugin(*plugin.second, args);
	}

    return app.exec();
}
