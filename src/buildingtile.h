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

#ifndef BUILDINGTILE_H
#define BUILDINGTILE_H

#include "kgamecanvas.h"

class BomberRenderer;
class BomberBoard;

class BuildingTile: public KGameCanvasPixmap
{
public:
	BuildingTile(BomberRenderer *renderer, BomberBoard *board);
	~BuildingTile();

	/**
	 * Sets width and height of plane.
	 */
	void setSize(const QSize& tileSize);

	/**
	 * Updates plane position and pixmap.
	 * This method is called once per frame.
	 */
	void update();

	/**
	 * Performs move calculations
	 * This method is called once per frame
	 */
	void advanceItem();

	/**
	 * Sets plane's current frame
	 * \param frame The number of the frame
	 */
	void setFrame(unsigned int frame);

	/**
	 * Used to set the name of the pixmap used to display the tile
	 * \param pixmap the name of the pixmap
	 */
	void setPixmapName(const QString& pixmap);
private:
	BomberRenderer *m_renderer;
	BomberBoard *m_board;

	QString m_pixmapName;
	QSize m_tileSize;
};

#endif
