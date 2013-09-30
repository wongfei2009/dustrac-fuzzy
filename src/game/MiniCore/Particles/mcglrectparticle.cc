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

#include "mcglrectparticle.hh"
#include "mcglshaderprogram.hh"
#include "mcglvertex.hh"
#include "mccamera.hh"
#include "mcexception.hh"

#include <cassert>

GLuint MCGLRectParticle::m_vbo = 0;
QOpenGLVertexArrayObject MCGLRectParticle::m_vao;

static const int NUM_VERTICES     = 6;
static const int VERTEX_DATA_SIZE = sizeof(MCGLVertex) * NUM_VERTICES;
static const int NORMAL_DATA_SIZE = sizeof(MCGLVertex) * NUM_VERTICES;
static const int COLOR_DATA_SIZE  = sizeof(MCGLColor)  * NUM_VERTICES;
static const int TOTAL_DATA_SIZE  = VERTEX_DATA_SIZE + NORMAL_DATA_SIZE + COLOR_DATA_SIZE;

MCGLRectParticle::MCGLRectParticle(const std::string & typeID)
: MCParticle(typeID)
, m_color(1.0, 1.0, 1.0, 1.0)
, m_program(nullptr)
{
    initializeOpenGLFunctions();

    if (m_vbo == 0)
    {
        // Init vertice data for a quad
        const MCGLVertex vertices[NUM_VERTICES] =
        {
            {-1, -1, 0},
            { 1,  1, 0},
            {-1,  1, 0},
            {-1, -1, 0},
            { 1, -1, 0},
            { 1,  1, 0}
        };

        const MCGLVertex normals[NUM_VERTICES] =
        {
            { 0, 0, 1},
            { 0, 0, 1},
            { 0, 0, 1},
            { 0, 0, 1},
            { 0, 0, 1},
            { 0, 0, 1}
        };

        const MCGLColor colors[NUM_VERTICES] =
        {
            m_color,
            m_color,
            m_color,
            m_color,
            m_color,
            m_color
        };

        int offset = 0;

        glGenBuffers(1, &m_vbo);

        // Wrapped inside QOpenGLVertexArrayObject:
        // glGenVertexArrays(1, &m_vao);
        // glBindVertexArray(m_vao);

//        m_vao.create();
//        if (!m_vao.isCreated())
//        {
//            MCException VAOFailed("Cannot create a VAO!");
//            throw VAOFailed;
//        }

//        m_vao.bind();

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, TOTAL_DATA_SIZE, nullptr, GL_STATIC_DRAW);

        // Vertex data
        glBufferSubData(GL_ARRAY_BUFFER, offset, VERTEX_DATA_SIZE, vertices);
        offset += VERTEX_DATA_SIZE;

        // Normal data
        glBufferSubData(GL_ARRAY_BUFFER, offset, NORMAL_DATA_SIZE, normals);
        offset += NORMAL_DATA_SIZE;

        // Vertex color data
        glBufferSubData(GL_ARRAY_BUFFER, offset, COLOR_DATA_SIZE, colors);

        glVertexAttribPointer(MCGLShaderProgram::VAL_Vertex, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glVertexAttribPointer(MCGLShaderProgram::VAL_Normal, 3, GL_FLOAT, GL_FALSE, 0,
            reinterpret_cast<GLvoid *>(VERTEX_DATA_SIZE));
        glVertexAttribPointer(MCGLShaderProgram::VAL_Color,  4, GL_FLOAT, GL_FALSE, 0,
            reinterpret_cast<GLvoid *>(VERTEX_DATA_SIZE + NORMAL_DATA_SIZE));

        glEnableVertexAttribArray(MCGLShaderProgram::VAL_Vertex);
        glEnableVertexAttribArray(MCGLShaderProgram::VAL_Normal);
        glEnableVertexAttribArray(MCGLShaderProgram::VAL_Color);

        //m_vao.release();
    }
}

MCGLRectParticle::~MCGLRectParticle()
{
    if (m_vbo != 0)
    {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
}

void MCGLRectParticle::setShaderProgram(MCGLShaderProgram * program)
{
    m_program = program;
}

void MCGLRectParticle::setColor(const MCGLColor & color)
{
    m_color = color;
}

void MCGLRectParticle::beginBatch()
{
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_program->bind();
    m_vao.bind();
}

void MCGLRectParticle::endBatch()
{
    m_vao.release();
    glDisable(GL_BLEND);
}

void MCGLRectParticle::render(MCCamera * camera)
{
    // Scale radius if fading out
    MCFloat r = radius();
    if (animationStyle() == Shrink)
    {
        r *= scale();
    }

    if (r > 0 && m_program)
    {
        MCFloat x = location().i();
        MCFloat y = location().j();

        if (camera)
        {
            camera->mapToCamera(x, y);
        }

        // Scale alpha if fading out.
        GLfloat alpha = m_color.a();
        if (animationStyle() == FadeOut)
        {
            alpha *= scale();
        }
        m_color.setA(alpha);

        m_program->translate(MCVector3dF(x, y, location().k()));
        m_program->rotate(angle());
        m_program->setColor(m_color);
        m_program->setScale(r, r, 1.0);

        glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES);
    }
}

void MCGLRectParticle::renderShadow(MCCamera *)
{
    return;
}
