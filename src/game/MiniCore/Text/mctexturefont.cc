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

#include "mctexturefont.hh"
#include "../Core/mcsurface.hh"

MCTextureFont::MCTextureFont(MCSurface & surface)
  : m_default(
    0,
    MCTextureGlyph::UV(0, 0),
    MCTextureGlyph::UV(1, 0),
    MCTextureGlyph::UV(1, 1),
    MCTextureGlyph::UV(0, 1))
  , m_glyphLookUp(256, m_default)
  , m_surface(surface)
{
}

void MCTextureFont::addGlyphMapping(int glyphId, MCTextureGlyph textureGlyph)
{
    if (static_cast<unsigned int>(glyphId) < m_glyphLookUp.size())
    {
        m_glyphLookUp[glyphId] = textureGlyph;
    }
    else
    {
        m_glyphs.insert(GlyphHash::value_type(glyphId, textureGlyph));
    }
}

MCTextureGlyph & MCTextureFont::glyph(int glyphId)
{
    if (static_cast<unsigned int>(glyphId) < m_glyphLookUp.size())
    {
        return m_glyphLookUp[glyphId];
    }
    else
    {
        auto textureGlyph = m_glyphs.find(glyphId);
        if (textureGlyph != m_glyphs.end())
        {
            return textureGlyph->second;
        }
    }

    return m_default;
}

const MCSurface & MCTextureFont::surface() const
{
    return m_surface;
}
