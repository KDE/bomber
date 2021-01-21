/*
    SPDX-FileCopyrightText: 2007-2008 John-Paul Stanford <jp@stanwood.org.uk>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

// own
#include "bomber.h"

// Qt
#include <QAction>
#include <QLabel>
#include <QStatusBar>

// KF
#include <KActionCollection>
#include <KLocalizedString>
#include <KMessageBox>
#include <KToggleAction>

// KDEGames
#include <KScoreDialog>
#include <KStandardGameAction>
#include <KgThemeSelector>

// Bomber
#include "settings.h"

Bomber::Bomber()
{
    m_provider.discoverThemes(
        "appdata", QStringLiteral("themes"), //theme data location
        QStringLiteral("kbomber")); //default theme name

    m_selector = new KgThemeSelector(&m_provider);

    m_statusBar = statusBar();
    m_level = new QLabel(i18nc("Used to display the current level of play to the user", "Level: %1", 0));
    m_statusBar->addPermanentWidget(m_level);
    m_score = new QLabel(i18nc("Used to inform the user of their current score", "Score: %1", 0));
    m_statusBar->addPermanentWidget(m_score);
    m_lives = new QLabel(i18nc("Used to tell the user how many lives they have left", "Lives: %1", 3));
    m_statusBar->addPermanentWidget(m_lives);

    m_gameWidget = new BomberGameWidget(&m_provider, this);
    connect(&m_provider, &KgThemeProvider::currentThemeChanged, m_gameWidget, &BomberGameWidget::settingsChanged);

    connect(m_gameWidget, &BomberGameWidget::levelChanged, this, &Bomber::displayLevel);
    connect(m_gameWidget, &BomberGameWidget::scoreChanged, this, &Bomber::displayScore);
    connect(m_gameWidget, &BomberGameWidget::livesChanged, this, &Bomber::displayLives);
    connect(m_gameWidget, &BomberGameWidget::stateChanged, this, &Bomber::gameStateChanged);

    setCentralWidget(m_gameWidget);

    initXMLUI();

    setFocusPolicy(Qt::StrongFocus);

    setFocus();
    setupGUI();
}

Bomber::~Bomber()
{
    delete m_selector;
}

/**
 * create the action events create the gui.
 */
void Bomber::initXMLUI()
{
    // Game
    m_newAction = KStandardGameAction::gameNew(this, &Bomber::newGame, actionCollection());
    KStandardGameAction::end(this, &Bomber::closeGame, actionCollection());
    m_pauseAction = KStandardGameAction::pause(this, &Bomber::pauseGame, actionCollection());
    KStandardGameAction::highscores(this, &Bomber::showHighscore, actionCollection());
    KStandardGameAction::quit(this, &QWidget::close, actionCollection());

    // Settings
    KStandardAction::preferences(this, &Bomber::showPreferences, actionCollection());
    m_soundAction = new KToggleAction(i18nc("Menu item used to disable or enable sound", "&Play Sounds"), this);
    actionCollection()->addAction(QStringLiteral("toggle_sound"), m_soundAction);
    connect(m_soundAction, &KToggleAction::triggered, m_gameWidget, &BomberGameWidget::setSoundsEnabled);

    QAction * dropBombAction = actionCollection()->addAction(QStringLiteral("drop_bomb"));
    dropBombAction->setText(i18nc("The name of the action used for dropping bombs", "&Drop bomb"));
    dropBombAction->setToolTip(i18nc("The tool tip text for the action used to drop bombs", "Drop bomb"));
    dropBombAction->setWhatsThis(i18nc("Description of the action used to drop bombs",
                                       "Makes the plane drop a bomb while flying"));
    actionCollection()->setDefaultShortcut(dropBombAction, Qt::Key_Space);
    dropBombAction->setEnabled(true);
    connect(dropBombAction, &QAction::triggered, m_gameWidget, &BomberGameWidget::onDropBomb);
}

void Bomber::readSettings()
{
    m_soundAction->setChecked(BomberSettings::playSounds());
    m_gameWidget->settingsChanged();
}

void Bomber::newGame()
{
    // Check for running game
    closeGame();
    if (m_gameWidget->state() == BomberGameWidget::State::BeforeFirstGame
        || m_gameWidget->state() == BomberGameWidget::State::GameOver) {
        m_gameWidget->newGame();
    }
}

void Bomber::pauseGame()
{
    if (m_gameWidget->state() == BomberGameWidget::State::Paused) {
        m_gameWidget->setPaused(false);
    } else {
        m_gameWidget->setPaused(true);
    }
}

void Bomber::closeGame()
{
    if (m_gameWidget->state() == BomberGameWidget::State::BeforeFirstGame
        || m_gameWidget->state() == BomberGameWidget::State::GameOver) {
        return;
    }

    BomberGameWidget::State old_state = m_gameWidget->state();
    if (old_state == BomberGameWidget::State::Running) {
        m_gameWidget->setPaused(true);
    }
    int ret = KMessageBox::questionYesNo(this, i18nc("Message displayed when player tries to quit a game that is currently running",
                                                     "Do you really want to close the running game?"),
                                         QString(),
                                         KStandardGuiItem::close(), KStandardGuiItem::cancel());
    if (ret == KMessageBox::Yes) {
        m_gameWidget->closeGame();
    } else if (old_state == BomberGameWidget::State::Running) {
        m_gameWidget->setPaused(false);
    }
}

void Bomber::showHighscore()
{
    KScoreDialog ksdialog(KScoreDialog::Name | KScoreDialog::Score | KScoreDialog::Level, this);
    ksdialog.exec();
}

void Bomber::showPreferences()
{
    m_selector->showAsDialog();
}

void Bomber::highscore()
{
    const unsigned int score = m_gameWidget->score();

    QPointer<KScoreDialog> ksdialog = new KScoreDialog(KScoreDialog::Name | KScoreDialog::Score | KScoreDialog::Level, this);

    KScoreDialog::FieldInfo info;
    info[KScoreDialog::Score].setNum(score);
    info[KScoreDialog::Level].setNum(m_gameWidget->level());
    if (score > 0 && ksdialog->addScore(info) > 0) {
        QString const message = i18n("Congratulations!\nYou made it into the hall of fame.");
        ksdialog->setComment(message);
        ksdialog->exec();
    } else {
        QString const message = i18np("You gained a score of %1 point.", "You gained a score of %1 points.", score);
        KMessageBox::information(this, message, i18n("End of Game"));
    }
    delete ksdialog;
}

void Bomber::displayLevel(unsigned int level)
{
    m_level->setText(i18nc(
        "Used to display the current level of play to the user",
        "Level: %1", level));
}

void Bomber::displayScore(unsigned int score)
{
    m_score->setText(i18nc(
        "Used to inform the user of their current score", "Score: %1",
        score));
}

void Bomber::displayLives(unsigned int lives)
{
    m_lives->setText(i18nc(
        "Used to tell the user how many lives they have left", "Lives: %1",
        lives));
}

void Bomber::gameStateChanged(BomberGameWidget::State state)
{
    switch (state) {
        case BomberGameWidget::State::Paused:
            m_pauseAction->setChecked(true);
            m_statusBar->clearMessage();
            break;
        case BomberGameWidget::State::Running:
            m_pauseAction->setChecked(false);
            m_statusBar->clearMessage();
            break;
        case BomberGameWidget::State::GameOver:
            m_statusBar->showMessage(i18nc("Game over messaged displayed in the status bar", "Game over. Press '%1' for a new game",
                                           m_newAction->shortcuts().constFirst().toString(QKeySequence::NativeText)));
            highscore();
            break;
        default:
            break;
    }
}
