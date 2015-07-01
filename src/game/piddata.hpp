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

#ifndef PIDDATA_HPP
#define PIDDATA_HPP

#include <MCVector2d>
#include <cmath>

class Car;
class Route;

class DiffStore {
public:
	float error = 0;
	float deltaError = 0;
	float deltaError2 = 0;

public:
	void update(float error_);
};

class PIDData {
public:
	PIDData(bool random);

public:
	DiffStore angularErrors;
	DiffStore lateralErrors;
	float steerControl;
	float speedControl;
	
public:
	//! Recomputes angular and lateral errors for the car.
	void updateErrors(const Car& car, const Route& route);

	//! Logs the control signals for steering and speed.
	void updateControl(float steerControl_, float speedControl_)
	{
		steerControl = steerControl_;
		speedControl = speedControl_;
	}

private:
	void updateRandomTolerance();
	MCVector2dF generateTolerance() const;

	MCFloat constrainAngle(MCFloat x)
	{
		x = fmod(x + 180,360);
		if (x < 0)
		    x += 360;
		return x - 180;
	}

private:
	//! Specifies whether some random vector is added to the actual route.
	bool m_random;
	//! Index of the last target node on the prescribed route (so that we
	//! know when it changes).
	unsigned int m_lastTargetNodeIndex;
	//! A random vector added to the position of the previous node for this car.
	MCVector2dF m_prevRandomTolerance;
	//! A random vector added to the position of the current node for this car.
	MCVector2dF m_curRandomTolerance;
};

#endif // PIDDATA_HPP

