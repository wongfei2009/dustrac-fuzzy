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

#ifndef CARCONTROLLER_HPP
#define CARCONTROLLER_HPP

#include <memory>

class Car;
class Track;
class Route;

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

typedef std::shared_ptr<CarController> AIPtr;

#endif // CARCONTROLLER_HPP
