#include <iostream>
#include <aifactory.hpp>

#include <QCommandLineOption>
#include <QCommandLineParser>

#include "loader.hpp"
#include "fuzzycontroller.hpp"

std::shared_ptr<PluginInfo> pluginInfo() {
	auto info = std::make_shared<PluginInfo>();
	info->name = "FuzzyController";
	return info;
}

void init(Game&, const PluginInfo& info, const QStringList& args) {
	QCommandLineParser parser;
	QCommandLineOption pathOption(QStringList() << "p" << "controller-path", QCoreApplication::translate("main", "Path to the controller file (if any)."), "file", "controller.fis");
	parser.addOption(pathOption);
	parser.parse(args);

	PathResolver resolver(QStringList("") << QString(info.path.c_str()));
	std::string controllerPath = resolver.resolve(parser.value(pathOption)).toStdString();

	AIFactory& factory = AIFactory::instance();
	factory.add("fuzzy",
		[controllerPath](Car& car) {
			return new FuzzyController(car, controllerPath);
		}
	);
}
