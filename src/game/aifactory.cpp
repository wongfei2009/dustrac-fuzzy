// This file is part of Dust Racing 2D.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#include "aifactory.hpp"
#include "game.hpp"
#include "usercontroller.hpp"

AIFactory::AIFactory()
{
	add("user", [this](Car& car){return new UserController(car, Game::instance().inputHandler(), 0);});
	add("user1", [this](Car& car){return new UserController(car, Game::instance().inputHandler(), 0);});
	add("user2", [this](Car& car){return new UserController(car, Game::instance().inputHandler(), 1);});
	add("userpid", [](Car& car){return new PIDController(car, false);});
	add("pid", [](Car& car){return new PIDController(car, true);});
}

AIFactory& AIFactory::instance()
{
	static AIFactory aifactory;
	return aifactory;
}

void AIFactory::add(const std::string& name, std::function<CarController* (Car&)> creationFunction) {
	m_register[name] = creationFunction;
}

CarController* AIFactory::create(const std::string& name, Car& car) {
	auto iter = m_register.find(name);
	if(iter == m_register.end()) throw std::runtime_error("No creation function registered under name '" + name + "'.");
	return iter->second(car);
}

void AIFactory::clear() {
	m_register.clear();
}

