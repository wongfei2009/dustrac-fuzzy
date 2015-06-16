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

#ifndef FUZZYCONTROLLER_HPP
#define FUZZYCONTROLLER_HPP

#include <pidcontroller.hpp>
#include <fl/Headers.h>
#include <string>
#include <fstream>

//! A controller based on fuzzylite.
class FuzzyController: public PIDController {
public:
	FuzzyController(Car& car, fl::Engine* engine);
	FuzzyController(Car& car, const std::string& fis_filename);
	FuzzyController(Car& car, std::istream& fis_file);
	virtual ~FuzzyController() = default;

private:
	static fl::Engine* loadFis(std::istream& fis_file);
	static fl::Engine* loadFis(const std::string& fis_filename);

protected:
    //! Steering logic.
    virtual float steerControl(bool isRaceCompleted);
    //! Brake/accelerate logic.
	virtual float speedControl(bool isRaceCompleted);

protected:
    int           m_lastDiff = 0;
    fl::Engine* m_fis;
};

#endif // FUZZYCONTROLLER_HPP
