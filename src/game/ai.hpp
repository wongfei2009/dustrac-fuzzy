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

#ifndef AI_HPP
#define AI_HPP

#include <MCVector2d>

#include <memory>

class Car;
class Route;
class TargetNodeBase;
class Track;
class TrackTile;

//! The base class of car controllers.
class CarController {
public:
	CarController(Car & car);
	virtual void update(bool isRaceCompleted) = 0;
	virtual ~CarController() = default;

	//! Set the current race track.
	void setTrack(Track & track);

	//! Get associated car.
	Car& car() const;

protected:
	Car         & m_car;
	Track       * m_track;
	const Route * m_route;
};

//! Class that implements the artificial intelligence of the computer players.
class AI: public CarController {
public:
    //! Constructor.
    AI(Car & car);

    //! Virtual destructor.
    virtual ~AI() = default;

    //! Update.
    virtual void update(bool isRaceCompleted);

private:
    //! Steering logic.
    void steerControl(TargetNodeBase & tnode);

    //! Brake/accelerate logic.
    void speedControl(TrackTile & currentTile, bool isRaceCompleted);

    void setRandomTolerance();

    int           m_lastDiff;
    int           m_lastTargetNodeIndex;
    MCVector2dF   m_randomTolerance;
};

typedef std::shared_ptr<CarController> AIPtr;

#endif // AI_HPP
