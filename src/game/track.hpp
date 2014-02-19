// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef TRACK_HPP
#define TRACK_HPP

#include "updateableif.hpp"

#include <MCBBox>
#include <MCGLShaderProgram>
#include <MCTypes>

class TrackData;
class TrackTile;
class MCCamera;
class MCSurface;

//! A renderable race track object constructed from
//! the given track data.
class Track
{
public:

    //! Constructor.
    //! \param pTrackData The data that represents the track.
    //!                   Track will take the ownership.
    explicit Track(TrackData * pTrackData);

    //! Destructor.
    virtual ~Track();

    //! Render as seen through the given camera window.
    void render(MCCamera * camera);

    //! Return width in length units.
    MCUint width() const;

    //! Return height in length units.
    MCUint height() const;

    //! Return the track data.
    TrackData & trackData() const;

    //! Return pointer to the tile at the given location.
    TrackTile * trackTileAtLocation(MCUint x, MCUint y) const;

    //! Return pointer to the finish line tile.
    TrackTile * finishLine() const;

    //! Set the next track.
    void setNext(Track & next);

    //! Return the next track or nullptr.
    Track * next() const;

    //! Set the prev track.
    void setPrev(Track & prev);

    //! Return the prev track or nullptr.
    Track * prev() const;

private:

    void calculateVisibleIndices(const MCBBox<int> & r,
        MCUint & i0, MCUint & i2, MCUint & j0, MCUint & j2);

    void renderAsphalt(
        MCCamera * camera, MCGLShaderProgramPtr prog, MCUint i0, MCUint i2, MCUint j0, MCUint j2);

    void renderTiles(
        MCCamera * camera, MCGLShaderProgramPtr prog, MCUint i0, MCUint i2, MCUint j0, MCUint j2);

    TrackData * m_pTrackData;
    MCUint      m_rows, m_cols, m_width, m_height;
    MCSurface & m_asphalt;
    Track     * m_next;
    Track     * m_prev;
};

#endif // TRACK_HPP
