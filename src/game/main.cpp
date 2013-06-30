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

#include <QGuiApplication>
#include <QGLFormat>
#include <QDir>
#include <QHBoxLayout>
#include <QMessageBox>

#include "game.hpp"

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
    if (QGLFormat::openGLVersionFlags() < QGLFormat::OpenGL_Version_3_0)
    {
        QString versionError = QObject::tr("At least OpenGL 3.0 is required!");
        QMessageBox::critical(nullptr, QObject::tr("Cannot start Dust Racing 2D"), versionError);
        throw MCException(versionError.toStdString());
    }
}

static void printHelp()
{
    std::cout << "Dust Racing 2D version " << VERSION << std::endl;
    std::cout << "Copyright (c) 2011-2013 Jussi Lind." << std::endl << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "--show-cursor Show the mouse cursor in menus." << std::endl;
    std::cout << "--help        Show this help." << std::endl;
    std::cout << std::endl;
}

int main(int argc, char ** argv)
{
    try
    {
        QGuiApplication app(argc, argv);
        std::vector<std::string> args(argv, argv + argc);

        if (std::find(args.begin(), args.end(), "--help") != args.end())
        {
            printHelp();
            exit(EXIT_SUCCESS);
        }

        initLogger();
        checkOpenGLVersion();

        // Create the game object and set the renderer
        MCLogger().info() << "Creating game object..";
        Game game;
        game.setFps(Settings::instance().loadFps());
        game.setShowCursor(std::find(args.begin(), args.end(), "--show-cursor") != args.end());

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
