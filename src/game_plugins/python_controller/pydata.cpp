#include "pydata.hpp"
#include "pythonexception.hpp"

#include <piddata.hpp>
#include <MCException>

PyDataMaker::PyDataMaker(PyObject* bindingsModule)
{
	// pModuleDict is a borrowed reference
	if(!bindingsModule) throw MCException("NULL pointer to bindings module.");
	PyObject* pDict = PyModule_GetDict(bindingsModule);

	// PIDData
	m_dataMethod = PyDict_GetItemString(pDict, "PIDData");
	if(!m_dataMethod) throw PythonException("Function 'PIDData' not found in the Python module.");
	if(!PyCallable_Check(m_dataMethod)) throw MCException("Function 'PIDData' is not callable.");

	// PIDData
	m_diffMethod = PyDict_GetItemString(pDict, "DiffStore");
	if(!m_diffMethod) throw PythonException("Function 'DiffStore' not found in the Python module.");
	if(!PyCallable_Check(m_diffMethod)) throw MCException("Function 'DiffStore' is not callable.");
}

PyDataMaker::~PyDataMaker() {
	Py_DECREF(m_dataMethod);
	Py_DECREF(m_diffMethod);
}

PyObject* PyDataMaker::makeData(const DiffStore& diffData)
{
	return PyObject_CallFunction(m_diffMethod, const_cast<char*>("fff"), diffData.error, diffData.deltaError, diffData.deltaError2);
}

PyObject* PyDataMaker::makeData(const PIDData& pidData) {
	PyObject* angularErrors = makeData(pidData.angularErrors);
	PyObject* distanceErrors = makeData(pidData.distanceErrors);
	PyObject* steerControl = PyFloat_FromDouble(pidData.steerControl);
	PyObject* speedControl = PyFloat_FromDouble(pidData.speedControl);

	PyObject* data = PyObject_CallFunctionObjArgs(m_dataMethod, angularErrors, distanceErrors, steerControl, speedControl, NULL);

	Py_DECREF(angularErrors);
	Py_DECREF(distanceErrors);
	Py_DECREF(steerControl);
	Py_DECREF(speedControl);

	return data;
}
