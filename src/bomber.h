/*
    SPDX-FileCopyrightText: 2007-2008 John-Paul Stanford <jp@stanwood.org.uk>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef BOMBER_H
#define BOMBER_H

// KF
#include <KXmlGuiWindow>

// KDEGames
#include <KgThemeProvider>

// Bomber
#include "bomberwidget.h"

class QAction;
class QStatusBar;
class KToggleAction;
class KgThemeSelector;
class QLabel;

/**
 * This class serves as the main window for Bomber.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 */
class Bomber : public KXmlGuiWindow
{
    Q_OBJECT
public:
    Bomber();
    ~Bomber() override;

    void readSettings();

private Q_SLOTS:
    void closeGame();
    void newGame();
    void pauseGame();
    void showHighscore();
    void showPreferences();

    void displayLevel(unsigned int level);
    void displayScore(unsigned int score);
    void displayLives(unsigned int lives);
    void gameStateChanged(BomberGameWidget::State state);

private:
    void highscore();
    void initXMLUI();

    KgThemeProvider m_provider;
    KgThemeSelector * m_selector;

    BomberGameWidget * m_gameWidget;
    QStatusBar * m_statusBar;

    KToggleAction *m_pauseAction, *m_backgroundShowAction, *m_soundAction;
    QAction * m_newAction;
    QLabel *m_level, *m_score, *m_lives;
};

#endif
