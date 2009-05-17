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

#include "renderer.h"

#include <kdebug.h>

#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QPalette>

BomberRenderer::BomberRenderer() :
	m_svgRenderer(), m_backgroundSize(QSize(0, 0))
{

}

BomberRenderer::~BomberRenderer()
{

}

bool BomberRenderer::load(const QString& fileName)
{
	m_tileCache.clear();
	m_cachedBackground = QPixmap();

	bool success = m_svgRenderer.load(fileName);

	renderBackground();

	return success;
}

void BomberRenderer::setBackgroundSize(const QSize& size)
{
	if (size != m_backgroundSize)
	{
		m_cachedBackground = QPixmap();
		m_backgroundSize = size;
	}
}

QPixmap BomberRenderer::renderBackground()
{
	if (m_cachedBackground.isNull())
	{
		//This is a dirty fix to the qt's m_svgRenderer.render() method that
		//leaves an garbage-filled border of a pixmap
		m_cachedBackground = QPixmap(m_backgroundSize);
		m_cachedBackground.fill(QApplication::palette().window().color());
		QPainter p(&m_cachedBackground);
		m_svgRenderer.render(&p, "background");
	}
	return m_cachedBackground;
}

bool BomberRenderer::elementExists(const QString& id) const
{
	return m_svgRenderer.elementExists(id);
}

unsigned int BomberRenderer::frames(const QString& id)
{
	unsigned int frame = 0;
	while (elementExists(id + '_' + QString::number(frame)))
		frame++;
	return frame;
}

QPixmap BomberRenderer::renderElement(const QString& id, const QSize& size)
{
	QHash<QString, QPixmap>::iterator elementIt = m_tileCache.find(
			id);
	QHash<QString, QPixmap>::iterator itEnd = m_tileCache.end();
	if (elementIt == itEnd && size.isEmpty())
	{
		return QPixmap();
	}

	if (elementIt == itEnd || (size != QSize(0, 0) && size
			!= elementIt.value().size()))
	{
		QImage baseImage(size, QImage::Format_ARGB32_Premultiplied);
		baseImage.fill(0);

		QPainter p(&baseImage);
		m_svgRenderer.render(&p, id);
		p.end();

		QPixmap renderedTile = QPixmap::fromImage(baseImage);
		elementIt = m_tileCache.insert(id, renderedTile);
	}

	return elementIt.value();
}

QPixmap BomberRenderer::renderElement(const QString& id, unsigned int frame,
		const QSize& size)
{
	QString name = id + '_' + QString::number(frame);
	return renderElement(name, size);
}

unsigned int BomberRenderer::maxBuildingStyles()
{
	static signed int style = -1;
	if (style == -1)
	{
		style = 0;
		while (elementExists(QString("building_") + QString::number(style)
				+ QString("_0")))
			style++;
	}
	return style;
}
