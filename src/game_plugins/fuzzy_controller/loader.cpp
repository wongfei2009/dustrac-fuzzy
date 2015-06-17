#include <iostream>
#include <aifactory.hpp>

#include <QCommandLineOption>
#include <QCommandLineParser>

#include "loader.hpp"
#include "fuzzycontroller.hpp"

void init(Game&, int argc, char ** argv) {
	QStringList args;
	args << "plugin";
	for(int i = 0; i < argc; i++) {
		args << argv[i];
	}

	QCommandLineParser parser;
	QCommandLineOption pathOption(QStringList() << "p" << "controller-path", QCoreApplication::translate("main", "Path to the controller file (if any)."), "file", "data/controller.fis");
	parser.addOption(pathOption);
	parser.parse(args);

	std::string controllerPath = parser.value(pathOption).toStdString();

	AIFactory& factory = AIFactory::instance();
	factory.add("fuzzy",
		[controllerPath](Car& car) {
			return new FuzzyController(car, controllerPath);
		}
	);
}
