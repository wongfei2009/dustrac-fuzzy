// This file is part of Dust Racing 2D.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef SHADERPROGRAM_HPP
#define SHADERPROGRAM_HPP

#include <MCGLShaderProgram>
#include <MCGLScene>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QObject>

/*! Implements the MCGLShaderProgram interface by wrapping
 *  a QGLShaderProgram. */
class ShaderProgram : public MCGLShaderProgram, protected QOpenGLFunctions
{
public:

    //! Constructor.
    ShaderProgram(QOpenGLContext * context, MCGLScene & scene);

    //! Destructor.
    virtual ~ShaderProgram();

    //! \reimp
    virtual void bind();

    //! \reimp
    virtual void release();

    //! \reimp
    virtual void link();

    //! \reimp
    virtual bool isLinked() const;

    //! \reimp
    virtual bool addVertexShaderFromFile(const std::string & path);

    //! \reimp
    virtual bool addFragmentShaderFromFile(const std::string & path);

    //! \reimp
    virtual bool addVertexShaderFromSource(const std::string & source);

    //! \reimp
    virtual bool addFragmentShaderFromSource(const std::string & source);

    //! \reimp
    virtual void setModelViewProjectionMatrix(
        const glm::mat4x4 & modelViewProjectionMatrix);

    //! \reimp
    virtual void rotate(GLfloat angle);

    //! \reimp
    virtual void translate(const MCVector3dF & pos);

    //! \reimp
    virtual void setColor(const MCGLColor & color);

    //! \reimp
    virtual void setScale(GLfloat x, GLfloat y, GLfloat z);

    //! \reimp
    virtual void setAmbientLight(const MCGLAmbientLight & light);

    //! \reimp
    virtual void setDiffuseLight(const MCGLDiffuseLight & light);

    //! \reimp
    virtual void setFadeValue(GLfloat f);

    //! \reimp
    virtual void bindTextureUnit0(GLuint index);

    //! \reimp
    virtual void bindTextureUnit1(GLuint index);

    //! \reimp
    virtual void bindTextureUnit2(GLuint index);

private:

    QOpenGLShaderProgram m_program;
    QOpenGLShader        m_fragmentShader;
    QOpenGLShader        m_vertexShader;
};

#endif // SHADERPROGRAM_HPP
