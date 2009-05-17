/* Copyright (C) 2007-2008 John-Paul Stanford <jp@stanwood.org.uk>
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
#include "bomberwidget.h"
#include "settings.h"

#include <QPalette>
#include <QTimer>

#include <kdebug.h>

#include <KLocale>
#include <kgametheme.h>

#define NEW_LIVE_AT_SCORE 10000;

/** The amount the score increases when a bomb hits something */
static const unsigned int SCORE_INCREMENT = 5;

static const unsigned int GAME_TIME_DELAY = 1000;
static const unsigned int TICKS_PER_SECOND = 1000 / GAME_TIME_DELAY;

BomberGameWidget::BomberGameWidget(QWidget *parent) :
	KGameCanvasWidget(parent), m_state(BeforeFirstGame), m_level(0),m_lives(0), m_time(0)
{
	m_theme = new KGameTheme("KGameTheme");
	m_theme->loadDefault();

	m_board = new BomberBoard(&m_renderer, this, this);
	connect(m_board, SIGNAL(onPlaneCrash()), this, SLOT(onPlaneCrashed()));
	connect(m_board, SIGNAL(onBombHit()), this, SLOT(onBombHit()));
	connect(m_board, SIGNAL(levelCleared()), this, SLOT(onLevelCleared()));

	m_overlay = new KGameCanvasPixmap(this);

	m_overlay->raise();
	m_overlay->hide();

	m_clock = new QTimer(this);
	m_clock->setInterval(GAME_TIME_DELAY);

	connect(m_clock, SIGNAL(timeout() ), this, SLOT(tick() ));

	setMouseTracking(true);
}

BomberGameWidget::~BomberGameWidget()
{
	delete m_board;
	delete m_overlay;
	delete m_theme;
}

unsigned int BomberGameWidget::level() const
{
	return m_level;
}

unsigned int BomberGameWidget::score() const
{
	return m_score;
}

void BomberGameWidget::closeGame()
{
	if (m_state != BeforeFirstGame && m_state != GameOver)
	{
		closeLevel();

		m_state = GameOver;
		emit stateChanged(m_state);
		emit gameOver();

		redraw();
	}
}

void BomberGameWidget::newGame()
{
	closeGame();

	m_level = 1;
	m_score = 0;
	m_lives = 3;
	m_scoreLeftBeforeNewLife = NEW_LIVE_AT_SCORE;

	emit levelChanged(m_level);
	emit scoreChanged(m_score);
	emit livesChanged(m_lives);

	newLevel();
}

void BomberGameWidget::setPaused(bool val)
{
	if (m_state == Paused && val == false)
	{
		m_clock->start();
		m_board->setPaused(false);
		m_state = Running;
		emit stateChanged(m_state);
	}
	else if (m_state == Running && val == true)
	{
		m_clock->stop();
		m_board->setPaused(true);
		m_state = Paused;
		emit stateChanged(m_state);
	}

	redraw();
}

void BomberGameWidget::setSuspended(bool val)
{
	if (m_state == Suspended && val == false)
	{
		m_clock->start();
		m_board->setPaused(false);
		m_state = Running;
		emit stateChanged(m_state);
	}
	if (m_state == Running && val == true)
	{
		m_clock->stop();
		m_board->setPaused(true);
		m_state = Suspended;
		emit stateChanged(m_state);
	}
	redraw();
}

void BomberGameWidget::settingsChanged()
{
	m_board->setSounds(BomberSettings::playSounds());

	if (!m_theme->load(BomberSettings::theme()))
	{
		kDebug("Load default theme");
		m_theme->loadDefault();
	}

	m_renderer.load(m_theme->graphics());

	QPalette palette;
	palette.setBrush(backgroundRole(), m_renderer.renderBackground());

	setPalette(palette);
	redraw();
}

void BomberGameWidget::setSounds(bool val)
{
	m_board->setSounds(val);
}

void BomberGameWidget::onPlaneCrashed()
{
	m_lives--;
	emit livesChanged(m_lives);
	if (m_lives <= 0)
	{
		closeGame();
	}
	else
	{
		m_board->resetPlane();
	}
}

void BomberGameWidget::onBombHit()
{
	unsigned int bonus = SCORE_INCREMENT * m_level;
	m_score += bonus;emit
	scoreChanged(m_score);
	m_scoreLeftBeforeNewLife -= bonus;
	if (m_scoreLeftBeforeNewLife <= 0)
	{
		m_scoreLeftBeforeNewLife = NEW_LIVE_AT_SCORE;
		m_lives++;
		emit livesChanged(m_lives);
	}

}

void BomberGameWidget::tick()
{
	static signed int ticks = TICKS_PER_SECOND;
	ticks--;
	if (ticks <= 0)
	{
		m_time++;emit
		timeChanged(m_time);
		ticks = TICKS_PER_SECOND;
	}
}

void BomberGameWidget::resizeEvent(QResizeEvent *ev)
{
	QPalette palette;
	m_renderer.setBackgroundSize(ev->size());
	palette.setBrush(backgroundRole(), m_renderer.renderBackground());
	setPalette(palette);
	setAutoFillBackground(true);

	QSize boardSize(ev->size().width(), ev->size().height());
	m_board->resize(boardSize);
	m_board->moveTo((ev->size().width() - boardSize.width()) / 2,
			(ev->size().height() - boardSize.height()) / 2);

	redraw();
}

void BomberGameWidget::closeLevel()
{
	m_clock->stop();
	m_board->setPaused(true);
}

void BomberGameWidget::newLevel()
{
	m_state = Running;emit
	stateChanged(m_state);

	m_clock->start();
	m_board->newLevel(m_level);
	m_board->setPaused(false);

	emit
	livesChanged(m_lives);emit
	timeChanged(m_time);

	redraw();
}

void BomberGameWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() & Qt::LeftButton)
	{
		onDropBomb();
	}
}

void BomberGameWidget::onDropBomb()
{
	if (m_state == Running)
	{
		m_board->dropBomb();
	}
	else if (m_state == BetweenLevels)
	{
		newLevel();
	}
	else if (m_state == BeforeFirstGame)
	{
		newGame();
	}
}

void BomberGameWidget::redraw()
{
	if (size().isEmpty())
		return;

	switch (m_state)
	{
	case BeforeFirstGame:
		m_board->hide();
		generateOverlay();
		m_overlay->show();
		break;
	case Running:
		m_board->show();
		m_overlay->hide();
		break;
	default:
		m_board->show();
		generateOverlay();
		m_overlay->show();
		break;
	}
	m_board->redraw();
	update();
}

void BomberGameWidget::generateOverlay()
{
	unsigned int itemWidth = qRound(0.8 * size().width());
	unsigned int itemHeight = qRound(0.6 * size().height());

	QPixmap px(itemWidth, itemHeight);
	px.fill(QColor(0, 0, 0, 0));

	QPainter p(&px);
	p.setPen(QColor(0, 0, 0, 0));
	p.setBrush(QBrush(QColor(188, 202, 222, 155)));
	p.setRenderHint(QPainter::Antialiasing);
	p.drawRoundRect(0, 0, itemWidth, itemHeight, 25);

	QString text;
	switch (m_state)
	{
	case BeforeFirstGame:
		text = i18nc("Message show to the user when the game is loaded","Welcome to Bomber.\nClick to start a game");
		break;
	case Paused:
		text = i18nc("Message show to the user while the game is paused","Paused");
		break;
	case BetweenLevels:
		text = i18nc("Message telling user which level they just completed","You have successfully cleared level %1\n", m_level - 1)
				+ i18nc("Message telling user which level they are about to start","On to level %1.", m_level);
		break;
	case GameOver:
		text = i18nc("Used to tell the user that the game is over","Game over.");
		break;
	default:
		text.clear();
	}

	QFont font;
	font.setPointSize(28);
	p.setFont(font);
	unsigned int textWidth = p.boundingRect(p.viewport(), Qt::AlignCenter | Qt::AlignVCenter, text).width();
	unsigned int fontSize = 28;
	while ((textWidth > itemWidth * 0.95))
	{
		fontSize--;
		font.setPointSize(fontSize);
		p.setFont(font);
		textWidth = p.boundingRect(p.viewport(), Qt::AlignCenter | Qt::AlignVCenter, text).width();
	}

	p.setPen(QColor(0, 0, 0, 255));
	p.drawText(p.viewport(), Qt::AlignCenter | Qt::AlignVCenter, text);
	p.end();

	m_overlay->setPixmap(px);
	m_overlay->moveTo((size().width() - itemWidth) / 2, (size().height() - itemHeight) / 2);
}

void BomberGameWidget::onLevelCleared()
{
	if (m_state == Running)
	{
		m_state = BetweenLevels;
		closeLevel();
		m_level++;emit
		levelChanged(m_level);

		emit
		stateChanged(m_state);
		redraw();
	}

}

BomberGameWidget::State BomberGameWidget::state() const
{ 
  return m_state;
}
