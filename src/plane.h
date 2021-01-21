/*
    SPDX-FileCopyrightText: 2007-2008 John-Paul Stanford <jp@stanwood.org.uk>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLANE_H
#define PLANE_H

#include "explodable.h"

/**
 * This class is used to represent the plane object in the game. It extends class
 * explodable as the plane will explode when it hits buildings. The plane
 * flys from left to right and each time it goes off the side of the screen it
 * is repositioned back at the left side, one place lower than before.
 */
class Plane : public Explodable
{
public:
    /** This is the planes size relative to the tile */
    static const qreal PLANE_RELATIVE_SIZE;

    /** This is the position before the plane goes off the screen */
    static const qreal PLANE_MAX_POSITION_X;

    /** The speed the plane will fly at */
    static const qreal DEFAULT_VELOCITY;

    Plane(KGameRenderer * renderer, BomberBoard * board);
    ~Plane() override;

    /**
     * Performs move calculations
     * This method is called once per frame
     */
    void advanceItem();

    /**
     * Used to reset the plane to it's starting position
     */
    void resetPosition();
};

#endif
