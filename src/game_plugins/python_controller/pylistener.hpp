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

#ifndef PYLISTENER_HPP
#define PYLISTENER_HPP

#include <listener.hpp>
#include <piddata.hpp>
#include <Python.h>
#include "pydata.hpp"

class PyListener: public Listener {
public:
	PyListener(PyObject* listenerFunc, const PyDataMakerPtr& dataMaker);
	virtual ~PyListener();

public:
	virtual void report(
		const Car& car,
		const Track* track,
		float steerControl,
		float speedControl,
		bool isRaceCompleted
	);

private:
	PIDData m_data;
	PyObject* m_listenerObj = nullptr;
	PyObject* m_reportFunc = nullptr;
	PyDataMakerPtr m_dataMaker = nullptr;
};

#endif // PYLISTENER_HPP

