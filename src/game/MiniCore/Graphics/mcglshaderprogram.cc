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

#include "mcglshaderprogram.hh"
#include "mcglscene.hh"

MCGLShaderProgram * MCGLShaderProgram::m_activeProgram = nullptr;

MCGLShaderProgram::MCGLShaderProgram(MCGLScene & scene)
: m_scene(scene)
, m_isBound(false)
{
    m_scene.addShaderProgram(*this);
}

MCGLShaderProgram::~MCGLShaderProgram()
{
}

void MCGLShaderProgram::bind()
{
    m_isBound = true;

    if (MCGLShaderProgram::m_activeProgram && MCGLShaderProgram::m_activeProgram != this)
    {
        MCGLShaderProgram::m_activeProgram->release();
    }

    MCGLShaderProgram::m_activeProgram = this;
}

void MCGLShaderProgram::release()
{
    m_isBound = false;

    if (MCGLShaderProgram::m_activeProgram == this)
    {
        MCGLShaderProgram::m_activeProgram = nullptr;
    }
}

bool MCGLShaderProgram::isBound() const
{
    return m_isBound;
}

void MCGLShaderProgram::link()
{
}

bool MCGLShaderProgram::isLinked() const
{
    return false;
}

bool MCGLShaderProgram::addVertexShaderFromFile(const std::string &)
{
    return false;
}

bool MCGLShaderProgram::addFragmentShaderFromFile(const std::string &)
{
    return false;
}

bool MCGLShaderProgram::addGeometryShaderFromFile(const std::string &)
{
    return false;
}

bool MCGLShaderProgram::addVertexShaderFromSource(const std::string &)
{
    return false;
}

bool MCGLShaderProgram::addFragmentShaderFromSource(const std::string &)
{
    return false;
}

bool MCGLShaderProgram::addGeometryShaderFromSource(const std::string &)
{
    return false;
}

void MCGLShaderProgram::setModelViewProjectionMatrix(const glm::mat4x4 &)
{
}

void MCGLShaderProgram::rotate(GLfloat)
{
}

void MCGLShaderProgram::translate(const MCVector3dF &)
{
}

void MCGLShaderProgram::setColor(const MCGLColor &)
{
}

void MCGLShaderProgram::setScale(GLfloat, GLfloat, GLfloat)
{
}

void MCGLShaderProgram::setFadeValue(GLfloat)
{
}

void MCGLShaderProgram::bindTextureUnit0(GLuint)
{
}

void MCGLShaderProgram::bindTextureUnit1(GLuint)
{
}

void MCGLShaderProgram::bindTextureUnit2(GLuint)
{
}

void MCGLShaderProgram::setAmbientLight(const MCGLAmbientLight &)
{
}

void MCGLShaderProgram::setDiffuseLight(const MCGLDiffuseLight &)
{
}

