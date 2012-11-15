// This file is part of Dust Racing 2D.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "ai.hpp"
#include "car.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"
#include "../common/route.hpp"
#include "../common/targetnodebase.hpp"
#include "../common/tracktilebase.hpp"

#include <MCTrigonom>
#include <MCTypes>

AI::AI(Car & car)
: m_car(car)
, m_track(nullptr)
, m_route(nullptr)
, m_targetIndex(0)
, m_lastDiff(0)
{
}

Car & AI::car() const
{
    return m_car;
}

void AI::update(bool isRaceCompleted)
{
    if (m_track)
    {
        m_car.clearStatuses();

        const Route    & route       = m_track->trackData().route();
        TargetNodeBase & tnode       = route.get(m_car.currentTargetNodeIndex());
        TrackTile      & currentTile = *m_track->trackTileAtLocation(
            m_car.location().i(), m_car.location().j());

        steerControl(tnode);
        speedControl(currentTile, isRaceCompleted);
    }
}

void AI::steerControl(TargetNodeBase & tnode)
{
    // Initial target coordinates
    MCVector3dF target(tnode.location().x(), tnode.location().y());
    target -= MCVector3dF(m_car.location());

    MCFloat angle = MCTrigonom::radToDeg(std::atan2(target.j(), target.i()));
    MCFloat cur   = static_cast<int>(m_car.angle()) % 360;
    MCFloat diff  = angle - cur;

    bool ok = false;
    while (!ok)
    {
        if (diff > 180)
        {
            diff = diff - 360;
            ok = false;
        }
        else if (diff < -180)
        {
            diff = diff + 360;
            ok = false;
        }
        else
        {
            ok = true;
        }
    }

    // PID-controller. This makes the computer players to turn and react faster
    // than the human player, but hey...they are stupid.
    MCFloat control = diff * 0.01 + (diff - m_lastDiff) * 0.01;
    const MCFloat maxControl = 1.0;
    control = control < 0 ? -control : control;
    control = control > maxControl ? maxControl : control;
    m_car.setTurningImpulse(control);

    if (diff < -3.0)
    {
        m_car.turnRight();
    }
    else if (diff > 3.0)
    {
        m_car.turnLeft();
    }

    // Store the last difference
    m_lastDiff = diff;
}

void AI::speedControl(TrackTile & currentTile, bool isRaceCompleted)
{
    // TODO: Maybe it'd be possible to adjust speed according to
    // the difference between current and target angles so that
    // computer hints wouldn't be needed anymore..?

    // Braking / acceleration logic
    bool accelerate = true;
    bool brake      = false;

    if (isRaceCompleted)
    {
        accelerate = false;
    }
    else
    {
        if (currentTile.computerHint() == TrackTile::CH_SECOND_BEFORE_CORNER)
        {
            if (m_car.speedInKmh() > 100)
            {
                brake = true;
            }
        }

        if (currentTile.computerHint() == TrackTile::CH_FIRST_BEFORE_CORNER)
        {
            if (m_car.speedInKmh() > 50)
            {
                brake = true;
            }
        }

        if (currentTile.tileTypeEnum() == TrackTile::TT_CORNER_90)
        {
            if (m_car.speedInKmh() > 30)
            {
                accelerate = false;
            }
        }

        if (currentTile.tileTypeEnum() == TrackTile::TT_CORNER_45_LEFT ||
            currentTile.tileTypeEnum() == TrackTile::TT_CORNER_45_RIGHT)
        {
            if (m_car.speedInKmh() > 60)
            {
                accelerate = false;
            }
        }

        if (m_car.speedInKmh() < 25)
        {
            accelerate = true;
            brake = false;
        }
    }

    if (brake)
    {
        m_car.brake();
    }
    else if (accelerate)
    {
        m_car.accelerate();
    }
}

void AI::setTrack(Track & track)
{
    m_track = &track;
    m_route = &track.trackData().route();
}