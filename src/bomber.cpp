/*
 * Copyright (C) 2007-2008 John-Paul Stanford <jp@stanwood.org.uk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "bomber.h"

#include <KAction>
#include <KActionCollection>
#include <KConfigDialog>
#include <kdebug.h>
#include <KGlobal>
#include <KLocale>
#include <KMessageBox>
#include <krandom.h>
#include <KStatusBar>
#include <KStandardDirs>
#include <KToggleAction>

#include <kstandardgameaction.h>
#include <KScoreDialog>
#include <KGameThemeSelector>

#include "bomberwidget.h"
#include "settings.h"

Bomber::Bomber()
{
    m_statusBar = statusBar();
    m_statusBar->insertItem(i18n("Level: %1", QString::fromLatin1("XX") ), 1, 1);
    m_statusBar->insertItem(i18n("Score: %1", QString::fromLatin1("XXXXXX") ), 2, 1);
    m_statusBar->insertItem(i18n("Lives: %1", QString::fromLatin1("XX") ), 4, 1);

    m_gameWidget =new BomberGameWidget(this);

    connect(m_gameWidget, SIGNAL( levelChanged( int ) ), this, SLOT( displayLevel( int ) ));
    connect(m_gameWidget, SIGNAL( scoreChanged( int
            ) ), this, SLOT( displayScore( int ) ));
    connect(m_gameWidget, SIGNAL( livesChanged( int
            ) ), this, SLOT( displayLives( int ) ));
    connect(m_gameWidget,
    SIGNAL(stateChanged(BomberGameWidget::State) ), this,
    SLOT(gameStateChanged(BomberGameWidget::State) ));

    setCentralWidget(m_gameWidget);

    initXMLUI();

    setFocusPolicy(Qt::StrongFocus);

    setFocus();
    setupGUI();

    // readSettings();
}

Bomber::~Bomber()
{
}

/**
 * create the action events create the gui.
 */
void Bomber::initXMLUI()
{
    // Game
    m_newAction = KStandardGameAction::gameNew(this, SLOT(newGame()), actionCollection());
    KStandardGameAction::end(this, SLOT(closeGame()), actionCollection());
    m_pauseAction = KStandardGameAction::pause(this, SLOT(pauseGame()), actionCollection());
    KStandardGameAction::highscores(this, SLOT(showHighscore()), actionCollection());
    KStandardGameAction::quit(this, SLOT(close()), actionCollection());

    // Settings
    KStandardAction::preferences( this, SLOT(configureSettings() ), actionCollection() );
    m_soundAction = new KToggleAction( i18n("&Play Sounds"), this );
    actionCollection()->addAction("toggle_sound", m_soundAction);
    connect(m_soundAction, SIGNAL( triggered( bool ) ), this, SLOT( setSounds( bool ) ));

    QAction *dropBombAction = actionCollection()->addAction ("drop_bomb");
    dropBombAction->setText (i18n ("&Drop bomb"));
    dropBombAction->setToolTip (i18n ("Drop bomb"));
    dropBombAction->setWhatsThis (i18n ("Makes the plane drop a bomb while flying"));
    dropBombAction->setShortcut (Qt::Key_Space);
    dropBombAction->setEnabled (true);
    connect (dropBombAction, SIGNAL (triggered (bool)), m_gameWidget, SLOT (onDropBomb()));
}

void Bomber::readSettings()
{
    m_soundAction->setChecked(BomberSettings::playSounds() );
    settingsChanged();
}

void Bomber::newGame()
{
    // Check for running game
    closeGame();
    if (m_gameWidget->state()== BomberGameWidget::BeforeFirstGame|| m_gameWidget->state()== BomberGameWidget::GameOver)
    {
        m_gameWidget->newGame();
    }
}

void Bomber::pauseGame()
{
    if (m_gameWidget->state()== BomberGameWidget::Paused)
    {
        m_gameWidget->setPaused( false);
    }
    else
    {
        m_gameWidget->setPaused( true);
    }
}

void Bomber::closeGame()
{
    if (m_gameWidget->state()== BomberGameWidget::BeforeFirstGame|| m_gameWidget->state()== BomberGameWidget::GameOver)
    {
        return;
    }

    BomberGameWidget::State old_state =m_gameWidget->state();
    if (old_state == BomberGameWidget::Running)
        m_gameWidget->setPaused( true);
    int ret =KMessageBox::questionYesNo(this, i18n("Do you really want to close the running game?"), QString(), KStandardGuiItem::close(),
            KStandardGuiItem::cancel() );
    if (ret == KMessageBox::Yes)
    {
        m_gameWidget->closeGame();
    }
    else if (old_state == BomberGameWidget::Running)
    {
        m_gameWidget->setPaused( false);
    }
}

void Bomber::gameOverNow()
{
    statusBar()->showMessage(i18n("Game over. Press <Space> for a new game") );
    highscore();
}

/**
 * Bring up the standard kde high score dialog.
 */
void Bomber::showHighscore()
{
    KScoreDialog ksdialog(KScoreDialog::Name | KScoreDialog::Score| KScoreDialog::Level, this);
    ksdialog.exec();
}

void Bomber::highscore()
{
    KScoreDialog ksdialog(KScoreDialog::Name | KScoreDialog::Score| KScoreDialog::Level, this);

    KScoreDialog::FieldInfo info;
    info[KScoreDialog::Score].setNum(m_gameWidget->score() );
    info[KScoreDialog::Level].setNum(m_gameWidget->level() );
    if (ksdialog.addScore(info) )
        ksdialog.exec();
}

void Bomber::configureSettings()
{
    if (KConfigDialog::showDialog("settings") )
        return;

    KConfigDialog* dialog = new KConfigDialog( this, "settings", BomberSettings::self());
    dialog->addPage(new KGameThemeSelector( dialog, BomberSettings::self(), KGameThemeSelector::NewStuffDisableDownload ), i18n("Theme"), "games-config-theme");
    dialog->setHelp(QString(), "bomber");
    dialog->show();
    connect(dialog, SIGNAL( settingsChanged( const QString& ) ), this, SLOT( settingsChanged() ));
}

void Bomber::settingsChanged()
{
    m_gameWidget->settingsChanged();
}

void Bomber::setSounds(bool val)
{
    BomberSettings::setPlaySounds(val);
    settingsChanged();
}

void Bomber::displayLevel(int level)
{
    m_statusBar->changeItem(i18n("Level: %1", level), 1);
}

void Bomber::displayScore(int score)
{
    m_statusBar->changeItem(i18n("Score: %1", score), 2);
}

void Bomber::displayLives(int lives)
{

    m_statusBar->changeItem(i18n("Lives: %1", lives), 4);
}

void Bomber::gameStateChanged(BomberGameWidget::State state)
{
    switch (state)
    {
    case BomberGameWidget::Paused:
        m_pauseAction->setChecked( true);
        m_statusBar->clearMessage();
        break;
    case BomberGameWidget::Running:
        m_pauseAction->setChecked( false);
        m_statusBar->clearMessage();
        break;
    case BomberGameWidget::GameOver:
        statusBar()->showMessage(i18n("Game over. Press %s for a new game",m_pauseAction->globalShortcut().toString()));
        highscore();
        break;
    default:
    	break;
    }
}

#include "bomber.moc"
