#include "pythonexception.hpp"
#include <Python.h>

PythonException::PythonException(const std::string& what_arg):
	std::runtime_error(""),
	m_what(what_arg)
{
	// retrieve the python error message	
	PyObject *ptype, *pvalue, *ptraceback;
	PyErr_Fetch(&ptype, &pvalue, &ptraceback);

	// if there is a python message, append it to what argument
	if(pvalue) {
		PyObject* str_exc_type = PyObject_Repr(pvalue);
		PyObject* pyMessage = PyUnicode_AsEncodedString(str_exc_type, "utf-8", "Error ~");

		m_what += "\n" + std::string(PyBytes_AS_STRING(pyMessage));

		// restore the python error
		PyErr_Restore(ptype, pvalue, ptraceback);

		Py_XDECREF(str_exc_type);
		Py_XDECREF(pyMessage);
	}
}

const char* PythonException::what() const noexcept {
	return m_what.c_str();
}
