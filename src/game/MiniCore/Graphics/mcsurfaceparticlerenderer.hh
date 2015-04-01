// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCSURFACEPARTICLERENDERER_HH
#define MCSURFACEPARTICLERENDERER_HH

#include <MCGLEW>

#include "mcmacros.hh"
#include "mcglvertex.hh"
#include "mcglcolor.hh"
#include "mcglobjectbase.hh"
#include "mcgltexcoord.hh"
#include "mcworldrenderer.hh"

#include <vector>

class MCSurfaceParticle;
class MCGLShaderProgram;
class MCCamera;
class MCObject;

/*! Each MCGLRectParticle id should have a corresponding MCSurfaceParticleRenderer
 *  registered to MCWorldRenderer. As for rendering, surface particles are special cases, because
 *  they need to be as efficient as possible. This is why a dedicated renderer is needed. */
class MCSurfaceParticleRenderer : public MCGLObjectBase
{
public:

    explicit MCSurfaceParticleRenderer(int maxBatchSize = 1024);

    //! Destructor.
    virtual ~MCSurfaceParticleRenderer();

    //! Enable/disable blending.
    virtual void setAlphaBlend(
        bool useAlphaBlend, GLenum src = GL_SRC_ALPHA, GLenum dst = GL_ONE_MINUS_SRC_ALPHA);

    virtual void setHasShadow(bool hasShadow);

private:

    DISABLE_COPY(MCSurfaceParticleRenderer);
    DISABLE_ASSI(MCSurfaceParticleRenderer);

    /*! Populate the current batch.
     *  \param particles The vector of particle data to be rendered.
     *  \param camera The camera window. */
    typedef std::vector<MCObject *> ParticleVector;
    void setBatch(const ParticleVector & particles, MCCamera * camera = nullptr);

    //! Render the current particle batch.
    void render();

    //! Render the current particle batch as shadows.
    void renderShadows();

    int m_batchSize;

    int m_maxBatchSize;

    MCGLVertex * m_vertices;

    MCGLVertex * m_normals;

    MCGLTexCoord * m_texCoords;

    MCGLColor * m_colors;

    bool m_useAlphaBlend;

    GLenum m_src;

    GLenum m_dst;

    bool m_hasShadow;

    friend class MCWorldRenderer;
};

#endif // MCSURFACEPARTICLERENDERER_HH
