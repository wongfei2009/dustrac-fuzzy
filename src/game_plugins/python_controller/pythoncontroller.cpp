#include "pythoncontroller.hpp"

#include <car.hpp>
#include <track.hpp>
#include <trackdata.hpp>
#include <tracktile.hpp>
#include <../common/route.hpp>
#include <../common/targetnodebase.hpp>
#include <../common/tracktilebase.hpp>

#include <MCTrigonom>
#include "pythonexception.hpp"

PythonController::PythonController(Car& car, PyObject* creation_method, const PyDataMakerPtr& dataMaker):
	PIDController(car, false), m_dataMaker(dataMaker)
{
	if (PyCallable_Check(creation_method)) {
        m_controller = PyObject_CallObject(creation_method, NULL);
    } else {
        throw PythonException("The specified Python creation function is not a callable.");
    }

	if(!m_controller) throw PythonException("The Python creation function has not returned a valid object.");

	if(PyObject_HasAttrString(m_controller, "steerControl")) {
		m_steerControl = PyObject_GetAttrString(m_controller, "steerControl");
		if(!PyCallable_Check(m_steerControl))
			throw PythonException("Name steerControl did not resolve into a valid method.");
	}

	if(PyObject_HasAttrString(m_controller, "speedControl")) {
		m_speedControl = PyObject_GetAttrString(m_controller, "speedControl");
		if(!PyCallable_Check(m_speedControl))
			throw PythonException("Name speedControl did not resolve into a valid method.");
	}
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
		PyObject* data = m_dataMaker->makeData(m_data);

		PyObject* controlObj = PyObject_CallFunctionObjArgs(m_steerControl, data, NULL);
		if(!controlObj) throw PythonException("An error calling python steerControl.");

		float control = PyFloat_AsDouble(controlObj);
		Py_DECREF(data);
		Py_DECREF(controlObj);

		return control;
	} else {
		return PIDController::steerControl(isRaceCompleted);
	}
}

//! Brake/accelerate logic. Returns the prescribed speed.
//! Negative values mean braking.
float PythonController::speedControl(bool isRaceCompleted) {
	if(m_speedControl) {
		PyObject* data = m_dataMaker->makeData(m_data);
		PyObject* controlObj = PyObject_CallFunctionObjArgs(m_speedControl, data, NULL);

		if(!controlObj) throw PythonException("An error calling python steerControl.");

		float control = PyFloat_AsDouble(controlObj);
		Py_DECREF(data);
		Py_DECREF(controlObj);

		return control;
	} else {
		return PIDController::speedControl(isRaceCompleted);
	}
}
