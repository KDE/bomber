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

#include "buildingtile.h"

#include "board.h"
#include "renderer.h"

#include <kdebug.h>

BuildingTile::BuildingTile(BomberRenderer *renderer, BomberBoard *board) :
	KGameCanvasPixmap(board), m_renderer(renderer), m_board(board), m_tileSize(
			32, 64)
{
}

BuildingTile::~BuildingTile()
{
}

/**
 * Sets width and height of plane.
 */
void BuildingTile::setSize(const QSize& tileSize)
{
	m_tileSize = tileSize;
}

/**
 * Updates plane position and pixmap.
 * This method is called once per frame.
 */
void BuildingTile::update()
{
	setFrame(0);
}

/**
 * Performs move calculations
 * This method is called once per frame
 */
void BuildingTile::advanceItem()
{
	// Building never moves, so this does nothing
}

/**
 * Sets plane's current frame
 */
void BuildingTile::setFrame(unsigned int)
{
	setPixmap(m_renderer->renderElement(m_pixmapName, m_tileSize));
}

/**
 * Used to set the name of the pixmap used to display the tile
 * \param pixmap the name of the pixmap
 */
void BuildingTile::setPixmapName(const QString& pixmap)
{
	m_pixmapName = pixmap;
}
