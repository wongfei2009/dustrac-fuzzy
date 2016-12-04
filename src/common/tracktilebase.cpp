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

#include "tracktilebase.hpp"
#include "trackdatabase.hpp"

TrackTileBase::TrackTileBase(
    TrackDataBase & trackData,
    QPointF location,
    QPoint matrixLocation,
    const QString & type)
: m_tileType(type)
, m_location(location)
, m_matrixLocation(matrixLocation)
, m_trackData(trackData)
, m_computerHint(TrackTileBase::CH_NONE)
{
}

QPointF TrackTileBase::location() const
{
    return m_location;
}

void TrackTileBase::setLocation(QPointF location)
{
    m_location = location;
}

QPoint TrackTileBase::matrixLocation() const
{
    return m_matrixLocation;
}

void TrackTileBase::setMatrixLocation(QPoint matrixLocation)
{
    m_matrixLocation = matrixLocation;
}

void TrackTileBase::setTileType(const QString & type)
{
    if (type != m_tileType)
    {
        m_tileType = type;
    }
}

const QString & TrackTileBase::tileType() const
{
    return m_tileType;
}

TrackDataBase & TrackTileBase::trackData() const
{
    return m_trackData;
}

void TrackTileBase::setComputerHint(TrackTileBase::ComputerHint hint)
{
    m_computerHint = hint;
}

TrackTileBase::ComputerHint TrackTileBase::computerHint() const
{
    return m_computerHint;
}

TrackTileBase::~TrackTileBase()
{
}
