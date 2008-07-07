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
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <KDE/KLocale>

static const char description[] =
    I18N_NOOP("A KDE 4 Application");

static const char version[] = "0.1";

int main(int argc, char **argv)
{
    KAboutData about("bomber", 0, ki18n("Bomber"), version, ki18n(description),
                     KAboutData::License_GPL, ki18n("(C) 2007 John-Paul Stanford"), KLocalizedString(), 0, "jp@stanwood.org.uk");
    about.addAuthor(ki18n("John-Paul Stanford"), KLocalizedString(), "jp@stanwood.org.uk" );
    about.addCredit(ki18n("sandyrb"),ki18n("Explode sound sample"),"","http://www.freesound.org/samplesViewSingle.php?id=35643");
    KCmdLineArgs::init(argc, argv, &about);
    
    KApplication app;
    KGlobal::locale()->insertCatalog("libkdegames");    

    // see if we are starting with session management
    if (app.isSessionRestored())
    {
        RESTORE(Bomber);
    }
    else
    {
    	Bomber *widget = new Bomber;
    	widget->show();        
    }

    return app.exec();
}
