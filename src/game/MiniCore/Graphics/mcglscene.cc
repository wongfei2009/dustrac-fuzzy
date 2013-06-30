// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#include "mcglscene.hh"
#include "mcglshaderprogram.hh"
#include "mclogger.hh"
#include "mctrigonom.hh"

#include <algorithm>
#include <cmath>

MCGLScene::MCGLScene()
: m_splitType(Single)
, m_viewWidth(0)
, m_viewHeight(0)
, m_sceneWidth(0)
, m_sceneHeight(0)
, m_viewAngle(0)
, m_updateModelViewProjection(false)
{
}

void MCGLScene::addShaderProgram(MCGLShaderProgram & shader)
{
    if (std::find(m_shaders.begin(), m_shaders.end(), &shader) == m_shaders.end())
    {
        m_shaders.push_back(&shader);
    }
}

void MCGLScene::initialize()
{
    MCLogger().info() << "Initializing MCGLScene..";

    initializeOpenGLFunctions();

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);
}

void MCGLScene::resize(
    MCUint viewWidth, MCUint viewHeight, MCUint sceneWidth, MCUint sceneHeight, MCFloat viewAngle)
{
    m_viewWidth   = viewWidth;
    m_viewHeight  = viewHeight;
    m_sceneWidth  = sceneWidth;
    m_sceneHeight = sceneHeight;
    m_viewAngle   = viewAngle;

    updateViewport();
}

void MCGLScene::setViewerPosition(MCUint sceneWidth, MCUint sceneHeight, MCFloat viewAngle)
{
    // Set eye position so that the scene looks like a pure 2D-scene
    const MCUint  vH2  = sceneHeight / 2;
    const MCUint  vW2  = sceneWidth  / 2;
    const MCFloat eyeZ = vH2 /
        std::tan(static_cast<MCFloat>(MCTrigonom::degToRad(viewAngle / 2)));

    m_modelViewMatrix  = glm::mat4(1.0);
    m_modelViewMatrix *= glm::lookAt(
        glm::vec3(vW2, vH2, eyeZ),
        glm::vec3(vW2, vH2, 0),
        glm::vec3(0,   1,   0));

    m_updateModelViewProjection = true;
}

void MCGLScene::setSplitType(SplitType splitType)
{
    m_splitType = splitType;

    updateViewport();
}

void MCGLScene::setProjection(float aspectRatio, float zNear, float zFar)
{
    m_projectionMatrix  = glm::mat4(1.0);
    m_projectionMatrix *= glm::perspective(m_viewAngle, aspectRatio, zNear, zFar);

    m_updateModelViewProjection = true;
}

void MCGLScene::updateViewport()
{
    static const float zNear = 1.0;
    static const float zFar  = 1000.0;

    if (m_splitType == Single)
    {
        setProjection(static_cast<float>(m_sceneWidth) / m_sceneHeight, zNear, zFar);

        glViewport(0, 0, m_viewWidth, m_viewHeight);

        setViewerPosition(m_sceneWidth, m_sceneHeight, m_viewAngle);
    }
    else if (m_splitType == Left)
    {
        setProjection(static_cast<float>(m_sceneWidth / 2) / m_sceneHeight, zNear, zFar);

        glViewport(0, 0, m_viewWidth / 2, m_viewHeight);

        setViewerPosition(m_sceneWidth / 2, m_sceneHeight, m_viewAngle);
    }
    else if (m_splitType == Right)
    {
        setProjection(static_cast<float>(m_sceneWidth / 2) / m_sceneHeight, zNear, zFar);

        glViewport(m_viewWidth / 2, 0, m_viewWidth / 2, m_viewHeight);

        setViewerPosition(m_sceneWidth / 2, m_sceneHeight, m_viewAngle);
    }

    updateModelViewProjectionMatrixAndShaders();
}

const glm::mat4 & MCGLScene::modelViewProjectionMatrix() const
{
    if (m_updateModelViewProjection)
    {
        m_modelViewProjectionMatrix = m_projectionMatrix * m_modelViewMatrix;
        m_updateModelViewProjection = false;
    }

    return m_modelViewProjectionMatrix;
}

void MCGLScene::updateModelViewProjectionMatrixAndShaders()
{
    for (MCGLShaderProgram * p : m_shaders)
    {
        p->setModelViewProjectionMatrix(modelViewProjectionMatrix());
    }
}

MCGLScene::~MCGLScene()
{
}

