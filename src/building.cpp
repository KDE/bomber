/*
 * Copyright (C) 2007-2008 John-Paul Stanford <jp@stanwood.org.uk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "building.h"

#include <kdebug.h>
#include <KRandom>

#include "board.h"
#include "renderer.h"
#include "buildingtile.h"

#define BUILDING_RELATIVE_WIDTH 1.0
#define BUILDING_RELATIVE_HEIGHT 1.0
#define MAX_STYLE 1

const int Building::BUILD_BASE_LOCATION = 16;

Building::Building(BomberRenderer* renderer, BomberBoard* board, int position, int height) :
    m_renderer(renderer), m_board(board)
{
    m_height = 0;
    m_xPos = position;
    setHeight(height);
    setupBuildingTiles();
}

Building::~Building()
{
    qDeleteAll(m_buildingTiles);
}

void Building::setHeight(int height)
{
    if (height<0)
    {
        height = 0;
    }

    if (height < m_height)
    {
        // Building getting smaller so remove the top
        for (int i=1; i<=m_height-height; i++)
        {
            m_buildingTiles.at(m_height-i)->hide();
            m_buildingTiles.removeAt(m_height-i);
        }
    }

    m_height = height;
    m_boundingRect.moveTo(m_xPos, BUILD_BASE_LOCATION-m_height+1);
    m_boundingRect.setSize(QSizeF(BUILDING_RELATIVE_WIDTH, BUILDING_RELATIVE_HEIGHT * height) );
}

void Building::setupBuildingTiles()
{
    int style = KRandom::random() % (m_renderer->maxBuildingStyles());    
    int maxVarient = m_renderer->frames(QString("building_")+QString::number(style));
    
    for (int heightIndex = 0; heightIndex < m_height-1; heightIndex++)
    {
        int varient = KRandom::random() % (maxVarient);
        QString pixmap;
        pixmap.sprintf("building_%d_%d", style, varient);
        m_buildingTiles.append(createBuildingTile(pixmap, heightIndex));
    }

    int varient = 0;
    QString pixmap;
    pixmap.sprintf("roof_%d_%d", style, varient);
    m_buildingTiles.append(createBuildingTile(pixmap, m_height));
    m_boundingRect.moveTo(m_xPos, BUILD_BASE_LOCATION-m_height+1);
}

BuildingTile* Building::createBuildingTile(QString pixmap, int)
{
    BuildingTile *tile = new BuildingTile(m_renderer,m_board);
    tile->setPixmapName(pixmap);
    return tile;
}

void Building::raise()
{
    foreach(BuildingTile *tile, m_buildingTiles)
    {
        tile->raise();
    }
}

void Building::show()
{
    foreach(BuildingTile *tile, m_buildingTiles)
    {
        tile->show();
    }
}

void Building::update()
{
    foreach(BuildingTile *tile, m_buildingTiles)
    {
        tile->update();
    }
}

void Building::advanceItem()
{
    foreach(BuildingTile *tile, m_buildingTiles)
    {
        tile->advanceItem();
    }
}

void Building::resize(const QSize& size)
{
    QSize tileSize(static_cast<int>(BUILDING_RELATIVE_WIDTH * size.width() ), static_cast<int> (BUILDING_RELATIVE_HEIGHT * size.height() ));
    for (int i=0; i< m_buildingTiles.size(); i++)
    {
        BuildingTile *tile = m_buildingTiles.at(i);
        tile->setSize(tileSize);
        tile->moveTo(m_board->mapPosition(QPointF(m_xPos, BUILD_BASE_LOCATION-i) ) );
        tile->setFrame(0);
    }
}

QRectF Building::boundingRect()
{
    return m_boundingRect;
}

void Building::destoryTop()
{
    setHeight(m_height -1);
}

QPointF Building::position()
{
    return QPointF(m_xPos, BUILD_BASE_LOCATION);
}
