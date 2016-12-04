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

#include "menuitem.hpp"
#include "menuitemaction.hpp"
#include "menu.hpp"
#include "menumanager.hpp"

namespace MTFH {

MenuItem::MenuItem(int width, int height, std::wstring text, bool selectable)
: m_text(text)
, m_menuOpenActionMenuId("")
, m_action(nullptr)
, m_actionFunction(nullptr)
, m_view(nullptr)
, m_focused(false)
, m_selected(false)
, m_selectable(selectable)
, m_width(width)
, m_height(height)
, m_x(0)
, m_y(0)
, m_lMargin(1)
, m_rMargin(1)
, m_tMargin(1)
, m_bMargin(1)
, m_index(-1)
{
}

int MenuItem::width() const
{
    return m_width + m_lMargin + m_rMargin;
}

int MenuItem::height() const
{
    return m_height + m_tMargin + m_bMargin;
}

void MenuItem::setIndex(int index)
{
    m_index = index;
}

int MenuItem::index() const
{
    return m_index;
}

void MenuItem::setPos(int x, int y)
{
    m_x = x;
    m_y = y;
}

int MenuItem::x() const
{
    return m_x;
}

int MenuItem::y() const
{
    return m_y;
}

void MenuItem::setView(MenuItemViewPtr view)
{
    m_view = view;
}

MenuItemViewPtr MenuItem::view()
{
    return m_view;
}

void MenuItem::setAction(MenuItemActionPtr action)
{
    m_action = action;
}

void MenuItem::setAction(ActionFunction actionFunction)
{
    m_actionFunction = actionFunction;
}

void MenuItem::setMenuOpenAction(const std::string & menuId)
{
    m_menuOpenActionMenuId = menuId;
}

MenuItemActionPtr MenuItem::action() const
{
    return m_action;
}

void MenuItem::onLeft()
{
}

void MenuItem::onRight()
{
}

void MenuItem::onUp()
{
}

void MenuItem::onDown()
{
}

void MenuItem::setSelected(bool flag)
{
    if (flag)
    {
        if (m_action)
        {
            m_action->fire();
        }

        if (m_actionFunction)
        {
            m_actionFunction();
        }

        if (!m_menuOpenActionMenuId.empty())
        {
            MenuManager::instance().pushMenu(m_menuOpenActionMenuId);
        }
    }

    m_selected = flag;
}

void MenuItem::setFocused(bool focused)
{
    m_focused = focused;
}

void MenuItem::setContentsMargins(int left, int right, int top, int bottom)
{
    m_lMargin = left;
    m_rMargin = right;
    m_tMargin = top;
    m_bMargin = bottom;
}

void MenuItem::getContentsMargins(int & left, int & right, int & top, int & bottom) const
{
    left   = m_lMargin;
    right  = m_rMargin;
    top    = m_tMargin;
    bottom = m_bMargin;
}

bool MenuItem::focused() const
{
    return m_focused;
}

bool MenuItem::selected() const
{
    return m_selected && m_selectable;
}

bool MenuItem::selectable() const
{
    return m_selectable;
}

void MenuItem::render()
{
    if (m_view)
    {
        m_view->render(m_x, m_y);
    }
}

const std::wstring & MenuItem::text() const
{
    return m_text;
}

MenuItem::~MenuItem()
{
}

} // namespace MTFH

