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

#ifndef AIFACTORY_HPP
#define AIFACTORY_HPP

#include "config.hpp"
#include "pidcontroller.hpp"
#include <map>
#include <functional>
#include <string>

class DUST_API AIFactory {
private:
	AIFactory();

public:
	DUST_API static AIFactory& instance();

public:
	//! Adds the creation function to the factory under the specified name.
	//! If an entry with the same name exists already, it is replaced.
	void add(const std::string& name, std::function<CarController* (Car&)> creationFunction);

	/**
	* Uses the creation function registered under the specified name
	* to return a new object.
	* \exception std::runtime_error Throws when there is no creation function
	* registered under the specified name.
	**/
	CarController* create(const std::string& name, Car& car);

	//! Removes all the registered entries.
	void clear();

private:
	std::map<std::string, std::function<CarController* (Car&)> > m_register;
};

#endif // AIFACTORY_HPP
