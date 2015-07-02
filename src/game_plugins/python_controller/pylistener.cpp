#include "pylistener.hpp"
#include "pydata.hpp"
#include "pythonexception.hpp"

#include <car.hpp>
#include <track.hpp>
#include <trackdata.hpp>
#include <../common/route.hpp>
#include <../common/targetnodebase.hpp>
#include <../common/tracktilebase.hpp>
#include <MCTrigonom>
#include <MCException>

PyListener::PyListener(PyObject* listenerFunc, const PyDataMakerPtr& dataMaker):
m_data(false), m_dataMaker(dataMaker)
{
	m_listenerObj = PyObject_CallObject(listenerFunc, NULL);
	if(!m_listenerObj) throw MCException("The Python listener creation function has not returned a valid object.");
	
	m_reportFunc = PyObject_GetAttrString(m_listenerObj, "report");
	if(m_reportFunc and !PyCallable_Check(m_reportFunc))
		throw MCException("Name report did not resolve into a valid method.");
}

PyListener::~PyListener() {
	Py_DECREF(m_reportFunc);
	Py_DECREF(m_listenerObj);
}

void PyListener::report(
	const Car& car,
	const Track* track,
	float steerControl,
	float speedControl,
	bool
) {
	const Route    & route       = track->trackData().route();
	TargetNodeBase & tnode       = route.get(car.currentTargetNodeIndex());

	// Initial target coordinates
	MCVector3dF target(tnode.location().x(), tnode.location().y());
	target -= MCVector3dF(car.location());
	double tnodedistance = target.length();

	m_data.updateErrors(car, route);
	m_data.updateControl(steerControl, speedControl);

	PyObject* pidData = m_dataMaker->makeData(m_data);
	PyObject* distance = PyFloat_FromDouble(tnodedistance);

	PyObject* res = PyObject_CallFunctionObjArgs(m_reportFunc, pidData, NULL);
	if(!res) throw PythonException("Unsuccessful call to PyListener's report.");

	Py_DECREF(pidData);
	Py_DECREF(distance);
}
