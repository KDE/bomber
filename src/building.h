/*
    SPDX-FileCopyrightText: 2007-2008 John-Paul Stanford <jp@stanwood.org.uk>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef BUILDING_H
#define BUILDING_H

// Qt
#include <QList>
#include <QRect>
#include <QSize>

class KGameRenderedItem;
class KGameRenderer;
class BomberBoard;

/**
 * This class is used to represent the Building game objects. Each buildings are made up for
 * building tiles which are placed on the game canvas.
 */
class Building
{
public:
    static const unsigned int BUILD_BASE_LOCATION;
    Building(KGameRenderer * renderer, BomberBoard * board, unsigned int position,
             unsigned int height);
    ~Building();

    /**
     * Set the height of the building
     * \param height The new height of the building
     */
    void setHeight(unsigned int height);

    /**
     * Get the height of the building
     * \return The height of the building
     */
    unsigned int height() const;

    /**
     * Sets width and height of plane.
     */
    void resize(const QSize & tileSize);

    /**
     * Sets building's current frame
     */
    void setFrame(unsigned int frame);

    /**
     * Show the building item on the game scene
     */
    void show();

    /**
     * Get the bounding rectangle of the building
     * \return the Bounding rectangle of the building
     */
    QRectF boundingRect() const;

    /**
     * This is used to destory the top of the building
     */
    void destoryTop();

    /**
     * Return the position of the building
     * \return The position of the building
     */
    QPointF position() const;

private:
    /**
     * Setup the tiles that make up the building
     */
    void setupBuildingTiles();

    /**
     * Used to create a new building tile
     * \param pixmap The pixmap the building tile should be created with
     * \return The new created building tile
     */
    KGameRenderedItem * createBuildingTile(const QString & pixmap);

    unsigned int m_height;
    QRectF m_boundingRect;
    KGameRenderer * m_renderer;
    BomberBoard * m_board;
    QList<KGameRenderedItem *> m_buildingTiles;

    qreal m_xPos;
};

#endif
