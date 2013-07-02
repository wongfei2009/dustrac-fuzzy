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
#include "shaderprogram.hpp"

#ifdef __ANDROID__
#include "shadersGLES.h"
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
    create();

    assert(!Renderer::m_instance);
    Renderer::m_instance = this;
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

void Renderer::createProgramFromFile(
    const std::string & handle, const std::string & fshPath, const std::string & vshPath)
{
    // Note: ShaderProgram throws on error.

    MCGLShaderProgram * program = new ShaderProgram(m_context, *m_glScene);
    program->addFragmentShaderFromFile(
        std::string(Config::Common::dataPath) + "/shaders/" + fshPath);
    program->addVertexShaderFromFile(
        std::string(Config::Common::dataPath) + "/shaders/" + vshPath);
    program->link();
    m_shaderHash[handle].reset(program);
}

void Renderer::createProgramFromSource(
    const std::string & handle, const std::string & fshSource, const std::string & vshSource)
{
    // Note: ShaderProgram throws on error.

    MCGLShaderProgram * program = new ShaderProgram(m_context, *m_glScene);
    program->addFragmentShaderFromSource(fshSource);
    program->addVertexShaderFromSource(vshSource);
    program->link();
    m_shaderHash[handle].reset(program);
}

void Renderer::loadShaders()
{
    createProgramFromSource("car",           carFshDesktop,              carVshDesktop);
    createProgramFromSource("fbo",           fboFshDesktop,              fboVshDesktop);
    createProgramFromSource("master",        masterFshDesktop,           masterVshDesktop);
    createProgramFromSource("masterShadow",  masterShadowFshDesktop,     masterShadowVshDesktop);
    createProgramFromSource("menu",          menuFshDesktop,             menuVshDesktop);
    createProgramFromSource("particle",      particleFshDesktop,         masterVshDesktop);
    createProgramFromSource("pointParticle", pointParticleFshDesktop,    pointParticleVshDesktop);
    createProgramFromSource("pointParticleDiscard", pointParticleDiscardFshDesktop, pointParticleVshDesktop);
    createProgramFromSource("text",          textFshDesktop,             textVshDesktop);
    createProgramFromSource("textShadow",    textShadowFshDesktop,       textVshDesktop);
    createProgramFromSource("tile2d",        tile2dFshDesktop,           tileVshDesktop);
    createProgramFromSource("tile3d",        tile3dFshDesktop,           tileVshDesktop);

    // Make sure that shaders have the current model view projection matrix.
    m_glScene->updateModelViewProjectionMatrixAndShaders();
}

void Renderer::setEnabled(bool enable)
{
    m_enabled = enable;
}

MCGLShaderProgram & Renderer::program(const std::string & id)
{
    MCGLShaderProgramPtr program = m_shaderHash[id];
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
    if (m_enabled)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (m_scene)
        {
            m_scene->render();
        }
    }
}

void Renderer::renderCustomResolution()
{
    // Render the game scene to the frame buffer object
    if (m_enabled)
    {
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
