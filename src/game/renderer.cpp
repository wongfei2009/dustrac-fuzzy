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

#include "renderer.hpp"

#include "eventhandler.hpp"
#include "scene.hpp"

#ifdef __MC_GLES__
#include "shadersGLES.h"
#elseif defined(__MC_GL30__)
#include "shaders30.h"
#else
#include "shaders.h"
#endif

#include "../common/config.hpp"

#include <MCGLScene>
#include <MCGLShaderProgram>
#include <MCException>
#include <MCLogger>
#include <MCSurface>
#include <MCSurfaceManager>
#include <MCTrigonom>
#include <MCWorld>

#include <cmath>
#include <cassert>

#include <QIcon>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QOpenGLPaintDevice>
#include <QScreen>

Renderer * Renderer::m_instance = nullptr;

Renderer::Renderer(
    const QSurfaceFormat & format,
    int hRes,
    int vRes,
    bool nativeResolution,
    bool fullScreen,
    QWindow * parent)
: QWindow(parent)
, m_context(nullptr)
, m_scene(nullptr)
, m_glScene(new MCGLScene)
, m_eventHandler(nullptr)
, m_viewAngle(45.0)
, m_fadeValue(1.0)
, m_enabled(false)
, m_hRes(hRes)
, m_vRes(vRes)
, m_nativeResolution(nativeResolution)
, m_fullScreen(fullScreen)
{
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(format);

    assert(!Renderer::m_instance);
    Renderer::m_instance = this;

    setTitle(QString(Config::Game::GAME_NAME) + " " + Config::Game::GAME_VERSION);
    setIcon(QIcon(":/dustrac-game.png"));
}

Renderer & Renderer::instance()
{
    assert(Renderer::m_instance);
    return *Renderer::m_instance;
}

void Renderer::initialize()
{
    if (!m_fullScreen)
    {
        // Set window size & disable resize
        resize(m_hRes, m_vRes);
        setMinimumSize(QSize(m_hRes, m_vRes));
        setMaximumSize(QSize(m_hRes, m_vRes));

        // Try to center the window
        const int fullVRes = QGuiApplication::primaryScreen()->geometry().height();
        const int fullHRes = QGuiApplication::primaryScreen()->geometry().width();
        setPosition(fullHRes / 2 - m_hRes / 2, fullVRes / 2 - m_vRes / 2);
    }

    m_context = new QOpenGLContext(this);
    m_context->setFormat(requestedFormat());
    m_context->create();
    m_context->makeCurrent(this);

    if (!m_context->isValid())
    {
        std::stringstream ss;
        ss << "Cannot create context for OpenGL version " <<
              requestedFormat().majorVersion() << "." << requestedFormat().minorVersion();
        throw MCException(ss.str());
    }

    // This has to be called after we have a current context!
    initializeOpenGLFunctions();

    MCLogger().info() << "OpenGL Version: " << glGetString(GL_VERSION);

    m_glScene->initialize();

    resizeGL(m_hRes, m_vRes);
    loadShaders();
}

void Renderer::resizeGL(int viewWidth, int viewHeight)
{
    m_glScene->resize(
        viewWidth, viewHeight, Scene::width(), Scene::height(),
        m_viewAngle);
}

void Renderer::createProgramFromSource(
    const std::string & handle, const std::string & vshSource, const std::string & fshSource)
{
    // Note: ShaderProgram throws on error.

    MCGLShaderProgram * program = new MCGLShaderProgram(*m_glScene);
    program->addFragmentShaderFromSource(fshSource);
    program->addVertexShaderFromSource(vshSource);
    program->link();
    m_shaderHash[handle].reset(program);
}

void Renderer::loadShaders()
{
    // Engine defaults
    createProgramFromSource("master",
        MCGLShaderProgram::getDefaultVertexShaderSource(),
        MCGLShaderProgram::getDefaultFragmentShaderSource());
    createProgramFromSource("masterShadow",
        MCGLShaderProgram::getDefaultShadowVertexShaderSource(),
        MCGLShaderProgram::getDefaultShadowFragmentShaderSource());

    // Custom shaders
    createProgramFromSource("car",           carVsh,              carFsh);
    createProgramFromSource("fbo",           fboVsh,              fboFsh);
    createProgramFromSource("menu",          menuVsh,             MCGLShaderProgram::getDefaultFragmentShaderSource());
    createProgramFromSource("particle",      MCGLShaderProgram::getDefaultVertexShaderSource(), particleFsh);
    createProgramFromSource("pointParticle", pointParticleVsh,    pointParticleFsh);
    createProgramFromSource("pointParticleRotate", pointParticleVsh, pointParticleRotateFsh);
    createProgramFromSource("text",          textVsh,             MCGLShaderProgram::getDefaultFragmentShaderSource());
    createProgramFromSource("textShadow",    textVsh,             textShadowFsh);
    createProgramFromSource("tile2d",        tileVsh,             MCGLShaderProgram::getDefaultFragmentShaderSource());
    createProgramFromSource("tile3d",        tileVsh,             tile3dFsh);

    // Make sure that shaders have the current view projection matrix.
    m_glScene->updateViewProjectionMatrixAndShaders();
}

void Renderer::setEnabled(bool enable)
{
    m_enabled = enable;
}

MCGLShaderProgram & Renderer::program(const std::string & id)
{
    MCGLShaderProgramPtr program(m_shaderHash[id]);
    if (!program.get())
    {
        throw MCException("Cannot find shader program '" + id +"'");
    }
    return *program;
}

MCGLScene & Renderer::glScene()
{
    return *m_glScene;
}

void Renderer::setFadeValue(float value)
{
    m_fadeValue = value;
}

float Renderer::fadeValue() const
{
    return m_fadeValue;
}

void Renderer::renderNativeResolutionOrWindowed()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_scene)
    {
        m_scene->render();
    }
}

void Renderer::renderCustomResolution()
{
    // Render the game scene to the frame buffer object
    resizeGL(m_hRes, m_vRes);

    static QOpenGLFramebufferObject fbo(m_hRes, m_vRes);

    fbo.bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_scene)
    {
        m_scene->render();
    }

    fbo.release();

    // Render the frame buffer object onto the screen

    const int fullVRes = QGuiApplication::primaryScreen()->geometry().height();
    const int fullHRes = QGuiApplication::primaryScreen()->geometry().width();

    resizeGL(fullHRes, fullVRes);

    MCSurface sd(fbo.texture(), 0, 0, Scene::width(), Scene::height());
    sd.setShaderProgram(&program("fbo"));
    sd.bindTextures();
    sd.render(nullptr, MCVector3dF(Scene::width() / 2, Scene::height() / 2, 0), 0);
}

void Renderer::render()
{
    if (m_enabled)
    {
        if (!m_fullScreen || m_nativeResolution)
        {
            renderNativeResolutionOrWindowed();
        }
        else
        {
            renderCustomResolution();
        }

        m_context->swapBuffers(this);
    }
}

void Renderer::keyPressEvent(QKeyEvent * event)
{
    assert(m_eventHandler);
    if (!m_eventHandler->handleKeyPressEvent(event))
    {
        QWindow::keyPressEvent(event);
    }
}

void Renderer::keyReleaseEvent(QKeyEvent * event)
{
    assert(m_eventHandler);
    if (!m_eventHandler->handleKeyReleaseEvent(event))
    {
        QWindow::keyReleaseEvent(event);
    }
}

void Renderer::mousePressEvent(QMouseEvent * event)
{
    assert(m_eventHandler);
    m_eventHandler->handleMousePressEvent(event, width(), height(), true);
}

void Renderer::mouseReleaseEvent(QMouseEvent * event)
{
    assert(m_eventHandler);
    m_eventHandler->handleMouseReleaseEvent(event, width(), height(), true);
}

void Renderer::mouseMoveEvent(QMouseEvent * event)
{
    assert(m_eventHandler);
    m_eventHandler->handleMouseMoveEvent(event);
}

void Renderer::closeEvent(QCloseEvent * event)
{
    event->accept();
    emit closed();
}

void Renderer::setScene(Scene & scene)
{
    m_scene = &scene;
}

void Renderer::setEventHandler(EventHandler & eventHandler)
{
    m_eventHandler = &eventHandler;
}

Renderer::~Renderer()
{
    delete m_glScene;
}
