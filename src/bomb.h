/*
    SPDX-FileCopyrightText: 2007-2008 John-Paul Stanford <jp@stanwood.org.uk>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef BOMB_H
#define BOMB_H

// Bomber
#include "explodable.h"

/**
 * This is the bomb game object class. It extends Explodable as it
 * is a explodable object.
 */
class Bomb : public Explodable
{
public:
    /** The width of the bomb relative to the tile */
    static const qreal BOMB_RELATIVE_SIZE_W;
    /** The height of the bomb relative to the tile */
    static const qreal BOMB_RELATIVE_SIZE_H;

    Bomb(KGameRenderer * renderer, BomberBoard * board, qreal xPos, qreal yPos,
         QSize tileSize);
    ~Bomb() override;

    /**
     * Performs move calculations
     * This method is called once per frame
     */
    void advanceItem();
};

#endif
