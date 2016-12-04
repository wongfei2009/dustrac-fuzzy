// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#include "objects.hpp"

#include <algorithm>

Objects::Objects()
{
}

void Objects::add(ObjectPtr object)
{
    auto i = std::find(m_objects.begin(), m_objects.end(), object);
    if (i == m_objects.end())
    {
        m_objects.push_back(object);
    }
}

void Objects::remove(ObjectBase & object)
{
    for (auto i = m_objects.begin(); i != m_objects.end(); i++)
    {
        if ((*i).get() == &object)
        {
            m_objects.erase(i);
            return;
        }
    }
}

unsigned int Objects::count() const
{
    return static_cast<int>(m_objects.size());
}

ObjectPtr Objects::object(unsigned int index) const
{
    return m_objects.at(index);
}

Objects::ObjectVector::iterator Objects::begin()
{
    return m_objects.begin();
}

Objects::ObjectVector::iterator Objects::end()
{
    return m_objects.end();
}

Objects::ObjectVector::const_iterator Objects::cbegin() const
{
    return m_objects.cbegin();
}

Objects::ObjectVector::const_iterator Objects::cend() const
{
    return m_objects.cend();
}

