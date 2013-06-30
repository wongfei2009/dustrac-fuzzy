// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2010 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCGLRECTPARTICLE_HH
#define MCGLRECTPARTICLE_HH

#include "mcparticle.hh"
#include "mcglcolor.hh"

#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

class MCCamera;
class MCGLShaderProgram;

/*! \class MCGLRectParticle
 *  \brief A particle that renders as a simple OpenGL rectangle. Final class. */
class MCGLRectParticle : public MCParticle, protected QOpenGLFunctions
{
public:

    //! Constructor.
    MCGLRectParticle(const std::string & typeID);

    //! Destructor.
    virtual ~MCGLRectParticle();

    //! Set color
    void setColor(const MCGLColor & color);

    //! Set the shader program to be used.
    void setShaderProgram(MCGLShaderProgram * program);

    //! \reimp
    void render(MCCamera * camera = nullptr);

    //! \reimp
    void renderShadow(MCCamera * camera = nullptr);

    //! \reimp
    void beginBatch();

    //! \reimp
    void endBatch();

private:

    DISABLE_COPY(MCGLRectParticle);
    DISABLE_ASSI(MCGLRectParticle);

    MCGLColor                       m_color;
    static GLuint                   m_vbo;
    static QOpenGLVertexArrayObject m_vao;
    MCGLShaderProgram             * m_program;
};

#endif // MCGLRECTPARTICLE_HH
