/*
    SPDX-FileCopyrightText: 2007-2008 John-Paul Stanford <jp@stanwood.org.uk>
    SPDX-FileCopyrightText: 2010 Stefan Majewsky <majewsky@gmx.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef BOMBER_WIDGET_H
#define BOMBER_WIDGET_H

// Qt
#include <QGraphicsView>
#include <QMouseEvent>

// KDEGames
#include <KgSound>

// Bomber
#include "board.h"

class KgThemeProvider;

/**
 * This is the main game widget class. It manages things like the lives, source counts and game states.
 */
class BomberGameWidget : public QGraphicsView
{
    Q_OBJECT

public:
    /**
     * The different states that the game can be in
     */
    enum class State { BeforeFirstGame,
                       Running,
                       BetweenLevels,
                       Paused,
                       Suspended,
                       GameOver };

    explicit BomberGameWidget(KgThemeProvider * provider, QWidget * parent = nullptr);
    ~BomberGameWidget() override;

    /**
     * Used to find out the current level
     * \return The current level
     */
    unsigned int level() const;

    /**
     * Used to find the current score
     * \return the current score
     */
    unsigned int score() const;

    /**
     * Used to return the game sate
     * \return The game state
     */
    BomberGameWidget::State state() const;

public Q_SLOTS:
    /**
     * Used to end the game
     */
    void closeGame();

    /**
     * Used to start a new game
     */
    void newGame();

    /**
     * Used to pause the game
     * \param paused True to pause the game, False to resume the game
     */
    void setPaused(bool paused);

    /**
     * This is called when the settings change to save the settings
     */
    void settingsChanged();

    /**
     * Enabled or disabled the sounds
     * \param enable True to enable the sounds, false to disable
     */
    void setSoundsEnabled(bool enable);

    /**
     * Used to toggle the suspended game state. If val is true and the state is currently running,
     * them set the sate to suspended. If value is false and the state is suspended, set the sate to running.
     * \param value True to set state to suspended, False to set state to running.
     */
    void setSuspended(bool value);

    /**
     * This is called when the drop bomb key is press or the mouse in the game area.
     * If the game state is running, If it is between levels, then a new level is created and
     * if it's BeforeFirstGame then a new game is started.
     */
    void onDropBomb();

Q_SIGNALS:
    /**
     * This is emitted when the game is over
     */
    void gameOver();

    /**
     * This is emitted when the level changes.
     * \param level The new level
     */
    void levelChanged(unsigned int level);

    /**
     * This is emitted when the source changes
     * \param score The new score
     */
    void scoreChanged(unsigned int score);

    /**
     * This is emitted when the lives change
     * \param lives The new lives
     */
    void livesChanged(unsigned int lives);

    /**
     * This is emitted when the time changes
     * \param time The new time value
     */
    void timeChanged(unsigned int time);

    /**
     * This is emitted when the game state changes
     * \param state The new game state
     */
    void stateChanged(BomberGameWidget::State state);

private Q_SLOTS:
    void tick();

    /**
     * This is called when a bomb hits a building
     */
    void onBombHit();

    /**
     * This slot is called when a level has been cleared to change the state and get
     * ready for the next level.
     */
    void onLevelCleared();

    /**
     * This slot is called when the plane crashes
     */
    void onPlaneCrashed();

private:
    void resizeEvent(QResizeEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;

    /**
     * Create the overlay used to display info to the user. The info will
     * change depending on the game state.
     */
    void generateOverlay();
    void redraw();

    void closeLevel();
    void newLevel();

    void playBombSound();
    void playCrashSound();

    State m_state;
    unsigned int m_level;
    unsigned int m_score;
    unsigned int m_lives;
    unsigned int m_time;

    /**
     * Used to store the remaining score before a new life is given
     */
    int m_scoreLeftBeforeNewLife;

    KGameRenderer m_renderer;
    QTimer * m_clock;
    BomberBoard * m_board;

    QGraphicsPixmapItem * m_overlay;

    KgSound m_soundBomb;
    KgSound m_soundCrash;
};

#endif
