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

#include "aboutdlg.hpp"
#include "../common/config.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>

AboutDlg::AboutDlg(QWidget * parent)
: QDialog(parent)
{
    setWindowTitle(tr("About Dust Racing"));
    initWidgets();
}

void AboutDlg::initWidgets()
{
    QVBoxLayout * vLayout     = new QVBoxLayout(this);
    QLabel      * pixmapLabel = new QLabel(this);

    pixmapLabel->setPixmap(QPixmap(":/about.png"));
    vLayout->addWidget(pixmapLabel);

    QLabel * infoLabel = new QLabel(this);
    infoLabel->setText(
        QString("<h2>") + Config::Game::GAME_NAME + " v" + Config::Game::GAME_VERSION + "</h2>"
        + "<p>" + Config::Game::GAME_NAME + " is licenced under GNU GPLv3.</p>"
        + "<p>Copyright (c) Jussi Lind 2012.</p>"
        + "<a href='http://dustrac.sourceforge.net'>"
        + "http://dustrac.sourceforge.net</a>");

    vLayout->addWidget(infoLabel);

    QHBoxLayout * buttonLayout = new QHBoxLayout();
    QPushButton * button = new QPushButton("&Ok", this);
    connect(button, SIGNAL(clicked()), this, SLOT(accept()));
    buttonLayout->addWidget(button);
    buttonLayout->insertStretch(0);
    vLayout->addLayout(buttonLayout);
}
