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

#include "mctypes.hh"
#include "mcsurface.hh"
#include "mcsurfaceconfigloader.hh"
#include "mcsurfacemanager.hh"

#include <QDir>
#include <QGLWidget>
#include <MCGLEW>

#include <cassert>
#include <cmath>

MCSurfaceManager::MCSurfaceManager()
{
}

inline bool colorMatch(int val1, int val2, int threshold)
{
    return (val1 >= val2 - threshold) && (val1 <= val2 + threshold);
}

MCSurface & MCSurfaceManager::createSurfaceFromImage(
    const MCSurfaceMetaData & data, const QImage & image) throw (MCException)
{
    // Store original width of the image
    int origH = data.height.second ? data.height.first : image.height();
    int origW = data.width.second  ? data.width.first  : image.width();

    const GLuint textureHandle1 = create2DTextureFromImage(data, image);
    const GLuint textureHandle2 = data.handle2.length() ? surface(data.handle2).handle1() : 0;
    const GLuint textureHandle3 = data.handle3.length() ? surface(data.handle3).handle1() : 0;

    // Create a new MCSurface object
    MCSurface * surface =
        new MCSurface(textureHandle1, textureHandle2, textureHandle3,
            origW, origH, data.z0, data.z1, data.z2, data.z3);

    assert(surface);
    createSurfaceCommon(*surface, data);

    return *surface;
}

void MCSurfaceManager::createSurfaceCommon(MCSurface & surface, const MCSurfaceMetaData & data)
{
    // Enable alpha blend, if set
    surface.setAlphaBlend(
        data.alphaBlend.second, data.alphaBlend.first.m_src, data.alphaBlend.first.m_dst);

    // Set custom center if it was set
    if (data.center.second)
    {
        surface.setCenter(data.center.first);
    }

    // Store MCSurface to map
    m_surfaceMap[data.handle] = &surface;
}

GLuint MCSurfaceManager::create2DTextureFromImage(
    const MCSurfaceMetaData & data, const QImage & image)
{
    QImage textureImage = image;

    // Flip pA about X-axis if set active
    if (data.xAxisMirror)
    {
        textureImage = textureImage.mirrored(false, true);
    }

    // Ensure alpha channel
    textureImage = textureImage.convertToFormat(QImage::Format_ARGB32);

    // Apply colorkey if it was set (set or clear alpha)
    if (data.colorKeySet)
    {
        applyColorKey(textureImage, data.colorKey.m_r, data.colorKey.m_g, data.colorKey.m_b);
    }

    // Apply brightness if it was set
    if (data.brightness.second)
    {
        applyBrightness(textureImage, data.brightness.first);
    }

    // Apply contrast if it was set
    if (data.contrast.second)
    {
        applyContrast(textureImage, data.contrast.first);
    }

    // Convert to GL_RGBA
    textureImage = QGLWidget::convertToGLFormat(textureImage);

    // Let OpenGL generate a texture handle
    GLuint textureHandle;
    glGenTextures(1, &textureHandle);

    // Bind the texture object
    glBindTexture(GL_TEXTURE_2D, textureHandle);

    // Set min filter.
    if (data.minFilter.second)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, data.minFilter.first);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    // Set mag filter.
    if (data.magFilter.second)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, data.magFilter.first);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    // Edit image data using the information textureImage gives us
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        textureImage.width(), textureImage.height(),
        0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage.bits());

    return textureHandle;
}

void MCSurfaceManager::applyColorKey(QImage & textureImage, MCUint r, MCUint g, MCUint b) const
{
    for (int i = 0; i < textureImage.width(); i++)
    {
        for (int j = 0; j < textureImage.height(); j++)
        {
            if (colorMatch( textureImage.pixel(i, j) & 0x000000ff,        b, 2) &&
                colorMatch((textureImage.pixel(i, j) & 0x0000ff00) >> 8,  g, 2) &&
                colorMatch((textureImage.pixel(i, j) & 0x00ff0000) >> 16, r, 2))
            {
                textureImage.setPixel(i, j, textureImage.pixel(i, j) & 0x00000000);
            }
            else
            {
                textureImage.setPixel(i, j, textureImage.pixel(i, j) | 0xff000000);
            }
        }
    }
}

void MCSurfaceManager::applyBrightness(QImage & textureImage, float brightness) const
{
    for (int i = 0; i < textureImage.width(); i++)
    {
        for (int j = 0; j < textureImage.height(); j++)
        {
            int r = (textureImage.pixel(i, j) & 0x00ff0000) >> 16;
            int g = (textureImage.pixel(i, j) & 0x0000ff00) >> 8;
            int b = (textureImage.pixel(i, j) & 0x000000ff);

            const unsigned int a = (textureImage.pixel(i, j) & 0xff000000);

            r = static_cast<int>((static_cast<float>(r)) * brightness);
            g = static_cast<int>((static_cast<float>(g)) * brightness);
            b = static_cast<int>((static_cast<float>(b)) * brightness);

            r = r < 0   ? 0   : r;
            r = r > 255 ? 255 : r;
            g = g < 0   ? 0   : g;
            g = g > 255 ? 255 : g;
            b = b < 0   ? 0   : b;
            b = b > 255 ? 255 : b;

            textureImage.setPixel(i, j, r << 16 | g << 8 | b | a);
        }
    }
}

void MCSurfaceManager::applyContrast(QImage & textureImage, float contrast) const
{
    for (int i = 0; i < textureImage.width(); i++)
    {
        for (int j = 0; j < textureImage.height(); j++)
        {
            int r = (textureImage.pixel(i, j) & 0x00ff0000) >> 16;
            int g = (textureImage.pixel(i, j) & 0x0000ff00) >> 8;
            int b = (textureImage.pixel(i, j) & 0x000000ff);

            const unsigned int a = (textureImage.pixel(i, j) & 0xff000000);

            float fr = static_cast<float>(r) / 256.0;
            float fg = static_cast<float>(g) / 256.0;
            float fb = static_cast<float>(b) / 256.0;

            if (contrast > 1.0)
            {
                fr = fr > 0.5 ? fr + (contrast - 1.0) : fr - (contrast - 1.0);
                fg = fg > 0.5 ? fg + (contrast - 1.0) : fg - (contrast - 1.0);
                fb = fb > 0.5 ? fb + (contrast - 1.0) : fb - (contrast - 1.0);
            }
            else
            {
                fr = fr > 0.5 ? fr - (1.0 - contrast) : fr + (1.0 - contrast);
                fg = fg > 0.5 ? fg - (1.0 - contrast) : fg + (1.0 - contrast);
                fb = fb > 0.5 ? fb - (1.0 - contrast) : fb + (1.0 - contrast);
            }

            r = static_cast<int>(fr * 256.0);
            g = static_cast<int>(fg * 256.0);
            b = static_cast<int>(fb * 256.0);

            r = r < 0   ? 0   : r;
            r = r > 255 ? 255 : r;
            g = g < 0   ? 0   : g;
            g = g > 255 ? 255 : g;
            b = b < 0   ? 0   : b;
            b = b > 255 ? 255 : b;

            textureImage.setPixel(i, j, r << 16 | g << 8 | b | a);
        }
    }
}

MCSurfaceManager::~MCSurfaceManager()
{
    // Delete OpenGL textures and Textures
    auto iter(m_surfaceMap.begin());
    while (iter != m_surfaceMap.end())
    {
        if (iter->second)
        {
            MCSurface * p = iter->second;

            GLuint dummyHandle1 = p->handle1();
            glDeleteTextures(1, &dummyHandle1);

            GLuint dummyHandle2 = p->handle2();
            if (dummyHandle2)
            {
                glDeleteTextures(1, &dummyHandle2);
            }

            delete p;
        }
        iter++;
    }
}

void MCSurfaceManager::load(
    const std::string & configFilePath, const std::string & baseDataPath) throw (MCException)
{
    MCSurfaceConfigLoader loader;

    // Parse the texture config file
    if (loader.load(configFilePath))
    {
        for (unsigned int i = 0; i < loader.surfaceCount(); i++)
        {
            const MCSurfaceMetaData & data = loader.surface(i);

            const std::string path =
                baseDataPath + QDir::separator().toLatin1() + data.imagePath;

            // Load the image and create a 2D texture.
            QImage textureImage;
            if (textureImage.load(path.c_str()))
            {
                createSurfaceFromImage(data, textureImage);
            }
            else
            {
                throw MCException("Cannot read file '" + path + "'");
            }
        }
    }
    else
    {
        // Throw an exception
        throw MCException("Parsing '" + configFilePath + "' failed!");
    }
}

MCSurface & MCSurfaceManager::surface(const std::string & id) const throw (MCException)
{
    // Try to find existing texture for the surface
    if (m_surfaceMap.count(id) == 0)
    {
        throw MCException("Cannot find texture object for handle '" + id + "'");
    }

    // Yes: return handle for the texture
    MCSurface * pSurface = m_surfaceMap.find(id)->second;
    assert(pSurface);
    return *pSurface;
}
