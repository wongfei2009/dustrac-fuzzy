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

#ifndef GAME_HPP
#define GAME_HPP

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QTime>

#include <cassert>
#include <memory>
#include <MCWorld>

#include "config.hpp"
#include "settings.hpp"

class AudioWorker;
class EventHandler;
class InputHandler;
class Renderer;
class Scene;
class Speedometer;
class Startlights;
class StartlightsOverlay;
class StateMachine;
class TimingOverlay;
class TrackLoader;
class MCObjectFactory;
class MCAssetManager;

//! The main game class.
class DUST_API Game : public QObject
{
    Q_OBJECT

public:

    enum class Mode
    {
        OnePlayerRace,
        TwoPlayerRace,
        TimeTrial,
        Duel
    };

    enum class SplitType
    {
        Horizontal,
        Vertical
    };

    //! Constructor
    Game(bool forceNoVSync = false, bool forceNoSounds = false);

    //! Destructor
    virtual ~Game();

    //! Return the game instance.
    static Game & instance();

    //! \return The renderer.
    Renderer & renderer() const;

    //! Start the game.
    void start();

    //! Stop scene.
    void stop();

    //! Set the game mode.
    void setMode(Mode mode);

    //! Get the game mode.
    Mode mode() const;

    //! Set the split type on two-player game.
    void setSplitType(SplitType splitType);

    //! Get the split type.
    SplitType splitType() const;

    //! Set the lap count.
    void setLapCount(int lapCount);

    //! Get the lap count.
    int lapCount() const;

    //! \return True if the current mode has two human players.
    bool hasTwoHumanPlayers() const;

    //! \return True if the current mode has computer players.
    bool hasComputerPlayers() const;

    EventHandler & eventHandler();

    AudioWorker & audioWorker();

    DifficultyProfile & difficultyProfile();

    const std::string & fontName() const;

    InputHandler& inputHandler() {
    	assert(m_inputHandler);
    	return *m_inputHandler;
    }

    const InputHandler& inputHandler() const {
    	assert(m_inputHandler);
		return *m_inputHandler;
	}

public slots:

    void exitGame();

private slots:

    void init();

    void togglePause();

private:

    void adjustSceneSize(int hRes, int vRes);

    void createRenderer(bool forceNoVSync);

    void initScene();

    bool loadTracks();

    Settings& m_settings;

    DifficultyProfile m_difficultyProfile;

    InputHandler * m_inputHandler;

    EventHandler * m_eventHandler;

    StateMachine * m_stateMachine;

    Renderer * m_renderer;

    Scene * m_scene;

    MCAssetManager * m_assetManager;

    MCObjectFactory * m_objectFactory;

    TrackLoader * m_trackLoader;

    int m_updateFps;

    int m_updateDelay;

    float m_timeStep;

    int m_lapCount;

    bool m_paused;

    QTimer m_updateTimer;

    QTime m_elapsed;

    int m_renderElapsed;

    Mode m_mode;

    SplitType m_splitType;

    AudioWorker * m_audioWorker;

    QThread m_audioThread;

    MCWorld m_world;

    std::shared_ptr<Track> _customTrack;

    static Game * m_instance;
};

#endif // GAME_HPP
