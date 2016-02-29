#include <iostream>
#include <aifactory.hpp>
#include <listenerbank.hpp>

#include <Python.h>

#include <QCommandLineOption>
#include <QCommandLineParser>

#include "loader.hpp"
#include "pythoncontroller.hpp"
#include "pylistener.hpp"
#include "pythonexception.hpp"

std::shared_ptr<PluginInfo> pluginInfo() {
	auto info = std::make_shared<PluginInfo>();
	info->name = "PythonController";
	return info;
}

void init(Game&, const PluginInfo& info, const QStringList& args) {
	QCommandLineParser parser;
	QCommandLineOption pathOption(QStringList() << "p" << "controller-path", QCoreApplication::translate("main", "Path to the controller file (if any)."), "file", "controller");
	parser.addOption(pathOption);
	QCommandLineOption methodOption(QStringList() << "m" << "method", QCoreApplication::translate("main", "Name of the method used to create the controller object."), "file", "Controller");
	parser.addOption(methodOption);
	QCommandLineOption listenerOption(QStringList() << "l" << "listener", QCoreApplication::translate("main", "Name of the method used to create the listener."), "file");
	parser.addOption(listenerOption);
	QCommandLineOption listenerPathOption(QStringList() << "t" << "listener-path", QCoreApplication::translate("main", "Path to the listener file (if any). If not specified, falls back to looking for the listener in the controller file."), "file", "");
	parser.addOption(listenerPathOption);
	parser.parse(args);

	std::string controllerPath = parser.value(pathOption).toStdString();
	std::string method = parser.value(methodOption).toStdString();
	std::string listener = parser.value(listenerOption).toStdString();
	std::string listenerPath = parser.value(listenerPathOption).toStdString();

    // Initialize the Python Interpreter
    Py_Initialize();
	// Pass in the CLI arguments
	PySys_SetArgvEx(0, 0, 0);

	PyObject* sys = PyImport_ImportModule("sys");
	PyObject* sys_path = PyObject_GetAttrString(sys, "path");
	PyObject* folder_path = PyUnicode_FromString(info.path.c_str());
	PyList_Append(sys_path, folder_path);
	folder_path = PyUnicode_FromString(".");
	PyList_Append(sys_path, folder_path);

	PyObject* bindings = PyImport_ImportModule("bindings");
	if(!bindings) throw PythonException("Failed loading python module 'bindings'.");

	PyObject* pModule =  PyImport_ImportModule(controllerPath.c_str());
	if(!pModule) throw PythonException("Failed loading python module '" + controllerPath + "'.");
	// pModuleDict is a borrowed reference
	PyObject* pModuleDict = PyModule_GetDict(pModule);

    // controllerFunc and dataFunc are also borrowed references
    PyObject* controllerFunc = PyDict_GetItemString(pModuleDict, method.c_str());
	if(!pModule) throw PythonException("Function '" + method + "' not found in the Python module.");

	PyDataMakerPtr dataMaker(new PyDataMaker(bindings));

	AIFactory& factory = AIFactory::instance();
	factory.add("python",
		[controllerFunc, dataMaker](Car& car) {
			return new PythonController(car, controllerFunc, dataMaker);
		}
	);

	if(listener.size()) {
		PyObject* listenerDict = pModuleDict;

		if(listenerPath.size()) {
			PyObject* pListenerModule =  PyImport_ImportModule(listenerPath.c_str());
			if(!pListenerModule) throw PythonException("Failed loading python module '" + listenerPath + "'.");
			// a borrowed reference
			listenerDict = PyModule_GetDict(pListenerModule);
		}

		// borrowed reference
		PyObject* listenerFunc = PyDict_GetItemString(listenerDict, listener.c_str());
		if(!listenerFunc) throw PythonException("Function '" + listener + "' not found in the Python module.");
		// the listener should be callable
		if(!PyCallable_Check(listenerFunc)) throw PythonException("The specified listener function '" + listener + "' is not callable.");

		if(!ListenerBank::instance().size()) ListenerBank::instance().resize(1);
		ListenerBank::instance().add(0, ListenerPtr(new PyListener(listenerFunc, dataMaker)));
	}

	Py_DECREF(sys);
	Py_DECREF(sys_path);
	Py_DECREF(folder_path);
	Py_DECREF(pModule);
	Py_DECREF(bindings);
}

struct PyFinalizer {
	// Finish the Python Interpreter when this lib is unloaded.
	~PyFinalizer() {
		Py_Finalize();
	}
} pyFinalizer;

