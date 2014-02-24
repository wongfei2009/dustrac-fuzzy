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

#ifndef SCENE_HPP
#define SCENE_HPP

#include "ai.hpp"
#include "car.hpp"
#include "race.hpp"
#include "timingoverlay.hpp"

#include <QObject>
#include <MCCamera>
#include <memory>
#include <vector>

class CheckeredFlag;
class FadeAnimation;
class Game;
class InputHandler;
class Intro;
class MCCamera;
class MCObject;
class MCSurface;
class MCWorld;
class MessageOverlay;
class ParticleFactory;
class Renderer;
class Startlights;
class StartlightsOverlay;
class StateMachine;
class Track;
class TrackSelectionMenu;
class TreeView;

namespace MTFH {
class Menu;
class MenuManager;
}

//! The game scene.
class Scene : public QObject
{
    Q_OBJECT

public:

    static const int NUM_CARS = 12;

    //! Constructor.
    Scene(Game & game, StateMachine & stateMachine, Renderer & renderer);

    //! Destructor.
    ~Scene();

    //! Width of the scene. This is always constant and doesn't
    //! depend on resolution.
    static int width();

    //! Height of the scene. This is always constant and doesn't
    //! depend on resolution.
    static int height();

    //! Set scene size.
    static void setSize(int width, int height);

    //! Update physics and objects by the given time step.
    void updateFrame(InputHandler & handler, float timeStep);

    //! Update/trigger animations.
    void updateAnimations();

    //! Update HUD overlays.
    void updateOverlays();

    //! Set the active race track.
    void setActiveTrack(Track & activeTrack);

    //! Return the active race track.
    Track & activeTrack() const;

    //! Return the world.
    MCWorld & world() const;

    //! Return track selection menu.
    TrackSelectionMenu & trackSelectionMenu() const;

    //! Render all components.
    void render();

signals:

    void listenerLocationChanged(float x, float y);

private:

    void addCarsToWorld();
    void addTrackObjectsToWorld();
    void createCars();
    void createMenus();
    void initRace();
    void processUserInput(InputHandler & handler);
    void renderCommonScene();
    void renderPlayerScene(MCCamera & camera);
    void resizeOverlays();
    void setupAudio(Car & car, int index);
    void setupAI(Track & activeTrack);
    void setupCameras(Track & activeTrack);
    void setWorldDimensions();
    void updateAI();
    void updateCameraLocation(MCCamera & camera, MCFloat & offset, MCObject & object);
    void updateRace();
    void updateWorld(float timeStep);

    static int            m_width;
    static int            m_height;
    Game                & m_game;
    StateMachine        & m_stateMachine;
    Renderer            & m_renderer;
    MessageOverlay      * m_messageOverlay;
    Race                  m_race;
    Track               * m_activeTrack;
    MCWorld             * m_world;
    TimingOverlay         m_timingOverlay[2];
    Startlights         * m_startlights;
    StartlightsOverlay  * m_startlightsOverlay;
    CheckeredFlag       * m_checkeredFlag;
    MCCamera              m_camera[2];
    MCFloat               m_cameraOffset[2];
    TrackSelectionMenu  * m_trackSelectionMenu;
    MTFH::Menu          * m_mainMenu;
    MTFH::Menu          * m_help;
    MTFH::Menu          * m_credits;
    MTFH::Menu          * m_settingsMenu;
    MTFH::MenuManager   * m_menuManager;
    Intro               * m_intro;
    ParticleFactory     * m_particleFactory;
    FadeAnimation       * m_fadeAnimation;

    typedef std::vector<CarPtr> CarVector;
    CarVector m_cars;

    typedef std::vector<AIPtr> AIVector;
    AIVector m_ai;

    // TreeViews need to be separately updated.
    std::vector<TreeView *> m_treeViews;

    // Bridges
    std::vector<MCObjectPtr> m_bridges;
};

#endif // SCENE_HPP
