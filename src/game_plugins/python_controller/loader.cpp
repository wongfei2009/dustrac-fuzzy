#include <iostream>
#include <aifactory.hpp>
#include <listenerbank.hpp>

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <MCException>
#include <Python.h>

#include "loader.hpp"
#include "pythoncontroller.hpp"
#include "pylistener.hpp"

void init(Game&, int argc, char ** argv) {
	QStringList args;
	args << "plugin";
	for(int i = 0; i < argc; i++) {
		args << argv[i];
	}

	QCommandLineParser parser;
	QCommandLineOption pathOption(QStringList() << "p" << "controller-path", QCoreApplication::translate("main", "Path to the controller file (if any)."), "file", "controller");
	parser.addOption(pathOption);
	QCommandLineOption methodOption(QStringList() << "m" << "method", QCoreApplication::translate("main", "Name of the method used to create the controller object."), "file", "Controller");
	parser.addOption(methodOption);
	QCommandLineOption dataOption(QStringList() << "d" << "data", QCoreApplication::translate("main", "Name of the method used to create the data object."), "file", "PIDData");
	parser.addOption(dataOption);
	QCommandLineOption listenerOption(QStringList() << "l" << "listener", QCoreApplication::translate("main", "Name of the method used to create the listener."), "file", "Listener");
	parser.addOption(listenerOption);
	parser.parse(args);

	std::string controllerPath = parser.value(pathOption).toStdString();
	std::string method = parser.value(methodOption).toStdString();
	std::string data = parser.value(dataOption).toStdString();
	std::string listener = parser.value(listenerOption).toStdString();

    // Initialize the Python Interpreter
    Py_Initialize();

	PyObject* sys = PyImport_ImportModule("sys");
	PyObject* sys_path = PyObject_GetAttrString(sys, "path");
	PyObject* folder_path = PyUnicode_FromString(DATA_PATH);
	PyList_Append(sys_path, folder_path);

	PyObject* pModule =  PyImport_ImportModule(controllerPath.c_str());
	if(!pModule) throw MCException("Failed loading python module '" + controllerPath + "'.");

	// pDict is a borrowed reference
	PyObject* pDict = PyModule_GetDict(pModule);
    // controllerFunc and dataFunc are also borrowed references
    PyObject* controllerFunc = PyDict_GetItemString(pDict, method.c_str());
	if(!controllerFunc) throw MCException("Function '" + method + "' not found in the Python module.");
	PyObject* dataFunc = PyDict_GetItemString(pDict, data.c_str());
	if(!dataFunc) throw MCException("Function '" + data + "' not found in the Python module.");

	PyDataMakerPtr dataMaker(new PyDataMaker(dataFunc));

	AIFactory& factory = AIFactory::instance();
	factory.add("python",
		[controllerFunc, dataMaker](Car& car) {
			return new PythonController(car, controllerFunc, dataMaker);
		}
	);

	// borrowed reference
	PyObject* listenerFunc = PyDict_GetItemString(pDict, listener.c_str());
	if(listenerFunc) {
		// the listener is optional, but if provided it should be a callable
		if(!PyCallable_Check(listenerFunc)) throw MCException("The specified listener function '" + listener + "' is not a callable.");

		if(!ListenerBank::instance().size()) ListenerBank::instance().resize(1);
		ListenerBank::instance().add(0, ListenerPtr(new PyListener(listenerFunc, dataMaker)));
	}

	Py_DECREF(sys);
	Py_DECREF(sys_path);
	Py_DECREF(folder_path);
	Py_DECREF(pModule);

//    // Finish the Python Interpreter
//    Py_Finalize();
}
