/*
    SPDX-FileCopyrightText: 2007-2008 John-Paul Stanford <jp@stanwood.org.uk>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef BOARD_H
#define BOARD_H

// Qt
#include <QGraphicsScene>
#include <QList>
#include <QQueue>
#include <QSize>

// KDEGames
#include <KGameRenderer>

class Plane;
class Building;
class Bomb;

/**
 * This class used to represent the game board. This makes sure all the game objects
 * get moved and redrawn every second. It also checks for any collisions
 */
class BomberBoard : public QGraphicsScene
{
    Q_OBJECT

public:
    /**
     * The constructor used to create the board.
     * \param renderer The renderer used to render game objects
     * \param view The graphics view object which this board is bound to
     * \param parent The widget which the board is inserted into
     */
    explicit BomberBoard(KGameRenderer * renderer, QGraphicsView * view, QObject * parent = nullptr);

    ~BomberBoard() override;

    /**
     * This is called when the game board is resized
     * \param size The new tile size used on the game board
     */
    void resize(QSize & size);

    /**
     * This will redraw the game board
     */
    void redraw();

    /**
     * This is called to start a new level
     * \param level The level number been started
     */
    void newLevel(unsigned int level);

    /**
     * This is called to pause the game.
     * \param val True if paused, otherwise false
     */
    void setPaused(bool val);

    /**
     * This will convert the tile location to actual cords on the board
     * \param pos The cords relative to the tile
     * \return The cords relative to the widget
     */
    QPoint mapPosition(const QPointF & pos) const;

    /**
     * This will convert the widget location to tile locations
     * \param pos The cords relative to the widget
     * \return The cords relative to the tile
     */
    QPointF unmapPosition(const QPoint & pos) const;

    /**
     * Used to set the plane state to flying and move it to the start position
     */
    void resetPlane();

    /**
     * This will attempt to drop a bomb if their is not already a bomb dropping
     */
    void dropBomb();

Q_SIGNALS:
    /**
     * This is emitted when a plane crashes into a building
     */
    void onPlaneCrash();

    void playBombSound();
    void playCrashSound();

    /**
     * This signal is emitted when a bomb hits a building and before it's exploding
     * animation starts
     */
    void onBombHit();

    /**
     * This is emitted when the level has been cleared of all buildings
     */
    void levelCleared();

public Q_SLOTS:
    /**
     * This is called when the settings change to save the settings
     */
    void settingsChanged();

private Q_SLOTS:
    /**
     * This is called once a second to update and draw all the game objects
     */
    void tick();

    /**
     * This is called when a plane has finished exploding
     */
    void planeExploded();

    /**
     * This is called when a bomb has finished exploding
     */
    void bombExploded();

private:
    /**
     * This is called when a bomb hits a building
     * \param bomb The bomb that hit
     * \param moveBombToX The x position to move the explosion too
     * \param moveBombToY The y position to move the explosion too
     */
    void bombHit(Bomb * bomb, qreal moveBombToX, qreal moveBombToY);

    /**
     * This is used to remove all the current game objects, usually called before
     * stating a new level
     */
    void clear();

    /**
     * This is used to check for any collisions of the plane or bombs
     */
    void checkCollisions();

    /**
     * This is called when a plane crashes into a building
     */
    void crashed();

    /**
     * This is the renderer used to render game objects
     */
    KGameRenderer * m_renderer;

    /**
     * This is the size of a tiling block
     */
    QSize m_tileSize;
    QTimer * m_clock;

    /**
     * If their is a bomb currently dropping then it is pointed to by this
     */
    Bomb * m_bomb;

    /**
     * This points to the plane object used in the level
     */
    Plane * m_plane;

    /**
     * This contains all the buildings in the current level
     */
    QList<Building *> m_buildings;

    /**
     * This contains all the bombs that are currently exploding
     */
    QQueue<Bomb *> m_explodingBombs;

    /**
     * This is the number of blocks that make up the buildings
     */
    unsigned int m_buildingBlocks;

    /**
     * This is the graphics view object which this board is bound.
     */
    QGraphicsView * m_view;
};

#endif
