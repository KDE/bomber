/*
    SPDX-FileCopyrightText: 2007-2008 John-Paul Stanford <jp@stanwood.org.uk>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

// own
#include "plane.h"

// Bomber
#include "board.h"

/** The speed the plane will fly at */
const qreal Plane::DEFAULT_VELOCITY = 0.08;

/** This is the planes size relative to the tile */
const qreal Plane::PLANE_RELATIVE_SIZE = 1;

/** This is the position before the plane goes off the screen */
const qreal Plane::PLANE_MAX_POSITION_X = 12;

Plane::Plane(KGameRenderer * renderer, BomberBoard * board)
    : Explodable(QStringLiteral("plane"), QStringLiteral("plane_explode"), PLANE_RELATIVE_SIZE,
                 PLANE_RELATIVE_SIZE, renderer, board)
{
    setVelocity(DEFAULT_VELOCITY);
    resetPosition();
}

Plane::~Plane()
{
}

void Plane::resetPosition()
{
    m_xPos = 0, m_yPos = 0;
    m_nextBoundingRect.moveTo(m_xPos, m_yPos);
}

void Plane::advanceItem()
{
    if (state() == State::Moving) {
        m_xPos += velocity();
        if (m_xPos > PLANE_MAX_POSITION_X) {
            m_xPos = 0;
            ++m_yPos;
        }
    }
    m_nextBoundingRect.moveTo(m_xPos + velocity(), m_yPos);
}
