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

#ifndef PIDCONTROLLER_HPP
#define PIDCONTROLLER_HPP

#include "carcontroller.hpp"
#include <MCVector2d>

class TargetNodeBase;
class TrackTile;

class PIDController: public CarController {
protected:
	struct PIDData {
		float error = 0;
		float deltaError = 0;
		float deltaError2 = 0;
		float prevControl = 0;

		void updateError(float error_) {
			float tmpDelta = error_ - error;
			error = error_;
			deltaError2 = tmpDelta - deltaError;
			deltaError = tmpDelta;
		}

		void updateControl(float prevControl_) {
			prevControl = prevControl_;
		}
	};

public:
	PIDController(Car& car, bool random);
    virtual ~PIDController() = default;

	//! Update. Computes the signals required to implement PID
	//! control and calls steerControl and speedControl, which
	//! may be reimplemented using other types of control.
	//! In this version steerControl is called before speedControl.
    virtual void update(bool isRaceCompleted);

protected:
	//! Steering logic. Returns the steering angle in degrees.
	//! Negative means left.
	virtual float steerControl(bool isRaceCompleted);

	//! Brake/accelerate logic. Returns the prescribed speed.
	//! Negative values mean braking.
	virtual float speedControl(bool isRaceCompleted);

	//! Sets up a random tolerance vector (only used if m_random
	//! is true).
	void setRandomTolerance();

protected:
	PIDData m_steeringData;
	PIDData m_speedData;
	bool m_random;
	int m_lastTargetNodeIndex;
	MCVector2dF m_randomTolerance;
};

#endif // PIDCONTROLLER_HPP

