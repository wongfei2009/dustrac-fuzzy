#include "pydata.hpp"

#include <piddata.hpp>
#include <MCException>

PyDataMaker::PyDataMaker(PyObject* dataMethod)
{
	if(!PyCallable_Check(dataMethod)) throw MCException("The specified Python data creation function is not a callable.");
	m_dataMethod = dataMethod;
}

PyObject* PyDataMaker::makeData(const PIDData& pidData) {
	return PyObject_CallFunction(m_dataMethod, const_cast<char*>("ffff"), pidData.error, pidData.deltaError, pidData.deltaError2, pidData.prevControl);
}
