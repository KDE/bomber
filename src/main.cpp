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

// Qt
#include <QApplication>
#include <QCommandLineParser>

// KDE
#include <KAboutData>
#include <KCrash>
#include <KDBusService>
#include <KLocalizedString>
#include <Kdelibs4ConfigMigrator>

// Bomber
#include "bomber.h"

static const char description[] = I18N_NOOP("Arcade bombing game");

static const char version[] = "0.4";

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    Kdelibs4ConfigMigrator migrate(QStringLiteral("bomber"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("bomberrc"));
    migrate.setUiFiles(QStringList() << QStringLiteral("bomberui.rc"));
    migrate.migrate();

    KLocalizedString::setApplicationDomain("bomber");

    KAboutData about(QStringLiteral("bomber"), i18n("Bomber"), QLatin1String(version), i18n(description),
                     KAboutLicense::GPL, i18n("(C) 2007 John-Paul Stanford"),
                     QString(), QStringLiteral("jp@stanwood.org.uk"));
    about.addAuthor(i18n("John-Paul Stanford"), QString(),
                    QStringLiteral("jp@stanwood.org.uk"));
    about.addAuthor(i18n("Mehmet Emre"), i18n("Porting to QGraphicsView."),
                    QStringLiteral("maemre2@gmail.com"));
    QCommandLineParser parser;
    KAboutData::setApplicationData(about);
    KCrash::initialize();
    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);
    KDBusService service;

    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("bomber")));

    // see if we are starting with session management
    if (app.isSessionRestored()) {
        RESTORE(Bomber);
    } else {
        auto widget = new Bomber;
        widget->setMinimumSize(320, 200);
        widget->show();
        widget->readSettings();
    }

    return app.exec();
}
