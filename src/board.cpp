/*
    SPDX-FileCopyrightText: 2007-2008 John-Paul Stanford <jp@stanwood.org.uk>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

// own
#include "board.h"

// Qt
#include <QGraphicsView>
#include <QStandardPaths>
#include <QTimer>
#include <QRandomGenerator>


// Bomber
#include "bomb.h"
#include "building.h"
#include "plane.h"
#include "settings.h"


/** The value that the plane velocity increases by */
const qreal PLANE_INC_VELOCITY = 0.0005;
/** The value of this controls the speed of the game */
const unsigned int GAME_DELAY = 15;
/** The number of tiles vertical in the playing area */
const unsigned int TILE_NUM_H = 20;
/** The number of builds to display */
const unsigned int NUMBER_BUILDINGS = 10;
/** The number of tiles horizontally in the playing area */
const unsigned int TILE_NUM_W = ((NUMBER_BUILDINGS) + 2);
/** The maximum level number before the game stops getting harder */
const unsigned int MAX_LEVEL = 11;

/** This time in milliseconds that the plane exploding animation is played for */
const unsigned int PLANE_EXPLODE_TIME = 2000;

/** This time in milliseconds that the bomb exploding animation is played for */
const unsigned int BOMB_EXPLODE_TIME = 1000;

BomberBoard::BomberBoard(KGameRenderer * renderer, QGraphicsView * view, QObject * parent)
    : QGraphicsScene(parent)
    , m_renderer(renderer)
    , m_bomb(nullptr)
    , m_view(view)
{
    m_clock = new QTimer(this);
    m_clock->setInterval(GAME_DELAY);
    connect(m_clock, &QTimer::timeout, this, &BomberBoard::tick);
    m_plane = new Plane(m_renderer, this);
    m_plane->resize(m_tileSize);
    this->addItem(m_plane);
    m_plane->show();
    resetPlane();
    clear();
}

BomberBoard::~BomberBoard()
{
    delete m_bomb;
    delete m_plane;
    qDeleteAll(m_buildings);
    qDeleteAll(m_explodingBombs);
}

void BomberBoard::resetPlane()
{
    m_plane->setState(Explodable::State::Moving);
    m_plane->resetPosition();
}

void BomberBoard::resize(QSize & size)
{
    setBackgroundBrush(m_renderer->spritePixmap(QStringLiteral("background"), size));

    unsigned int minTileSizeWidth = size.width() / TILE_NUM_W;
    unsigned int minTileSizeHeight = size.height() / TILE_NUM_H;

    m_tileSize = QSize(minTileSizeWidth, minTileSizeHeight);

    for (Building * building : qAsConst(m_buildings)) {
        building->resize(m_tileSize);
    }

    m_plane->resize(m_tileSize);
    if (m_bomb != nullptr) {
        m_bomb->resize(m_tileSize);
    }

    redraw();

    size.setWidth(minTileSizeWidth * TILE_NUM_W);
    size.setHeight(minTileSizeHeight * TILE_NUM_H);
}

void BomberBoard::redraw()
{
    m_plane->resetPixmaps();
    if (m_bomb != nullptr) {
        m_bomb->resetPixmaps();
    }
}

void BomberBoard::newLevel(unsigned int level)
{
    if (level > MAX_LEVEL) {
        level = MAX_LEVEL;
    }

    if (level == 1) {
        m_plane->setVelocity(Plane::DEFAULT_VELOCITY);
    } else if (level % 2 == 0) {
        m_plane->setVelocity(m_plane->velocity() + PLANE_INC_VELOCITY);
    }

    m_clock->stop();
    clear();
    m_plane->setState(Explodable::State::Moving);
    m_buildingBlocks = 0;
    //Create the buildings
    for (unsigned int i = 0; i < NUMBER_BUILDINGS; ++i) {
        unsigned int min = level;
        if (min < 3) {
            min = 3;
        }
        unsigned int max = level + 3;
        if (max < 5) {
            max = 5;
        }
        unsigned int height = QRandomGenerator::global()->bounded(max - min) + min;

        m_buildingBlocks += height;
        auto building = new Building(m_renderer, this, i + 1, height);

        building->resize(m_tileSize);
        building->show();

        m_buildings.append(building);
    }
}

void BomberBoard::setPaused(bool val)
{
    if (val) {
        m_clock->stop();
    } else {
        m_clock->start();
    }
}

void BomberBoard::tick()
{
    checkCollisions();

    m_plane->advanceItem();

    if (m_bomb != nullptr) {
        m_bomb->advanceItem();
    }

    for (Bomb * bomb : qAsConst(m_explodingBombs)) {
        bomb->advanceItem();
    }

    // Draw everything
    m_plane->update();

    if (m_bomb != nullptr) {
        m_bomb->update();
    }

    for (Bomb * bomb : qAsConst(m_explodingBombs)) {
        bomb->update();
    }
}

void BomberBoard::dropBomb()
{
    if (m_bomb == nullptr && m_plane->state() == Explodable::State::Moving) {
        QPointF planePos = m_plane->position();
        m_bomb = new Bomb(m_renderer, this, planePos.x(), planePos.y() + 1, m_tileSize);
        this->addItem(m_bomb);
        m_bomb->show();
    }
}

void BomberBoard::checkCollisions()
{
    const auto currentBuildings = m_buildings;
    for (Building * building : currentBuildings) {
        if (m_plane->nextBoundingRect().intersects(building->boundingRect()) && m_plane->state() == Explodable::State::Moving) {
            // Plane crashed into the building
            building->destoryTop();
            --m_buildingBlocks;
            crashed();
        }

        if (m_bomb != nullptr) {
            if (m_bomb->nextBoundingRect().intersects(building->boundingRect()) && m_bomb->state() == Explodable::State::Moving) {
                // Bomb hit a building
                building->destoryTop();
                --m_buildingBlocks;
                Q_EMIT onBombHit();
                bombHit(m_bomb, building->position().x(), Building::BUILD_BASE_LOCATION - (building->height()));
                m_bomb = nullptr;
            } else if (m_bomb->position().y() >= Building::BUILD_BASE_LOCATION + 1) {
                // Bomb hit the ground
                bombHit(m_bomb, (unsigned int)m_bomb->position().x(), Building::BUILD_BASE_LOCATION);
                m_bomb = nullptr;
            }
        }

        if (m_plane->state() == Explodable::State::Moving && m_buildingBlocks == 0) {
            Q_EMIT levelCleared();
        }
    }
}

void BomberBoard::bombHit(Bomb * bomb, qreal moveBombToX, qreal moveBombToY)
{
    bomb->setPosition(moveBombToX, moveBombToY);
    bomb->setState(Bomb::State::Exploding);
    m_explodingBombs.enqueue(bomb);
    QTimer::singleShot(BOMB_EXPLODE_TIME, this, &BomberBoard::bombExploded);
    Q_EMIT playBombSound();
}

void BomberBoard::bombExploded()
{
    Bomb * bomb = m_explodingBombs.dequeue();
    bomb->hide();
    delete bomb;
}

void BomberBoard::settingsChanged()
{
    setBackgroundBrush(m_renderer->spritePixmap(QStringLiteral("background"), m_view->size()));
    redraw();
}

void BomberBoard::planeExploded()
{
    m_plane->setState(Plane::State::Exploded);
    Q_EMIT onPlaneCrash();
}

void BomberBoard::crashed()
{
    QPointF pos = m_plane->position();
    m_plane->setPosition(pos.x() + 1, pos.y());
    m_plane->setState(Plane::State::Exploding);
    QTimer::singleShot(PLANE_EXPLODE_TIME, this, &BomberBoard::planeExploded);
    Q_EMIT playCrashSound();
}

void BomberBoard::clear()
{
    qDeleteAll(m_buildings);
    m_buildings.clear();

    delete m_bomb;
    m_bomb = nullptr;

    resetPlane();
}

QPoint BomberBoard::mapPosition(const QPointF & pos) const
{
    return QPoint(static_cast<unsigned int>(m_tileSize.width() * pos.x()), static_cast<int>(m_tileSize.height() * pos.y()));
}

QPointF BomberBoard::unmapPosition(const QPoint & pos) const
{
    return QPointF(1.0 * pos.x() / m_tileSize.width(), 1.0 * pos.y() / m_tileSize.height());
}
