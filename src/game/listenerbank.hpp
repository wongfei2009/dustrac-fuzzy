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

#ifndef LISTENERBANK_HPP
#define LISTENERBANK_HPP

#include "listener.hpp"
#include <vector>
#include <set>

/**
* A central place where we can register listeners
* for the individual cars.
**/
class ListenerBank {
private:
	ListenerBank() = default;

public:
	static ListenerBank& instance();

public:
	//! Adds a listener for car carNum.
	void add(unsigned int carNum, const ListenerPtr& listener);

	//! Removes a listener from car carNum.
	void remove(unsigned int carNum, const ListenerPtr& listener);

	//! Removes all listeners from car carNum.
	void clear(unsigned int carNum);

	//! Removes all listeners from all cars.
	void clear();

	//! Returs the set of listeners corresponding to carNum.
	const std::set<ListenerPtr>& getListeners(unsigned int carNum) const;

	//! The number of cars in the listener bank.
	unsigned int size() const {
		return static_cast<unsigned int>(m_register.size());
	}

	//! Specifies the number of cars there are.
	void resize(unsigned int numCars) {
		m_register.resize(numCars);
	}

private:
	//! The lists of listeners.
	std::vector<std::set<ListenerPtr> > m_register;
	//! A dummy empty set of listeners to return for
	//! cars that are not registered at all.
	const std::set<ListenerPtr> m_dummySet;
};

#endif // LISTENERBANK_HPP
