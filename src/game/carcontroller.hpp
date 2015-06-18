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
#include "listenerbank.hpp"

class Car;
class Track;
class Route;

//! The base class of car controllers.
class CarController {
public:
	//! @param random Specifies whether the controller will
	//! have stochastic elements (so that all cars do not do the same).
	CarController(Car& car);
	virtual ~CarController() = default;
	
	//! Calls upon computeSignals, steerControl and speedControl
	//! (in that order) and applies the control signals.
	virtual void update(bool isRaceCompleted);

	//! Set the current race track.
	void setTrack(Track & track);

	//! Get associated car.
	Car& car() const;

public:
	const std::set<ListenerPtr>* getListeners() const {return m_listeners;}
	void setListeners(const std::set<ListenerPtr>* listeners) {m_listeners = listeners;}
	void resetListeners() {m_listeners = nullptr;}

protected:
	void report(float steerControl, float speedControl, bool isRaceCompleted);

protected:
	//! Steering logic. Returns the steering angle in degrees.
	//! Negative means left.
	virtual float steerControl(bool isRaceCompleted) = 0;

	//! Brake/accelerate logic. Returns the prescribed speed.
	//! Negative values mean braking.
	virtual float speedControl(bool isRaceCompleted) = 0;

protected:
	Car         & m_car;
	Track       * m_track;
	const Route * m_route;
	const std::set<ListenerPtr>* m_listeners = nullptr;
};

typedef std::shared_ptr<CarController> AIPtr;

#endif // CARCONTROLLER_HPP
