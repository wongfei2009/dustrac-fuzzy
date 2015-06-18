#include "pythoncontroller.hpp"

#include <car.hpp>
#include <track.hpp>
#include <trackdata.hpp>
#include <tracktile.hpp>
#include <../common/route.hpp>
#include <../common/targetnodebase.hpp>
#include <../common/tracktilebase.hpp>

#include <MCTrigonom>
#include <MCException>

PythonController::PythonController(Car& car, PyObject* creation_method, const PyDataMakerPtr& dataMaker):
	PIDController(car, false), m_dataMaker(dataMaker)
{
	if (PyCallable_Check(creation_method)) {
        m_controller = PyObject_CallObject(creation_method, NULL);
    }
	else {
        throw MCException("The specified Python creation function is not a callable.");
    }

	if(!m_controller) throw MCException("The Python creation function has not returned a valid object.");

	m_steerControl = PyObject_GetAttrString(m_controller, "steerControl");
	if(m_steerControl and !PyCallable_Check(m_steerControl))
		throw MCException("Name steerControl did not resolve into a valid method.");

	m_speedControl = PyObject_GetAttrString(m_controller, "speedControl");
	if(m_speedControl and !PyCallable_Check(m_speedControl))
		throw MCException("Name speedControl did not resolve into a valid method.");
}

PythonController::~PythonController() {
	if(m_steerControl) Py_DECREF(m_steerControl);
	if(m_speedControl) Py_DECREF(m_speedControl);
	if(m_controller) Py_DECREF(m_controller);
}

//! Steering logic. Returns the steering angle in degrees.
//! Negative means left.
float PythonController::steerControl(bool isRaceCompleted) {
	if(m_steerControl) {
		PyObject* data = m_dataMaker->makeData(m_steeringData);
		float control = PyFloat_AsDouble(PyObject_CallFunctionObjArgs(m_steerControl, data, NULL));
		Py_DECREF(data);

		return control;
	} else {
		return PIDController::steerControl(isRaceCompleted);
	}
}

//! Brake/accelerate logic. Returns the prescribed speed.
//! Negative values mean braking.
float PythonController::speedControl(bool isRaceCompleted) {
	if(m_speedControl) {
		PyObject* data = m_dataMaker->makeData(m_speedData);
		float control = PyFloat_AsDouble(PyObject_CallFunctionObjArgs(m_speedControl, data, NULL));
		Py_DECREF(data);

		return control;
	} else {
		return PIDController::speedControl(isRaceCompleted);
	}
}
