// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef OVERLAYBASE_HPP
#define OVERLAYBASE_HPP

#include <MCBBox>
#include <MCTypes>

#include "renderable.hpp"
#include "updateableif.hpp"

//! Base class for overlays that are rendered on top of the game scene.
class OverlayBase : public UpdateableIf, public Renderable
{
public:

    //! Constructor.
    OverlayBase();

    //! Destructor.
    virtual ~OverlayBase();

    //! \reimp
    virtual bool update() override;

    //! \reimp
    virtual void reset() override;
};

#endif // OVERLAYBASE_HPP
