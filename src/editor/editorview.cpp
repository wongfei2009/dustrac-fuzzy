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

#include <QApplication>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QStatusBar>
#include <QString>

#ifdef USE_QT5
#include <QTransform>
#endif

#include "editordata.hpp"
#include "editorview.hpp"
#include "mainwindow.hpp"
#include "object.hpp"
#include "objectfactory.hpp"
#include "objectmodelloader.hpp"
#include "rotatedialog.hpp"
#include "rotatetileundostackitem.hpp"
#include "targetnode.hpp"
#include "targetnodesizedlg.hpp"
#include "tiletypeundostackitem.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"

EditorView::EditorView(EditorData & editorData, QWidget * parent)
: QGraphicsView(parent)
, m_clearComputerHint(nullptr)
, m_setComputerHintBrakeHard(nullptr)
, m_setComputerHintBrake(nullptr)
, m_editorData(editorData)
{
    createTileContextMenu();
    createObjectContextMenu();
    createTargetNodeContextMenu();
}

void EditorView::mouseMoveEvent(QMouseEvent * event)
{
    if (scene())
    {
        const QPointF mappedPos = mapToScene(event->pos());

#ifdef USE_QT5
        QTransform dummy;
        if (TrackTile * tile =
            dynamic_cast<TrackTile *>(scene()->itemAt(mappedPos, dummy)))
        {
            tile->setActive(true);
        }
#else
        if (TrackTile * tile =
            dynamic_cast<TrackTile *>(scene()->itemAt(mappedPos)))
        {
            tile->setActive(true);
        }
#endif
        // Tile drag'n'drop active?
        if (TrackTile * sourceTile = m_editorData.dragAndDropSourceTile())
        {
            sourceTile->setPos(mappedPos);
        }
        // Object drag'n'drop active?
        else if (Object * object = m_editorData.dragAndDropObject())
        {
            object->setLocation(mappedPos);
        }
        // Target node drag'n'drop active?
        else if (TargetNode * tnode = m_editorData.dragAndDropTargetNode())
        {
            tnode->setLocation(mappedPos);
        }

        // Show coordinates in status bar
        QString coordinates("X: %1 Y: %2 I: %3 J: %4");
        coordinates = coordinates.arg(
            mappedPos.x()).arg(
            mappedPos.y()).arg(
            static_cast<int>(mappedPos.x() / TrackTile::TILE_W)).arg(
            static_cast<int>(mappedPos.y() / TrackTile::TILE_H));
        MainWindow::instance()->statusBar()->showMessage(coordinates);
    }
}

void EditorView::createTileContextMenu()
{
    const QChar degreeSign(176);
    const QString dummy1(QString(QWidget::tr("Rotate 90")) +
        degreeSign + QWidget::tr(" CW.."));

    QAction * rotate90CW = new QAction(dummy1, &m_tileContextMenu);
    QObject::connect(rotate90CW, SIGNAL(triggered()), this, SLOT(doRotateTile90CW()));

    const QString dummy2(QString(QWidget::tr("Rotate 90")) +
        degreeSign + QWidget::tr(" CCW.."));

    QAction * rotate90CCW = new QAction(dummy2, &m_tileContextMenu);
    QObject::connect(rotate90CCW, SIGNAL(triggered()), this, SLOT(doRotateTile90CCW()));

    m_clearComputerHint = new QAction(QWidget::tr("Clear computer hint"), &m_tileContextMenu);
    QObject::connect(m_clearComputerHint, SIGNAL(triggered()), this, SLOT(doClearComputerHint()));

    m_setComputerHintBrakeHard = new QAction(
        QWidget::tr("Set computer hint 'brake hard'.."), &m_tileContextMenu);
    QObject::connect(m_setComputerHintBrakeHard, SIGNAL(triggered()), this,
        SLOT(doSetComputerHintBrakeHard()));

    m_setComputerHintBrake = new QAction(
        QWidget::tr("Set computer hint 'brake'.."), &m_tileContextMenu);
    QObject::connect(m_setComputerHintBrake, SIGNAL(triggered()), this,
        SLOT(doSetComputerHintBrake()));

    // Populate the menu
    m_tileContextMenu.addAction(rotate90CW);
    m_tileContextMenu.addAction(rotate90CCW);
    m_tileContextMenu.addSeparator();
    m_tileContextMenu.addAction(m_clearComputerHint);
    m_tileContextMenu.addAction(m_setComputerHintBrakeHard);
    m_tileContextMenu.addAction(m_setComputerHintBrake);
}

void EditorView::createObjectContextMenu()
{
    const QString dummy1(QString(QWidget::tr("Rotate..")));
    QAction * rotate = new QAction(dummy1, &m_tileContextMenu);
    QObject::connect(rotate, SIGNAL(triggered()), this, SLOT(doRotateObject()));

    // Populate the menu
    m_objectContextMenu.addAction(rotate);
}

void EditorView::createTargetNodeContextMenu()
{
    const QString dummy1(QString(QWidget::tr("Set size..")));
    QAction * setSize = new QAction(dummy1, &m_targetNodeContextMenu);
    QObject::connect(setSize, SIGNAL(triggered()), this, SLOT(doSetTargetNodeSize()));

    // Populate the menu
    m_targetNodeContextMenu.addAction(setSize);
}

void EditorView::mousePressEvent(QMouseEvent * event)
{
    if (scene())
    {
        m_clickedPos      = event->pos();
        m_clickedScenePos = mapToScene(m_clickedPos);

        // User is erasing an object
        if (m_editorData.mode() == EditorData::EM_ERASE_OBJECT)
        {
            eraseObjectAtCurrentClickedPos();
        }
        // User is adding an object
        else if (m_editorData.mode() == EditorData::EM_ADD_OBJECT)
        {
            addCurrentToolBarObjectToScene();
        }
        // Default actions
        else
        {
            // Fetch all items at the location
            QList<QGraphicsItem *> items = scene()->items(
                m_clickedScenePos, Qt::IntersectsItemShape, Qt::DescendingOrder);

            if (items.size())
            {
                // Check if there's an object at the position and handle that.
                // Otherwise it would be difficult to select objects that are surrounded
                // by a target node area.
                auto iter = items.begin();
                while (iter != items.end())
                {
                    QGraphicsItem * item = *iter;
                    if (Object * object = dynamic_cast<Object *>(item))
                    {
                        handleMousePressEventOnObject(*event, *object);
                        return;
                    }

                    iter++;
                }

                QGraphicsItem * item = *items.begin();
                if (TargetNode * tnode = dynamic_cast<TargetNode *>(item))
                {
                    handleMousePressEventOnTargetNode(*event, *tnode);
                }
                else if (TrackTile * tile = dynamic_cast<TrackTile *>(item))
                {
                    handleMousePressEventOnTile(*event, *tile);
                }
            }
        }
    }
}

void EditorView::eraseObjectAtCurrentClickedPos()
{
    // Fetch all items at the location
    QList<QGraphicsItem *> items = scene()->items(
        m_clickedScenePos, Qt::IntersectsItemShape, Qt::DescendingOrder);

    // We need to find the first object to be erased, because
    // there might be also overlapping TargetNodes.
    for (QGraphicsItem * item : items)
    {
        if (Object * object = dynamic_cast<Object *>(item))
        {
            m_editorData.trackData()->objects().remove(*object);
            m_editorData.setSelectedObject(nullptr);
            scene()->removeItem(object);
            delete object;
            break;
        }
    }
}

void EditorView::addCurrentToolBarObjectToScene()
{
    if (QAction * action = MainWindow::instance()->currentToolBarAction())
    {
        if (scene())
        {
            Object & object = ObjectFactory::createObject(action->data().toString());
            object.setLocation(m_clickedScenePos);

            if (MainWindow::instance()->randomlyRotateObjects())
            {
                object.setRotation(std::rand() % 360);
            }

            scene()->addItem(&object);

            m_editorData.trackData()->objects().add(object);
            m_editorData.setSelectedObject(&object);
        }
    }
}

void EditorView::handleMousePressEventOnObject(QMouseEvent & event, Object & object)
{
    if (event.button() == Qt::RightButton)
    {
        handleRightButtonClickOnObject(object);
    }
    else if (event.button() == Qt::LeftButton)
    {
        handleLeftButtonClickOnObject(object);
    }

    QWidget::mousePressEvent(&event);
}

void EditorView::handleMousePressEventOnTargetNode(QMouseEvent & event, TargetNode & tnode)
{
    if (event.button() == Qt::RightButton)
    {
        handleRightButtonClickOnTargetNode(tnode);
    }
    else if (event.button() == Qt::LeftButton)
    {
        handleLeftButtonClickOnTargetNode(tnode);
    }

    QWidget::mousePressEvent(&event);
}

void EditorView::handleMousePressEventOnTile(QMouseEvent & event, TrackTile & tile)
{
    tile.setActive(true);

    if (event.button() == Qt::RightButton)
    {
        handleRightButtonClickOnTile(tile);
    }
    else if (event.button() == Qt::LeftButton)
    {
        handleLeftButtonClickOnTile(tile);
    }

    QWidget::mousePressEvent(&event);
}

void EditorView::handleLeftButtonClickOnObject(Object & object)
{
    // User is initiating a drag'n'drop
    if (m_editorData.mode() == EditorData::EM_NONE)
    {
        object.setZValue(object.zValue() + 1);
        m_editorData.setDragAndDropObject(&object);
        m_editorData.setSelectedObject(&object);

        // Change cursor to the closed hand cursor.
        QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
    }
}

void EditorView::handleLeftButtonClickOnTargetNode(TargetNode & tnode)
{
    // User is initiating a drag'n'drop
    if (m_editorData.mode() == EditorData::EM_NONE)
    {
        tnode.setZValue(tnode.zValue() + 1);
        m_editorData.setDragAndDropTargetNode(&tnode);

        // Change cursor to the closed hand cursor.
        QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
    }
    // It's not possible to make nodes overlap if not handled also here.
    else if (m_editorData.mode() == EditorData::EM_SET_ROUTE)
    {
        m_editorData.pushNewTargetNodeToRoute(m_clickedScenePos);
    }
}

void EditorView::handleLeftButtonClickOnTile(TrackTile & tile)
{
    // User is defining the route
    if (m_editorData.mode() == EditorData::EM_SET_ROUTE)
    {
        m_editorData.pushNewTargetNodeToRoute(m_clickedScenePos);
    }
    // User is setting the tile type
    else if (m_editorData.mode() == EditorData::EM_SET_TILE_TYPE)
    {
        if (QAction * action = MainWindow::instance()->currentToolBarAction())
        {
            Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();

            if (modifiers & Qt::ControlModifier)
            {
                QString typeToFill = tile.tileType();

                if (typeToFill != action->data().toString())
                {
                    doFloodFill(tile, action, typeToFill);
                }
            }
            else
            {
                changeTileType(tile, action);
            }
        }
    }
    // User is initiating a drag'n'drop
    else if (m_editorData.mode() == EditorData::EM_NONE)
    {
        tile.setZValue(tile.zValue() + 1);
        m_editorData.setDragAndDropSourceTile(&tile);
        m_editorData.setDragAndDropSourcePos(tile.pos());

        // Change cursor to the closed hand cursor.
        QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
    }
}

void EditorView::handleRightButtonClickOnTile(TrackTile & tile)
{
    // Enable all hints by default
    m_clearComputerHint->setEnabled(true);
    m_setComputerHintBrakeHard->setEnabled(true);
    m_setComputerHintBrake->setEnabled(true);

    switch (tile.computerHint())
    {
    case TrackTileBase::CH_NONE:
        m_clearComputerHint->setEnabled(false);
        break;
    case TrackTileBase::CH_BRAKE_HARD:
        m_setComputerHintBrakeHard->setEnabled(false);
        break;
    case TrackTileBase::CH_BRAKE:
        m_setComputerHintBrake->setEnabled(false);
        break;
    default:
        break;
    }

    // Show the context menu
    QPoint globalPos = mapToGlobal(m_clickedPos);
    m_tileContextMenu.exec(globalPos);
}

void EditorView::handleRightButtonClickOnObject(Object & object)
{
    m_editorData.setSelectedObject(&object);

    // Show the context menu
    QPoint globalPos = mapToGlobal(m_clickedPos);
    m_objectContextMenu.exec(globalPos);
}

void EditorView::handleRightButtonClickOnTargetNode(TargetNode & tnode)
{
    m_editorData.setSelectedTargetNode(&tnode);

    // Show the context menu
    QPoint globalPos = mapToGlobal(m_clickedPos);
    m_targetNodeContextMenu.exec(globalPos);
}

void EditorView::mouseReleaseEvent(QMouseEvent * event)
{
    handleTileDragRelease(event);
    handleObjectDragRelease(event);
    handleTargetNodeDragRelease(event);
}

void EditorView::keyPressEvent(QKeyEvent * event)
{
    if (Object * object = m_editorData.selectedObject())
    {
        if (!event->isAutoRepeat())
        {
            switch (event->key())
            {
            case Qt::Key_Left:
                object->setLocation(QPointF(object->location().x() - 1, object->location().y()));
                break;
            case Qt::Key_Right:
                object->setLocation(QPointF(object->location().x() + 1, object->location().y()));
                break;
            case Qt::Key_Up:
                object->setLocation(QPointF(object->location().x(), object->location().y() - 1));
                break;
            case Qt::Key_Down:
                object->setLocation(QPointF(object->location().x(), object->location().y() + 1));
                break;
            default:
                break;
            }
        }
    }
}

void EditorView::handleTileDragRelease(QMouseEvent * event)
{
    if (scene())
    {
        // Tile drag'n'drop active?
        if (TrackTile * sourceTile = m_editorData.dragAndDropSourceTile())
        {
            // Determine the dest tile
            TrackTile * destTile = sourceTile;
            QList<QGraphicsItem *> items = scene()->items(mapToScene(event->pos()));
            for (QGraphicsItem * item : items)
            {
                TrackTile * testTile = dynamic_cast<TrackTile *>(item);
                if (testTile && testTile != sourceTile)
                {
                    destTile = testTile;
                    break;
                }
            }

            // Swap tiles
            sourceTile->swap(*destTile);

            // Restore position
            sourceTile->setPos(m_editorData.dragAndDropSourcePos());
            sourceTile->setZValue(sourceTile->zValue() - 1);
            destTile->setZValue(sourceTile->zValue());

            update();

            m_editorData.setDragAndDropSourceTile(nullptr);

            // Restore the cursor.
            QApplication::restoreOverrideCursor();
        }
    }
}

void EditorView::handleObjectDragRelease(QMouseEvent * event)
{
    if (scene())
    {
        // Object drag'n'drop active?
        if (Object * object = m_editorData.dragAndDropObject())
        {
            // Set the new position position
            object->setLocation(mapToScene(event->pos()));
            object->setZValue(object->zValue() - 1);

            update();

            m_editorData.setDragAndDropObject(nullptr);

            // Restore the cursor.
            QApplication::restoreOverrideCursor();
        }
    }
}

void EditorView::handleTargetNodeDragRelease(QMouseEvent * event)
{
    if (scene())
    {
        // Target node drag'n'drop active?
        if (TargetNode * tnode = m_editorData.dragAndDropTargetNode())
        {
            // Set the new position position
            tnode->setLocation(mapToScene(event->pos()));
            tnode->setZValue(tnode->zValue() - 1);

            update();

            m_editorData.setDragAndDropTargetNode(nullptr);

            // Restore the cursor.
            QApplication::restoreOverrideCursor();
        }
    }
}

void EditorView::doRotateTile90CW()
{
#ifdef USE_QT5
    QTransform dummy;
    if (TrackTile * tile =
        dynamic_cast<TrackTile *>(scene()->itemAt(mapToScene(m_clickedPos), dummy)))
#else
    if (TrackTile * tile =
        dynamic_cast<TrackTile *>(scene()->itemAt(mapToScene(m_clickedPos))))
#endif
    {
        qreal oldRotation = tile->rotation();
        qreal newRotation;

        if (tile->rotate90CW(&newRotation))
        {
            addRotateUndoStackItem(tile, oldRotation, newRotation);
        }
    }
}

void EditorView::doRotateTile90CCW()
{
#ifdef USE_QT5
    QTransform dummy;
    if (TrackTile * tile =
        dynamic_cast<TrackTile *>(scene()->itemAt(mapToScene(m_clickedPos), dummy)))
#else
    if (TrackTile * tile =
        dynamic_cast<TrackTile *>(scene()->itemAt(mapToScene(m_clickedPos))))
#endif
    {
        qreal oldRotation = tile->rotation();
        qreal newRotation;

        if (tile->rotate90CCW(&newRotation))
        {
            addRotateUndoStackItem(tile, oldRotation, newRotation);
        }
    }
}

void EditorView::doRotateObject()
{
    RotateDialog dialog;
    if (dialog.exec() == QDialog::Accepted)
    {
        if (Object * object = m_editorData.selectedObject())
        {
            object->setRotation(static_cast<int>(dialog.angle() + object->rotation()) % 360);
        }
    }
}

void EditorView::doSetTargetNodeSize()
{
    if (TargetNode * tnode = m_editorData.selectedTargetNode())
    {
        TargetNodeSizeDlg dialog(tnode->size());
        if (dialog.exec() == QDialog::Accepted)
        {
            tnode->setSize(dialog.targetNodeSize());
        }
    }
}

void EditorView::doClearComputerHint()
{
    doSetComputerHint(TrackTileBase::CH_NONE);
}

void EditorView::doSetComputerHintBrakeHard()
{
    doSetComputerHint(TrackTileBase::CH_BRAKE_HARD);
}

void EditorView::doSetComputerHintBrake()
{
    doSetComputerHint(TrackTileBase::CH_BRAKE);
}

void EditorView::doSetComputerHint(TrackTileBase::ComputerHint hint)
{
#ifdef USE_QT5
    QTransform dummy;
    if (TrackTile * tile =
        dynamic_cast<TrackTile *>(scene()->itemAt(mapToScene(m_clickedPos), dummy)))
    {
        tile->setComputerHint(hint);
    }
#else
    if (TrackTile * tile =
        dynamic_cast<TrackTile *>(scene()->itemAt(mapToScene(m_clickedPos))))
    {
        tile->setComputerHint(hint);
    }
#endif
}

void EditorView::doFloodFill(TrackTile & tile, QAction * action, QString typeToFill)
{
    std::vector< QPoint > positions;

    floodFill(tile, action, typeToFill, positions);

    QString newType = action->data().toString();
    UndoStackItemBase * item = new TileTypeUndoStackItem(positions, typeToFill, newType);
    m_editorData.trackData()->addItemToUndoStack(item);

    emit itemAddedToUndoStack();
}

void EditorView::floodFill(TrackTile & tile, QAction * action, const QString & typeToFill, std::vector<QPoint> & positions)
{
    static const int DIRECTION_COUNT = 4;

    // Coordinates of neighbor tiles can be calculated by adding these
    // adjustments to tile coordinates.
    static const QPoint neighborAdjustments[DIRECTION_COUNT] =
    {
        QPoint( 1,  0),  // right
        QPoint( 0, -1),  // up
        QPoint(-1,  0),  // left
        QPoint( 0,  1)   // down
    };

    setTileType(tile, action);

    positions.push_back(tile.matrixLocation());

    MapBase & map = m_editorData.trackData()->map();
    QPoint location = tile.matrixLocation();

    for (int i = 0; i < DIRECTION_COUNT; ++i)
    {
        int x = location.x() + neighborAdjustments[i].x();
        int y = location.y() + neighborAdjustments[i].y();

        if (x >= 0 && y >= 0)
        {
            TrackTile * tile = dynamic_cast<TrackTile *>(map.getTile(x, y));

            if (tile != nullptr && tile->tileType() == typeToFill)
            {
                floodFill(*tile, action, typeToFill, positions);
            }
        }
    }
}

void EditorView::changeTileType(TrackTile & tile, QAction * action)
{
    QString oldType = tile.tileType();

    setTileType(tile, action);

    QString newType = tile.tileType();

    std::vector< QPoint > positions;
    positions.push_back(tile.matrixLocation());
    UndoStackItemBase * item = new TileTypeUndoStackItem(positions, oldType, newType);
    m_editorData.trackData()->addItemToUndoStack(item);

    emit itemAddedToUndoStack();
}

void EditorView::addRotateUndoStackItem(TrackTile * tile, qreal oldRotation, qreal newRotation)
{
    QPoint pos = tile->matrixLocation();
    UndoStackItemBase * item = new RotateTileUndoStackItem(pos, oldRotation, newRotation);

    m_editorData.trackData()->addItemToUndoStack(item);

    emit itemAddedToUndoStack();
}

void EditorView::setTileType(TrackTile & tile, QAction * action)
{
    tile.setTileType(action->data().toString());
    tile.setPixmap(action->icon().pixmap(TrackTile::TILE_W, TrackTile::TILE_H));
}
