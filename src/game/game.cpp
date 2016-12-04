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

#include "../common/config.hpp"

#include "game.hpp"

#include "audioworker.hpp"
#include "graphicsfactory.hpp"
#include "eventhandler.hpp"
#include "inputhandler.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "statemachine.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "trackloader.hpp"
#include "trackselectionmenu.hpp"

#include <MCAssetManager>
#include <MCCamera>
#include <MCLogger>
#include <MCObjectFactory>
#include <MCWorldRenderer>

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QTime>
#include <QScreen>
#include <QSurfaceFormat>

#include <cassert>

static const unsigned int MAX_PLAYERS = 2;

Game * Game::m_instance = nullptr;

Game::Game(bool forceNoVSync, bool forceNoSounds)
: m_settings(Settings::instance())
, m_difficultyProfile(m_settings.loadDifficulty())
, m_inputHandler(new InputHandler(MAX_PLAYERS))
, m_eventHandler(new EventHandler(*m_inputHandler))
, m_stateMachine(new StateMachine(*m_inputHandler))
, m_renderer(nullptr)
, m_scene(nullptr)
, m_assetManager(new MCAssetManager(
    Config::Common::dataPath.toStdString(),
    (Config::Common::dataPath + QDir::separator().toLatin1() + "surfaces.conf").toStdString(),
    "",
    (Config::Common::dataPath + QDir::separator().toLatin1() + "meshes.conf").toStdString()))
, m_objectFactory(new MCObjectFactory(*m_assetManager))
, m_trackLoader(new TrackLoader(*m_objectFactory))
, m_updateFps(60)
, m_updateDelay(1000 / m_updateFps)
, m_timeStep(1.0 / m_updateFps)
, m_lapCount(m_settings.getLapCount())
, m_paused(false)
, m_renderElapsed(0)
, m_mode(Mode::OnePlayerRace)
, m_splitType(SplitType::Vertical)
, m_audioWorker(new AudioWorker(
      Scene::NUM_CARS, forceNoSounds ? false : Settings::instance().loadValue(Settings::soundsKey(), true)))
{
    assert(!Game::m_instance);
    Game::m_instance = this;
	createRenderer(forceNoVSync);

    const QString& mode = Settings::instance().getGameMode();
    if(mode == "OnePlayerRace") setMode(Mode::OnePlayerRace);
    else if(mode == "TwoPlayerRace") setMode(Mode::TwoPlayerRace);
    else if(mode == "TimeTrial") setMode(Mode::TimeTrial);
    else if(mode == "Duel") setMode(Mode::Duel);
    else {
    	MCLogger().warning() << "Unknown game mode '" << mode.toStdString() << "'.";
    }

	connect(&m_difficultyProfile, &DifficultyProfile::difficultyChanged, [this] () {
        m_trackLoader->updateLockedTracks(m_lapCount, m_difficultyProfile.difficulty());
    });

	if(!Settings::instance().getDisableRendering()) {
		connect(m_eventHandler, &EventHandler::pauseToggled, this, &Game::togglePause);

		connect(m_eventHandler, &EventHandler::cursorRevealed, [this] () {
		    m_renderer->setCursor(Qt::ArrowCursor);
		});

		connect(m_eventHandler, &EventHandler::cursorHid, [this] () {
		    m_renderer->setCursor(Qt::BlankCursor);
		});
	}

	connect(m_eventHandler, &EventHandler::gameExited, this, &Game::exitGame);

	connect(m_eventHandler, SIGNAL(soundRequested(QString)), m_audioWorker, SLOT(playSound(QString)));

    connect(&m_updateTimer, &QTimer::timeout, [this] () {
        m_stateMachine->update();
        m_scene->updateFrame(m_timeStep);
        m_scene->updateAnimations();
        m_scene->updateOverlays();
        m_renderer->renderNow();
    });

    m_updateTimer.setInterval(m_updateDelay);

    connect(m_stateMachine, &StateMachine::exitGameRequested, this, &Game::exitGame);

    // Add race track search paths
    m_trackLoader->addTrackSearchPath(QString(Config::Common::dataPath) +
        QDir::separator() + "levels");
    m_trackLoader->addTrackSearchPath(QDir::homePath() + QDir::separator() +
        Config::Common::TRACK_SEARCH_PATH);

    m_elapsed.start();
}

Game & Game::instance()
{
    assert(Game::m_instance);
    return *Game::m_instance;
}

void Game::createRenderer(bool forceNoVSync)
{
    // Create the main window / renderer
    int hRes, vRes;
    bool fullScreen = false;

	if(!Settings::instance().getDisableRendering()) {
    	m_settings.getResolution(hRes, vRes, fullScreen);

		if (!hRes || !vRes)
		{
			hRes = QGuiApplication::primaryScreen()->geometry().width();
			vRes = QGuiApplication::primaryScreen()->geometry().height();
		}

		adjustSceneSize(hRes, vRes);

		MCLogger().info()
	        << "Resolution: " << hRes << " " << vRes << " " << fullScreen;

	} else {
		hRes = 100;
		vRes = 100;
	}

    MCLogger().info() << "Creating the renderer..";

    QSurfaceFormat format;

#ifdef __MC_GL30__
    format.setVersion(3, 0);
    format.setProfile(QSurfaceFormat::CoreProfile);
#elif defined(__MC_GLES__)
    format.setVersion(1, 0);
#else
    format.setVersion(2, 1);
#endif
    format.setSamples(0);

// Supported only in Qt 5.3+
#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
    if (forceNoVSync)
    {
        format.setSwapInterval(0);
    }
    else
    {
        format.setSwapInterval(Settings::instance().loadVSync());
    }
#else
    Q_UNUSED(forceNoVSync);
#endif

    m_renderer = new Renderer(hRes, vRes, fullScreen, m_world.renderer().glScene());
    m_renderer->setFormat(format);

	if(!Settings::instance().getDisableRendering()) {

		if (fullScreen)
		{
		    m_renderer->showFullScreen();
		}
		else
		{
		    m_renderer->show();
		}

		m_renderer->setEventHandler(*m_eventHandler);
		connect(m_renderer, &Renderer::initialized, this, &Game::init);
	    connect(m_stateMachine, &StateMachine::renderingEnabled, m_renderer, &Renderer::setEnabled);
	} else {
		m_renderer->show();
		//m_renderer->makeCurrent();
		m_renderer->setEventHandler(*m_eventHandler);
		connect(m_renderer, &Renderer::initialized, this, &Game::init);
		connect(m_stateMachine, &StateMachine::renderingEnabled, m_renderer, &Renderer::setEnabled);
		//m_renderer->setVisible(false);
	}
}

void Game::adjustSceneSize(int hRes, int vRes)
{
    // Adjust scene height so that view aspect ratio is taken into account.
    const int newSceneHeight = Scene::width() * vRes / hRes;
    Scene::setSize(Scene::width(), newSceneHeight);
}

void Game::setMode(Game::Mode mode)
{
    m_mode = mode;
}

Game::Mode Game::mode() const
{
    return m_mode;
}

void Game::setSplitType(Game::SplitType splitType)
{
    m_splitType = splitType;
}

Game::SplitType Game::splitType() const
{
    return m_splitType;
}

void Game::setLapCount(int lapCount)
{
    m_lapCount = lapCount;
    m_trackLoader->updateLockedTracks(lapCount, m_difficultyProfile.difficulty());
}

int Game::lapCount() const
{
    return m_lapCount;
}

bool Game::hasTwoHumanPlayers() const
{
    return m_mode == Mode::TwoPlayerRace || m_mode == Mode::Duel;
}

bool Game::hasComputerPlayers() const
{
    return m_mode == Mode::TwoPlayerRace || m_mode == Mode::OnePlayerRace;
}

EventHandler & Game::eventHandler()
{
    assert(m_eventHandler);
    return *m_eventHandler;
}

AudioWorker & Game::audioWorker()
{
    assert(m_audioWorker);
    return *m_audioWorker;
}

DifficultyProfile & Game::difficultyProfile()
{
    return m_difficultyProfile;
}

const std::string & Game::fontName() const
{
    static const std::string fontName("generated");
    return fontName;
}

Renderer & Game::renderer() const
{
    assert(m_renderer);
    return *m_renderer;
}

bool Game::loadTracks()
{
    // Load track data
    if (int numLoaded = m_trackLoader->loadTracks(m_lapCount, m_difficultyProfile.difficulty()))
    {
        MCLogger().info() << "A total of " << numLoaded << " race track(s) loaded.";
    }
    else
    {
        throw std::runtime_error("No valid race tracks found.");
    }

    return true;
}

void Game::initScene()
{
    assert(m_stateMachine);

    // Create the scene
    m_scene = new Scene(*this, *m_stateMachine, *m_renderer, m_world);

    // Add tracks to the menu.
    for (unsigned int i = 0; i < m_trackLoader->tracks(); i++)
    {
        m_scene->trackSelectionMenu().addTrack(*m_trackLoader->track(i));
    }

    // Set the current game scene. Renderer calls render()
    // for all objects in the scene.
    if(m_renderer) m_renderer->setScene(*m_scene);
}

void Game::init()
{
    Settings& settings = Settings::instance();

	if(!settings.getDisableRendering()) {
		m_audioThread.start();
		m_audioWorker->moveToThread(&m_audioThread);
		QMetaObject::invokeMethod(m_audioWorker, "init");
		QMetaObject::invokeMethod(m_audioWorker, "loadSounds");
	}

    m_assetManager->load();

    if(settings.getMenusDisabled()) {
    	initScene();

    	TrackData* t_data = m_trackLoader->loadTrack(settings.getCustomTrackFile(), false);

    	if(!t_data) {
    		throw std::runtime_error("Couldn't load tracks.");
    	}

    	_customTrack = std::shared_ptr<Track>(new Track(t_data));
		m_scene->setActiveTrack(*_customTrack);

		m_stateMachine->startGame();
		m_scene->startRace();
		InputHandler::setEnabled(true);

	} else {

		if (loadTracks())
		{
		    initScene();
		}
		else
		{
		    throw std::runtime_error("Couldn't load tracks.");
		}

	}

    start();
}

void Game::start()
{
    m_paused = false;
    m_updateTimer.start();
}

void Game::stop()
{
    m_paused = true;
    m_updateTimer.stop();
}

void Game::togglePause()
{
    if (m_paused)
    {
        start();
        MCLogger().info() << "Game continued.";
    }
    else
    {
        stop();
        MCLogger().info() << "Game paused.";
    }
}

void Game::exitGame()
{
    stop();
    m_renderer->close();

    m_audioThread.quit();
    m_audioThread.wait();

    QApplication::quit();
}

Game::~Game()
{
    delete m_renderer;
    delete m_stateMachine;
    delete m_scene;
    delete m_assetManager;
    delete m_trackLoader;
    delete m_objectFactory;
    delete m_eventHandler;
    delete m_inputHandler;
    delete m_audioWorker;
}
