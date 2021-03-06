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

#ifndef USERCONTROLLER_HPP
#define USERCONTROLLER_HPP

#include "carcontroller.hpp"

class InputHandler;

//! The base class of car controllers.
class UserController: public CarController {
public:
	UserController(Car& car, InputHandler& handler, unsigned int numPlayer);
	virtual ~UserController() = default;

protected:
	//! Steering logic. Returns the steering angle in degrees.
	//! Negative means left.
	virtual float steerControl(bool isRaceCompleted);

	//! Brake/accelerate logic. Returns the prescribed speed.
	//! Negative values mean braking.
	virtual float speedControl(bool isRaceCompleted);

protected:
	InputHandler	& m_handler;
	unsigned int	m_num_player;
	static const float controlStep;
};

#endif // USERCONTROLLER_HPP
