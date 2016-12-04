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

#ifndef MCWORLDRENDERER_HH
#define MCWORLDRENDERER_HH

#include "mcglscene.hh"
#include "mcrenderlayer.hh"
#include "mctypes.hh"
#include "mcworld.hh"

#include <map>
#include <memory>
#include <unordered_set>
#include <vector>

class MCCamera;
class MCObject;

class MCSurfaceParticleRenderer;

//! Helper class used by MCWorld. Renders all objects in the scene.
class MCWorldRenderer
{
public:

    MCWorldRenderer();

    ~MCWorldRenderer();

    //! \return the current OpenGL scene object.
    MCGLScene & glScene();

    /*! Enables/disables depth test on given render layer. Default is true. */
    void enableDepthTestOnLayer(int layer, bool enable = true);

    /*! Enables/disables depth mask (write to depth buffer) on given render layer.
     *  Default is true. */
    void enableDepthMaskOnLayer(int layer, bool enable = true);

    /*! If a particle gets outside all visibility cameras, it'll be killed.
     *  This is just an optimization. The camera given to render()
     *  cannot be used, because there might be multiple cameras and viewports.
     *  If no cameras are set, particles will be always drawn. */
    void addParticleVisibilityCamera(MCCamera & camera);

    /*! Remove all particle visibility cameras. */
    void removeParticleVisibilityCameras();

private:

    void addToLayerMap(MCObject & object);

    /*! Must be called before calls to render() or renderShadows() */
    void buildBatches(MCCamera * camera);

    void clear();

    void removeFromLayerMap(MCObject & object);

    void render(MCCamera * camera, const std::vector<int> & layers);

    void renderBatches(MCCamera * camera = nullptr, const std::vector<int> & layers = std::vector<int>());

    void renderObjectBatches(MCCamera * camera, MCRenderLayer & layer);

    void renderParticleBatches(MCCamera * camera, MCRenderLayer & layer);

    void renderShadows(MCCamera * camera, const std::vector<int> & layers);

    void renderObjectShadowBatches(MCCamera * camera, MCRenderLayer & layer);

    void renderParticleShadowBatches(MCCamera * camera, MCRenderLayer & layer);

    typedef int LayerId;
    std::map<LayerId, MCRenderLayer> m_layers;

    std::vector<MCCamera *> m_visibilityCameras;

    MCSurfaceParticleRenderer * m_surfaceParticleRenderer;

    MCGLScene m_glScene;

    friend class MCWorld;
    friend class MCObject;
};

#endif // MCWORLDRENDERER_HH
