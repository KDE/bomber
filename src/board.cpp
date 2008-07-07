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
#include "kgamecanvas.h"

#include "board.h"

#include <kdebug.h>
#include <KGlobal>
#include <KLocale>
#include <KRandom>
#include <Phonon/MediaObject>
#include <KStandardDirs>

#include <QTimer>

#include "plane.h"
#include "building.h"
#include "bomb.h"

#define PLANE_INC_VELOCITY 0.0005
#define GAME_DELAY 15
#define TILE_NUM_H 20
#define NUMBER_BUILDINGS 10
#define TILE_NUM_W ((NUMBER_BUILDINGS)+2)
#define MAX_LEVEL 11

/** This time in miliseconds that the plane exloding animation is played for */
#define PLANE_EXPLODE_TIME 2000

/** This time in miliseconds that the bomb exloding animation is played for */
#define BOMB_EXPLODE_TIME 1000

BomberBoard::BomberBoard(BomberRenderer* renderer, KGameCanvasAbstract* canvas,
		QWidget* parent) :
	QObject(parent), KGameCanvasGroup(canvas), m_renderer(renderer)
{
	m_bomb=NULL;
	m_clock = new QTimer( this );
	m_clock->setInterval(GAME_DELAY);
	connect(m_clock, SIGNAL(timeout() ), this, SLOT(tick() ));

	m_plane = new Plane(m_renderer,this);
	m_plane->resize(m_tileSize);
	m_plane->raise();
	m_plane->show();
	resetPlane();

	clear();

	m_audioPlayer = 0;
	m_playSounds = false;
	m_soundPath = KStandardDirs::locate("appdata", "sounds/");

}

BomberBoard::~BomberBoard()
{
	delete m_bomb;
	delete m_plane;
	qDeleteAll(m_buildings);
	qDeleteAll(m_explodingBombs);
	delete m_audioPlayer;
}

void BomberBoard::resetPlane()
{
	m_plane->setState(Explodable::Moving);
	m_plane->resetPosition();
}

void BomberBoard::resize(QSize& size)
{
	int minTileSizeWidth, minTileSizeHeight;

	minTileSizeWidth = size.width() / TILE_NUM_W;
	minTileSizeHeight = size.height() / TILE_NUM_H;

	m_tileSize = QSize(minTileSizeWidth, minTileSizeHeight);

	foreach(Building* building, m_buildings)
	{
		building->resize(m_tileSize);
	}

	m_plane->resize(m_tileSize);
	if (m_bomb!=NULL)
	{
		m_bomb->resize(m_tileSize);
	}

	redraw();

	size.setWidth(minTileSizeWidth * TILE_NUM_W);
	size.setHeight(minTileSizeHeight * TILE_NUM_H);
}

void BomberBoard::redraw()
{
	m_plane->resetPixmaps();
	if (m_bomb !=NULL)
	{
		m_bomb->resetPixmaps();
	}
}

void BomberBoard::newLevel(int level)
{
	if (level > MAX_LEVEL)
	{
		level = MAX_LEVEL;
	}
	
	if (level==1) {
	    m_plane->setVelocity(Plane::DEFAULT_VELOCITY);
	}
	else if (level % 2 == 0){	    
	    m_plane->setVelocity(m_plane->velocity()+PLANE_INC_VELOCITY);
	}
	
	m_clock->stop();
	clear();
	m_plane->setState(Explodable::Moving);
	m_buildingBlocks = 0;
	//Create the buildings
	for (int i=0; i<NUMBER_BUILDINGS; i++)
	{	   
	    int min = level;
	    if (min < 3) {
	        min = 3;
	    }	 
	    int max = level +3;
	    if (max<5) {
	        max = 5;
	    }	    
		int height = (KRandom::random() % (max-min))+min;		
		
		m_buildingBlocks+=height;
		Building *building = new Building(m_renderer,this,i+1,height);

		building->resize(m_tileSize);
		building->raise();
		building->show();

		m_buildings.append(building);
	}
}

void BomberBoard::setPaused(bool val)
{
	if (val)
		m_clock->stop();
	else
		m_clock->start();
}

void BomberBoard::playSound(const QString& name)
{
	if (m_playSounds == true && !name.isEmpty())
	{		    
	    QString file = m_soundPath.filePath(name);	    	   
		m_audioPlayer->setCurrentSource(file);
		m_audioPlayer->play();	    
	}
}

void BomberBoard::setSounds(bool val)
{
	m_playSounds = val;
	if (val == true&& m_audioPlayer == 0)
	{
		m_audioPlayer = Phonon::createPlayer(Phonon::GameCategory);
	}
}

void BomberBoard::tick()
{
	checkCollisions();

	// Move everything
	foreach(Building* building, m_buildings)
	{
		building->advanceItem();
	}

	m_plane->advanceItem();

	if (m_bomb!=NULL)
	{
		m_bomb->advanceItem();
	}

	foreach(Bomb* bomb, m_explodingBombs)
	{
		bomb->advanceItem();
	}

	// Draw everything
	m_plane->update();

	foreach(Building* building, m_buildings)
	{
		building->update();
	}

	if (m_bomb!=NULL)
	{
		m_bomb->update();
	}

	foreach(Bomb* bomb, m_explodingBombs)
	{
		bomb->update();
	}
}

void BomberBoard::dropBomb()
{
	if (m_bomb == NULL&& m_plane->state() == Explodable::Moving)
	{
		m_bomb = new Bomb(m_renderer,this);
		m_bomb->resize(m_tileSize);
		QPointF planePos = m_plane->position();
		m_bomb->setPosition(planePos.x(), planePos.y()+1);
		m_bomb->raise();
		m_bomb->show();
	}
}

void BomberBoard::checkCollisions()
{
	foreach(Building *building, m_buildings)
	{
		if (m_plane->nextBoundingRect().intersects(building->boundingRect()) && m_plane->state()
				== Explodable::Moving)
		{
			building->destoryTop();
			m_buildingBlocks--;
			crashed();
		}

		if (m_bomb!=NULL)
		{
			if (m_bomb->nextBoundingRect().intersects(building->boundingRect()) && m_bomb->state()
					== Explodable::Moving)
			{
				building->destoryTop();
				m_buildingBlocks--;
				emit onBombHit();
				bombHit(m_bomb,building->position().x(),Building::BUILD_BASE_LOCATION-(building->height()));
				m_bomb = NULL;
			}
			else if (m_bomb->position().y()>=Building::BUILD_BASE_LOCATION)
			{
				bombHit(m_bomb,(int)m_bomb->position().x(),Building::BUILD_BASE_LOCATION);
				m_bomb = NULL;
			}
		}
		
		if (m_plane->state()==Explodable::Moving && /*m_plane->position().x()>Plane::PLANE_MAX_POSITION_X &&*/ m_buildingBlocks==0)
		{
			emit levelCleared();
		}
	}

}

void BomberBoard::bombHit(Bomb *bomb, qreal moveBombToX, qreal moveBombToY)
{
    playSound(QString("explode.ogg"));
	bomb->setPosition(moveBombToX, moveBombToY);
	bomb->setState(Bomb::Exploding);
	m_explodingBombs.enqueue(bomb);
	QTimer::singleShot(BOMB_EXPLODE_TIME, this, SLOT(bombExploded()));
}

void BomberBoard::bombExploded()
{
	Bomb *bomb = m_explodingBombs.dequeue();
	bomb->hide();	
	delete bomb;
}

void BomberBoard::planeExploded()
{
	m_plane->setState(Plane::Exploded);	
emit 				onPlaneCrash();	
}

void BomberBoard::crashed()
{
	QPointF pos = m_plane->position();
	m_plane->setPosition(pos.x()+1, pos.y());
	m_plane->setState(Plane::Exploding);
	playSound(QString("explode.ogg"));
	QTimer::singleShot(PLANE_EXPLODE_TIME, this, SLOT(planeExploded()));
}

void BomberBoard::clear()
{
	foreach(Building* building, m_buildings)
	{
		delete building;
	}

	m_buildings.clear();

	if (m_bomb!=NULL)
	{
		delete m_bomb;
		m_bomb=NULL;
	}

	resetPlane();
}

QPoint BomberBoard::mapPosition(const QPointF& pos) const
{
	return QPoint( static_cast<int>(m_tileSize.width() * pos.x() ),
			static_cast<int>(m_tileSize.height() * pos.y() ) );
}

QPointF BomberBoard::unmapPosition(const QPoint& pos) const
{
	return QPointF( 1.0 * pos.x() / m_tileSize.width(), 1.0 * pos.y()
			/ m_tileSize.height() );
}
