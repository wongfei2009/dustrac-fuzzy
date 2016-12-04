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

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QSysInfo>
#include <MCGLEW>

#include <cassert>
#include <cmath>
#include <exception>

MCSurfaceManager::MCSurfaceManager()
{
}

inline bool colorMatch(int val1, int val2, int threshold)
{
    return (val1 >= val2 - threshold) && (val1 <= val2 + threshold);
}

// This function is taken from Qt in order to drop dependency to QGLWidget::convertToGLFormat().
static inline QRgb qt_gl_convertToGLFormatHelper(QRgb src_pixel, GLenum texture_format)
{
    if (texture_format == GL_BGRA) {
        if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
            return ((src_pixel << 24) & 0xff000000)
                   | ((src_pixel >> 24) & 0x000000ff)
                   | ((src_pixel << 8) & 0x00ff0000)
                   | ((src_pixel >> 8) & 0x0000ff00);
        } else {
            return src_pixel;
        }
    } else {  // GL_RGBA
        if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
            return (src_pixel << 8) | ((src_pixel >> 24) & 0xff);
        } else {
            return ((src_pixel << 16) & 0xff0000)
                   | ((src_pixel >> 16) & 0xff)
                   | (src_pixel & 0xff00ff00);
        }
    }
}

// This function is taken from Qt in order to drop dependency to QGLWidget::convertToGLFormat().
static void convertToGLFormatHelper(QImage &dst, const QImage &img, GLenum texture_format)
{
    Q_ASSERT(dst.depth() == 32);
    Q_ASSERT(img.depth() == 32);

    if (dst.size() != img.size()) {
        int target_width = dst.width();
        int target_height = dst.height();
        qreal sx = target_width / qreal(img.width());
        qreal sy = target_height / qreal(img.height());

        quint32 *dest = (quint32 *) dst.scanLine(0); // NB! avoid detach here
        uchar *srcPixels = (uchar *) img.scanLine(img.height() - 1);
        int sbpl = img.bytesPerLine();
        int dbpl = dst.bytesPerLine();

        int ix = int(0x00010000 / sx);
        int iy = int(0x00010000 / sy);

        quint32 basex = int(0.5 * ix);
        quint32 srcy = int(0.5 * iy);

        // scale, swizzle and mirror in one loop
        while (target_height--) {
            const uint *src = (const quint32 *) (srcPixels - (srcy >> 16) * sbpl);
            int srcx = basex;
            for (int x=0; x<target_width; ++x) {
                dest[x] = qt_gl_convertToGLFormatHelper(src[srcx >> 16], texture_format);
                srcx += ix;
            }
            dest = (quint32 *)(((uchar *) dest) + dbpl);
            srcy += iy;
        }
    } else {
        const int width = img.width();
        const int height = img.height();
        const uint *p = (const uint*) img.scanLine(img.height() - 1);
        uint *q = (uint*) dst.scanLine(0);

        if (texture_format == GL_BGRA) {
            if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
                // mirror + swizzle
                for (int i=0; i < height; ++i) {
                    const uint *end = p + width;
                    while (p < end) {
                        *q = ((*p << 24) & 0xff000000)
                             | ((*p >> 24) & 0x000000ff)
                             | ((*p << 8) & 0x00ff0000)
                             | ((*p >> 8) & 0x0000ff00);
                        p++;
                        q++;
                    }
                    p -= 2 * width;
                }
            } else {
                const uint bytesPerLine = img.bytesPerLine();
                for (int i=0; i < height; ++i) {
                    memcpy(q, p, bytesPerLine);
                    q += width;
                    p -= width;
                }
            }
        } else {
            if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
                for (int i=0; i < height; ++i) {
                    const uint *end = p + width;
                    while (p < end) {
                        *q = (*p << 8) | ((*p >> 24) & 0xff);
                        p++;
                        q++;
                    }
                    p -= 2 * width;
                }
            } else {
                for (int i=0; i < height; ++i) {
                    const uint *end = p + width;
                    while (p < end) {
                        *q = ((*p << 16) & 0xff0000) | ((*p >> 16) & 0xff) | (*p & 0xff00ff00);
                        p++;
                        q++;
                    }
                    p -= 2 * width;
                }
            }
        }
    }
}

MCSurface & MCSurfaceManager::createSurfaceFromImage(const MCSurfaceMetaData & data, QImage image)
{
    // Store original width of the image
    int origH = data.height.second ? data.height.first : image.height();
    int origW = data.width.second  ? data.width.first  : image.width();

    // Take maximum supported texture size into account
    GLint maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    while (image.width() > maxTextureSize || image.height() > maxTextureSize)
    {
        image = image.scaled(image.width() / 2, image.height() / 2);
    }

    // Create material. Possible secondary textures are taken from surfaces
    // that are initialized before this surface.
    MCGLMaterialPtr material(new MCGLMaterial);
    material->setTexture(create2DTextureFromImage(data, image), 0);
    material->setTexture(data.handle2.length() ? surface(data.handle2).material()->texture(0) : 0, 1);
    material->setTexture(data.handle3.length() ? surface(data.handle3).material()->texture(0) : 0, 2);

    if (data.specularCoeff.second)
    {
        material->setSpecularCoeff(data.specularCoeff.first);
    }

    // Create a new MCSurface object
    MCSurface * surface =
        new MCSurface(material, origW, origH, data.z0, data.z1, data.z2, data.z3);

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

    QImage glFormattedImage(textureImage.width(), textureImage.height(), textureImage.format());
    convertToGLFormatHelper(glFormattedImage, textureImage, GL_RGBA);

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

    if (data.wrapS.second)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, data.wrapS.first);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    }

    if (data.wrapT.second)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, data.wrapT.first);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // Edit image data using the information textureImage gives us
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        glFormattedImage.width(), glFormattedImage.height(),
        0, GL_RGBA, GL_UNSIGNED_BYTE, glFormattedImage.bits());

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

MCSurfaceManager::~MCSurfaceManager()
{
    // Delete OpenGL textures and Textures
    auto iter(m_surfaceMap.begin());
    while (iter != m_surfaceMap.end())
    {
        if (iter->second)
        {
            MCSurface * p = iter->second;
            for (unsigned int i = 0; i < MCGLMaterial::MAX_TEXTURES; i++)
            {
                GLuint dummyHandle1 = p->material()->texture(i);
                glDeleteTextures(1, &dummyHandle1);
            }
            delete p;
        }
        iter++;
    }
}

void MCSurfaceManager::load(
    const std::string & configFilePath, const std::string & baseDataPath)
{
    MCSurfaceConfigLoader loader;

    // Parse the texture config file
    if (loader.load(configFilePath))
    {
        for (unsigned int i = 0; i < loader.surfaceCount(); i++)
        {
            const MCSurfaceMetaData & metaData = loader.surface(i);

            // Load the image and create a 2D texture. Due to possible Android asset URLs,
            // an explicit QFile-based loading is used instead of directly using QImage::loadFromFile().
            QString path = QString(baseDataPath.c_str()) + QDir::separator() + metaData.imagePath.c_str();
            path.replace("./", "");
            path.replace("//", "/");

            QFile imageFile(path);
            if (!imageFile.open(QIODevice::ReadOnly))
            {
                throw std::runtime_error("Cannot read file '" + path.toStdString() + "'");
            }
            QByteArray blob = imageFile.readAll();

            QImage textureImage;
            textureImage.loadFromData(blob);
            createSurfaceFromImage(metaData, textureImage);
        }
    }
    else
    {
        // Throw an exception
        throw std::runtime_error("Parsing '" + configFilePath + "' failed!");
    }
}

MCSurface & MCSurfaceManager::surface(const std::string & id) const
{
    // Try to find existing texture for the surface
    if (m_surfaceMap.count(id) == 0)
    {
        throw std::runtime_error("Cannot find texture object for handle '" + id + "'");
    }

    // Yes: return handle for the texture
    MCSurface * pSurface = m_surfaceMap.find(id)->second;
    assert(pSurface);
    return *pSurface;
}
