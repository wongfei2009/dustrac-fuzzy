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

#ifndef PYTHONCONTROLLER_HPP
#define PYTHONCONTROLLER_HPP

#include <pidcontroller.hpp>
#include <fl/Headers.h>
#include <string>
#include <fstream>
#include "pydata.hpp"

#include <Python.h>

//! A class that allows implementing the controller in Python. 
class PythonController: public PIDController {
public:
	PythonController(Car& car, PyObject* creation_method, const PyDataMakerPtr& dataMaker);
	virtual ~PythonController();

protected:
	//! Steering logic. Returns the steering angle in degrees.
	//! Negative means left.
	virtual float steerControl(bool isRaceCompleted);

	//! Brake/accelerate logic. Returns the prescribed speed.
	//! Negative values mean braking.
	virtual float speedControl(bool isRaceCompleted);

private:
	PyObject* m_controller = nullptr;
	PyObject* m_steerControl = nullptr;
	PyObject* m_speedControl = nullptr;
	PyDataMakerPtr m_dataMaker = nullptr;
};

#endif // PYTHONCONTROLLER_HPP
