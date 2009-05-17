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

#ifndef RENDERER_H
#define RENDERER_H

#include <KSvgRenderer>

#include <QSize>
#include <QHash>
#include <QPixmap>

/**
 * Class for rendering elements of game SVG to QPixmap
 */
class BomberRenderer
{
public:
	explicit BomberRenderer();

	~BomberRenderer();

	/**
	 * Loads SVG file and invalidates pixmap cache
	 * \param fileName The SVG theme filename
	 * \return True if successful, otherwise false
	 */
	bool load(const QString& fileName);

	/**
	 * Sets Background size and invalidates background cache
	 * \param size The size of the background
	 */
	void setBackgroundSize(const QSize& size);
	
	/**
	 * Renders background to QPixmap of size set by setBachgroundSize
	 * Background pixmap is cached (setBackgroundSize() invalidates the cache)
	 * \return The pixmap for the background image
	 */
	QPixmap renderBackground();

	/**
	 * Returns number of frames for element in SVG with specified ID
	 * \param id the ID of the SVG element
	 * \return The number of frames for the SVG element.
	 */
	unsigned int frames(const QString& id);

	/**
	 * Renders elements to pixmap.
	 * \param id The id of the element in the SVG theme to render
	 * \param size if == QSize( 0, 0 ) will return rendered element of former size.
	 * \return A pixmap with the element rendered onto it
	 */
	QPixmap renderElement(const QString& id, const QSize& size = QSize(0, 0));
	
	/**
	 * Renders frame of an element to pixmap
	 * \param id The id of the element in the SVG theme to render
	 * \param frame The frame number of the SVG element to be rendered.
	 * \param size if == QSize( 0, 0 ) will return rendered element of former size
	 * \return A pixmap with the element rendered onto it
	 */
	QPixmap renderElement(const QString& id, unsigned int frame, const QSize& size =
			QSize(0, 0));

	/**
	 * Get the maximum number of building styles
	 * \return The maximum number of building styles
	 */
	unsigned int maxBuildingStyles();

private:
	/**
	 * Returns true of the element with the given ID exists in the SVG theme
	 * \param id The element to look for in the SVG theme
	 * \return True if element exists in the SVG theme
	 */
	bool elementExists(const QString& id) const;

	KSvgRenderer m_svgRenderer;
	QSize m_backgroundSize;
	QPixmap m_cachedBackground;
	QHash<QString, QPixmap> m_tileCache;
};

#endif
