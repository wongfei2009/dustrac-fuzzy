// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCGLOBJECTBASE_HH
#define MCGLOBJECTBASE_HH

#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

class MCGLShaderProgram;

/*! Base class for GL objects in MiniCore. Automatically creates VBO, VAO and
 *  basic texturing support. */
class MCGLObjectBase : protected QOpenGLFunctions
{
protected:

    //! Constructor.
    MCGLObjectBase();

    //! Destructor.
    virtual ~MCGLObjectBase();

    //! Bind the VAO. VAO will be created when constructed.
    void bindVAO();

    //! Release the VAO.
    void releaseVAO();

public:

    //! Bind the current texture setup.
    void bindTextures(bool bindOnlyFirstTexture = false);

    //! Set texture handle #1 or zero.
    void setTexture1(GLuint handle);

    //! Set texture handle #2 or zero.
    void setTexture2(GLuint handle);

    //! Set texture handle #2 or zero.
    void setTexture3(GLuint handle);

    //! Return texture handle #1 or zero.
    GLuint texture1() const;

    //! Return texture handle #2 or zero.
    GLuint texture2() const;

    //! Return texture handle #3 or zero.
    GLuint texture3() const;

    //! Set the shader program to be used.
    void setShaderProgram(MCGLShaderProgram * program);

    //! Set the shader program to be used for 2d shadows.
    void setShadowShaderProgram(MCGLShaderProgram * program);

    //! Get the shader program to be used.
    MCGLShaderProgram * shaderProgram() const;

    //! Get the shader program to be used for 2d shadows.
    MCGLShaderProgram * shadowShaderProgram() const;

private:

    void createVAO();

    QOpenGLVertexArrayObject m_vao;
    GLuint                   m_texture1;
    GLuint                   m_texture2;
    GLuint                   m_texture3;
    MCGLShaderProgram      * m_program;
    MCGLShaderProgram      * m_shadowProgram;
};

#endif // MCGLOBJECTBASE_HH
