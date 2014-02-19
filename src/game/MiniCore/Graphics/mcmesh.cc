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

#include "mcmesh.hh"

#include "mccamera.hh"
#include "mcbbox.hh"
#include "mcglshaderprogram.hh"
#include "mcglvertex.hh"
#include "mcgltexcoord.hh"
#include "mctrigonom.hh"
#include "mcassetmanager.hh"

#include <algorithm>

static const int NUM_VERTEX_COMPONENTS    = 3;
static const int NUM_COLOR_COMPONENTS     = 4;
static const int NUM_TEX_COORD_COMPONENTS = 2;

MCMesh::MCMesh(const FaceVector & faces, MCGLMaterialPtr material)
: m_w(1.0)
, m_h(1.0)
, m_color(1.0, 1.0, 1.0, 1.0)
, m_sx(1.0)
, m_sy(1.0)
, m_sz(1.0)
{
    init(faces);

    setMaterial(material);
}

void MCMesh::init(const FaceVector & faces)
{
    const int NUM_FACES = faces.size();
    m_numVertices = NUM_FACES * 3; // Only triagles accepted

    MCGLVertex   * vertices  = new MCGLVertex[m_numVertices];
    MCGLVertex   * normals   = new MCGLVertex[m_numVertices];
    MCGLTexCoord * texCoords = new MCGLTexCoord[m_numVertices];

    int minX = 0;
    int maxX = 0;
    int minY = 0;
    int maxY = 0;

    int vertexIndex = 0;
    for (int faceIndex = 0; faceIndex < NUM_FACES; faceIndex++)
    {
        const MCMesh::Face & face = faces.at(faceIndex);
        const int numFaceVertices = face.vertices.size();
        assert(numFaceVertices == 3); // Only triagles accepted

        for (int faceVertexIndex = 0; faceVertexIndex < numFaceVertices; faceVertexIndex++)
        {
            vertices[vertexIndex].setX(face.vertices.at(faceVertexIndex).x);
            vertices[vertexIndex].setY(face.vertices.at(faceVertexIndex).y);
            vertices[vertexIndex].setZ(face.vertices.at(faceVertexIndex).z);

            normals[vertexIndex].setX(face.vertices.at(faceVertexIndex).i);
            normals[vertexIndex].setY(face.vertices.at(faceVertexIndex).j);
            normals[vertexIndex].setZ(face.vertices.at(faceVertexIndex).k);

            texCoords[vertexIndex].u = face.vertices.at(faceVertexIndex).u;
            texCoords[vertexIndex].v = face.vertices.at(faceVertexIndex).v;

            if (!vertexIndex)
            {
                minX = vertices[vertexIndex].x();
                maxX = vertices[vertexIndex].x();
                minY = vertices[vertexIndex].y();
                maxY = vertices[vertexIndex].y();
            }
            else
            {
                minX = std::min(minX, static_cast<int>(vertices[vertexIndex].x()));
                maxX = std::max(maxX, static_cast<int>(vertices[vertexIndex].x()));
                minY = std::min(minY, static_cast<int>(vertices[vertexIndex].y()));
                maxY = std::max(maxY, static_cast<int>(vertices[vertexIndex].y()));
            }

            vertexIndex++;
        }
    }

    m_w = maxX - minX;
    m_h = maxY - minY;

    GLfloat * colors = new GLfloat[m_numVertices * NUM_COLOR_COMPONENTS];
    for (int colorIndex = 0; colorIndex < m_numVertices * NUM_COLOR_COMPONENTS; colorIndex++)
    {
        colors[colorIndex] = 1.0f;
    }

    initVBOs(vertices, normals, texCoords, colors);

    delete [] vertices;
    delete [] normals;
    delete [] texCoords;
    delete [] colors;
}

void MCMesh::initVBOs(
    const MCGLVertex   * vertices,
    const MCGLVertex   * normals,
    const MCGLTexCoord * texCoords,
    const GLfloat      * colors)
{
    static const int VERTEX_DATA_SIZE   = sizeof(MCGLVertex)   * m_numVertices;
    static const int NORMAL_DATA_SIZE   = sizeof(MCGLVertex)   * m_numVertices;
    static const int TEXCOORD_DATA_SIZE = sizeof(MCGLTexCoord) * m_numVertices;
    static const int COLOR_DATA_SIZE    = sizeof(GLfloat)      * m_numVertices * NUM_COLOR_COMPONENTS;
    static const int TOTAL_DATA_SIZE    =
        VERTEX_DATA_SIZE + NORMAL_DATA_SIZE + TEXCOORD_DATA_SIZE + COLOR_DATA_SIZE;

    int offset = 0;

    createVAO();
    createVBO();

    bindVAO();
    bindVBO();

    glBufferData(GL_ARRAY_BUFFER,
        TOTAL_DATA_SIZE, nullptr, GL_STATIC_DRAW);

    // Vertex data
    glBufferSubData(GL_ARRAY_BUFFER, offset, VERTEX_DATA_SIZE, vertices);
    offset += VERTEX_DATA_SIZE;

    // Normal data
    glBufferSubData(GL_ARRAY_BUFFER, offset, NORMAL_DATA_SIZE, normals);
    offset += NORMAL_DATA_SIZE;

    // Texture coordinate data
    glBufferSubData(GL_ARRAY_BUFFER, offset, TEXCOORD_DATA_SIZE, texCoords);
    offset += TEXCOORD_DATA_SIZE;

    // Vertex color data
    glBufferSubData(GL_ARRAY_BUFFER, offset, COLOR_DATA_SIZE, colors);

    glVertexAttribPointer(MCGLShaderProgram::VAL_Vertex,    3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(MCGLShaderProgram::VAL_Normal,    3, GL_FLOAT, GL_FALSE, 0,
        reinterpret_cast<GLvoid *>(VERTEX_DATA_SIZE));
    glVertexAttribPointer(MCGLShaderProgram::VAL_TexCoords, 2, GL_FLOAT, GL_FALSE, 0,
        reinterpret_cast<GLvoid *>(VERTEX_DATA_SIZE + NORMAL_DATA_SIZE));
    glVertexAttribPointer(MCGLShaderProgram::VAL_Color,     4, GL_FLOAT, GL_FALSE, 0,
        reinterpret_cast<GLvoid *>(VERTEX_DATA_SIZE + NORMAL_DATA_SIZE + TEXCOORD_DATA_SIZE));

    glEnableVertexAttribArray(MCGLShaderProgram::VAL_Vertex);
    glEnableVertexAttribArray(MCGLShaderProgram::VAL_Normal);
    glEnableVertexAttribArray(MCGLShaderProgram::VAL_TexCoords);
    glEnableVertexAttribArray(MCGLShaderProgram::VAL_Color);

    releaseVBO();
    releaseVAO();
}

void MCMesh::render()
{
    glDrawArrays(GL_TRIANGLES, 0, m_numVertices);
}

void MCMesh::setColor(const MCGLColor & color)
{
    m_color = color;
}

void MCMesh::setScale(MCFloat x, MCFloat y, MCFloat z)
{
    m_sx = x;
    m_sy = y;
    m_sz = z;
}

void MCMesh::setScale(MCFloat w, MCFloat h)
{
    m_sx = w / m_w;
    m_sy = h / m_h;
}

void MCMesh::render(MCCamera * camera, MCVector3dFR pos, MCFloat angle, bool autoBind)
{
    if (autoBind)
    {
        bind();
    }

    MCFloat x = pos.i();
    MCFloat y = pos.j();
    MCFloat z = pos.k();

    if (camera)
    {
        camera->mapToCamera(x, y);
    }

    shaderProgram()->bind();
    shaderProgram()->setScale(m_sx, m_sy, m_sz);
    shaderProgram()->setColor(m_color);
    shaderProgram()->setTransform(angle, MCVector3dF(x, y, z));

    render();

    if (autoBind)
    {
        release();
    }
}

void MCMesh::renderShadow(MCCamera * camera, MCVector2dFR pos, MCFloat angle, bool autoBind)
{
    if (autoBind)
    {
        bindShadow();
    }

    MCFloat x = pos.i();
    MCFloat y = pos.j();
    MCFloat z = 0;

    if (camera)
    {
        camera->mapToCamera(x, y);
    }

    shadowShaderProgram()->bind();
    shadowShaderProgram()->setScale(m_sx, m_sy, m_sz);
    shadowShaderProgram()->setTransform(angle, MCVector3dF(x, y, z));

    render();

    if (autoBind)
    {
        releaseShadow();
    }
}

MCFloat MCMesh::width() const
{
    return m_w;
}

MCFloat MCMesh::height() const
{
    return m_h;
}
