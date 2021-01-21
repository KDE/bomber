/*
    SPDX-FileCopyrightText: 2007-2008 John-Paul Stanford <jp@stanwood.org.uk>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

// own
#include "bomb.h"

// Bomber
#include "board.h"

/** The speed the bomb will fall at */
const qreal DEFAULT_VELOCITY = 0.2;

const qreal Bomb::BOMB_RELATIVE_SIZE_H = 0.7;
const qreal Bomb::BOMB_RELATIVE_SIZE_W = 0.2;

Bomb::Bomb(KGameRenderer * renderer, BomberBoard * board, qreal xPos, qreal yPos, QSize tileSize)
    : Explodable(QStringLiteral("bomb"), QStringLiteral("bomb_explode"), BOMB_RELATIVE_SIZE_W,
                 BOMB_RELATIVE_SIZE_H, renderer, board)
{
    setVelocity(DEFAULT_VELOCITY);
    setPosition(xPos, yPos);
    resize(tileSize);
}

Bomb::~Bomb()
{
}

void Bomb::advanceItem()
{
    if (state() == State::Moving) {
        m_yPos += velocity();
    }
    m_nextBoundingRect.moveTo(m_xPos, m_yPos + velocity());
}
