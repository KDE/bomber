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

#include "explodable.h"

#include <kdebug.h>
#include <KRandom>

#include "board.h"
#include "renderer.h"

const qreal Explodable::EXPLOSION_RELATIVE_SIZE_H = 1.0;
const qreal Explodable::EXPLOSION_RELATIVE_SIZE_W = 1.0;

Explodable::Explodable(QString mainSvg,QString explosionSvg,qreal relativeWidth,qreal relativeHeight,BomberRenderer* renderer, BomberBoard* board) :
	KGameCanvasPixmap(board), m_renderer(renderer), m_board(board),m_mainSvg(mainSvg),m_explosionSvg(explosionSvg)
{
    m_relativeWidth = relativeWidth;
    m_relativeHeight = relativeHeight;
    m_size = QSize( 32, 64);
	resetPixmaps();
	m_state = Moving;
	m_nextBoundingRect.setSize(QSizeF(m_relativeWidth,m_relativeHeight) );
	// m_lastSize = tileSize;
	//m_size.setWidth( static_cast<int>(m_relativeWidth * tileSize.width() ) );
	//m_size.setHeight( static_cast<int> (m_relativeHeight * tileSize.height() ) );
	//moveTo(m_board->mapPosition(QPointF(m_xPos, m_yPos) ) );
}

Explodable::~Explodable()
{
}

void Explodable::setPosition(qreal xPos, qreal yPos)
{
	m_xPos=xPos, m_yPos= yPos;
	m_nextBoundingRect.moveTo(m_xPos, m_yPos);
}

void Explodable::update()
{
	m_frame++;
	if (m_state == Moving)
	{
		if (m_frame >= m_mainFramesNum)
		{
			m_frame = 0;
		}
	}
	else
	{
		if (m_frame >= m_explosionFramesNum)
		{
			m_frame = 0;
		}
	}

	setFrame(m_frame);
	moveTo(m_board->mapPosition(QPointF(m_xPos, m_yPos) ) );
}

void Explodable::resize(const QSize& tileSize)
{
	m_lastSize = tileSize;
	if (m_state == Moving)
	{
		m_size.setWidth( static_cast<int>(m_relativeWidth
				* tileSize.width() ) );
		m_size.setHeight( static_cast<int> (m_relativeHeight
				* tileSize.height() ) );
	}
	else
	{
		m_size.setWidth( static_cast<int>(EXPLOSION_RELATIVE_SIZE_W
				* tileSize.width() ) );
		m_size.setHeight( static_cast<int> (EXPLOSION_RELATIVE_SIZE_H
				* tileSize.height() ) );
	}

	moveTo(m_board->mapPosition(QPointF(m_xPos, m_yPos) ) );
	setFrame(m_frame);
}

void Explodable::setVelocity(qreal vX)
{
	m_velocity = vX;
}

void Explodable::setRandomFrame()
{
	int frame = 0;
	if (m_state == Moving)
	{
		if (m_mainFramesNum > 0)
			frame = KRandom::random() % m_mainFramesNum;
	}
	setFrame(frame);
}

/**
 * Returns bomb's bounding rect expected in next frame
 * used by colision test
 */
QRectF Explodable::nextBoundingRect() const
{
	return m_nextBoundingRect;
}

void Explodable::setState(Explodable::State state)
{
	m_state = state;
	setRandomFrame();
	if (m_state == Moving)
	{
		m_nextBoundingRect.setSize(QSizeF(m_relativeWidth,
				m_relativeHeight) );
	}
	else
	{
		m_nextBoundingRect.setSize(QSizeF(EXPLOSION_RELATIVE_SIZE_W,
				EXPLOSION_RELATIVE_SIZE_H) );
	}
	resize(m_lastSize);
}

QPointF Explodable::position()
{
	return QPointF(m_xPos, m_yPos);
}

void Explodable::resetPixmaps()
{
	m_frame = 0;
	m_mainFramesNum = m_renderer->frames(m_mainSvg );
	m_explosionFramesNum = m_renderer->frames(m_explosionSvg );
	setFrame(m_frame);
}

void Explodable::setFrame(int frame)
{
	if (m_state == Moving)
	{
		setPixmap(m_renderer->renderElement(m_mainSvg, frame, m_size) );
	}
	else
	{
		setPixmap(m_renderer->renderElement(m_explosionSvg, frame,
				m_size) );
	}
}
