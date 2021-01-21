/*
    SPDX-FileCopyrightText: 2007-2008 John-Paul Stanford <jp@stanwood.org.uk>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
    // Fixes blurry icons with fractional scaling
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
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
        kRestoreMainWindows<Bomber>();
    } else {
        auto widget = new Bomber;
        widget->setMinimumSize(320, 200);
        widget->show();
        widget->readSettings();
    }

    return app.exec();
}
