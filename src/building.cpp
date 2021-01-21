/*
    SPDX-FileCopyrightText: 2007-2008 John-Paul Stanford <jp@stanwood.org.uk>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

// own
#include "building.h"

// Qt
#include <QRandomGenerator>

// KDEGames
#include <KGameRenderedItem>

// Bomber
#include "board.h"

/**
 * How big is a building in relation to the tiles width.
 * 1.0 means it's the same size as the tile.
 */
const qreal BUILDING_RELATIVE_WIDTH = 1.0;
/**
 * How big is a building in relation to the tiles height.
 * 1.0 means it's the same size as the tile.
 */
const qreal BUILDING_RELATIVE_HEIGHT = 1.0;

/** The vertical tile number of the bottom tile in the building */
const unsigned int Building::BUILD_BASE_LOCATION = 16;

Building::Building(KGameRenderer * renderer, BomberBoard * board, unsigned int position,
                   unsigned int height)
    : m_height(0)
    , m_renderer(renderer)
    , m_board(board)
    , m_xPos(position)
{
    setHeight(height);
    setupBuildingTiles();
}

Building::~Building()
{
    for (KGameRenderedItem * tile : qAsConst(m_buildingTiles)) {
        m_board->removeItem(tile);
    }
    qDeleteAll(m_buildingTiles);
}

void Building::setHeight(unsigned int height)
{
    if (height < m_height) {
        // Building getting smaller so remove the top
        for (unsigned int i = 1; i <= m_height - height; ++i) {
            m_buildingTiles.at(m_height - i)->hide();
            m_board->removeItem(m_buildingTiles.at(m_height - i));
            delete m_buildingTiles.at(m_height - i);
            m_buildingTiles.removeAt(m_height - i);
        }
    }

    m_height = height;
    m_boundingRect.moveTo(m_xPos, BUILD_BASE_LOCATION - m_height + 1);
    m_boundingRect.setSize(QSizeF(BUILDING_RELATIVE_WIDTH,
                                  BUILDING_RELATIVE_HEIGHT * height));
}

void Building::setupBuildingTiles()
{
    //determine number of building styles
    static signed int styleCount = -1;
    if (styleCount == -1) {
        styleCount = 0;
        while (m_renderer->spriteExists(QStringLiteral("building_%1_0").arg(styleCount))) {
            ++styleCount;
        }
    }
    unsigned int style = QRandomGenerator::global()->bounded(styleCount);
    unsigned int maxVarient = m_renderer->frameCount(QStringLiteral("building_%1").arg(style));

    for (unsigned int heightIndex = 0; heightIndex < m_height - 1; ++heightIndex) {
        unsigned int varient = QRandomGenerator::global()->bounded(maxVarient);
        const QString pixmap = QStringLiteral("building_%1_%2").arg(style).arg(varient);
        m_buildingTiles.append(createBuildingTile(pixmap));
    }

    const QString pixmap = QStringLiteral("roof_%1_0").arg(style);
    m_buildingTiles.append(createBuildingTile(pixmap));
    m_boundingRect.moveTo(m_xPos, BUILD_BASE_LOCATION - m_height + 1);
    for (KGameRenderedItem * tile : qAsConst(m_buildingTiles)) {
        m_board->addItem(tile);
    }
}

KGameRenderedItem * Building::createBuildingTile(const QString & pixmap)
{
    auto tile = new KGameRenderedItem(m_renderer, pixmap);
    tile->setRenderSize(QSize(32, 64));
    return tile;
}

void Building::show()
{
    for (KGameRenderedItem * tile : qAsConst(m_buildingTiles)) {
        tile->show();
    }
}

void Building::resize(const QSize & size)
{
    QSize tileSize(static_cast<unsigned int>(BUILDING_RELATIVE_WIDTH * size.width()),
                   static_cast<unsigned int>(BUILDING_RELATIVE_HEIGHT * size.height()));
    for (int i = 0; i < m_buildingTiles.size(); ++i) {
        KGameRenderedItem * tile = m_buildingTiles.at(i);
        tile->setRenderSize(tileSize);
        tile->setPos(m_board->mapPosition(QPointF(m_xPos, BUILD_BASE_LOCATION - i)));
    }
}

QRectF Building::boundingRect() const
{
    return m_boundingRect;
}

void Building::destoryTop()
{
    setHeight(m_height - 1);
}

QPointF Building::position() const
{
    return QPointF(m_xPos, BUILD_BASE_LOCATION);
}

unsigned int Building::height() const
{
    return m_height;
}
