/*
    Copyright 2007 Dmitry Suzdalev <dimsuz@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "scene.h"

#include <QResizeEvent>

#include "minefielditem.h"

KMinesScene::KMinesScene( QObject* parent )
    : QGraphicsScene(parent)
{
    m_fieldItem = new MineFieldItem(10,10, 3);
    addItem(m_fieldItem);
    m_fieldItem->setCellSize(30);
    m_fieldItem->setPos(20,20);
}

void KMinesScene::resizeScene(int width, int height)
{
    setSceneRect(0, 0, width, height);
}

KMinesView::KMinesView( KMinesScene* scene, QWidget *parent )
    : QGraphicsView(scene, parent), m_scene(scene)
{
}

void KMinesView::resizeEvent( QResizeEvent *ev )
{
    m_scene->resizeScene( ev->size().width(), ev->size().height() );
}
