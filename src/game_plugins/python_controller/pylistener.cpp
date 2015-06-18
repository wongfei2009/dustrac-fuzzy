#include "pylistener.hpp"
#include "pydata.hpp"

#include <car.hpp>
#include <track.hpp>
#include <trackdata.hpp>
#include <../common/route.hpp>
#include <../common/targetnodebase.hpp>
#include <../common/tracktilebase.hpp>
#include <MCTrigonom>
#include <MCException>

PyListener::PyListener(PyObject* listenerFunc, const PyDataMakerPtr& dataMaker):
m_steerData(), m_speedData(), m_dataMaker(dataMaker)
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

	MCFloat angle = MCTrigonom::radToDeg(std::atan2(target.j(), target.i()));
	MCFloat cur   = static_cast<int>(car.angle()) % 360;
	MCFloat diff  = angle - cur;

	m_steerData.updateError(diff);
	m_speedData.updateError(diff);

	m_steerData.updateControl(steerControl);
	m_speedData.updateControl(speedControl);

	PyObject* steerData = m_dataMaker->makeData(m_steerData);
	PyObject* speedData = m_dataMaker->makeData(m_speedData);

	PyObject_CallFunctionObjArgs(m_reportFunc, steerData, speedData, NULL);

	Py_DECREF(steerData);
	Py_DECREF(speedData);
}
