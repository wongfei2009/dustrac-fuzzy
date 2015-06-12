#include <iostream>
#include <aifactory.hpp>

#include "loader.hpp"
#include "fuzzycontroller.hpp"

void init(Game&) {
	AIFactory& factory = AIFactory::instance();
	factory.add("fuzzy",
		[](Car& car) {
			return new FuzzyController(car, Settings::instance().getControllerPath().toStdString());
		}
	);
}
