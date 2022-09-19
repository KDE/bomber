/*
    SPDX-FileCopyrightText: 2007-2008 John-Paul Stanford <jp@stanwood.org.uk>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Qt
#include <QApplication>
#include <QCommandLineParser>

// KF
#include <KAboutData>
#include <KCrash>
#include <KDBusService>
#include <KLocalizedString>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <Kdelibs4ConfigMigrator>
#endif

// Bomber
#include "bomber.h"
#include "bomber_version.h"

int main(int argc, char ** argv)
{
    // Fixes blurry icons with fractional scaling
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication app(argc, argv);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Kdelibs4ConfigMigrator migrate(QStringLiteral("bomber"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("bomberrc"));
    migrate.setUiFiles(QStringList() << QStringLiteral("bomberui.rc"));
    migrate.migrate();
#endif
    KLocalizedString::setApplicationDomain("bomber");

    KAboutData about(QStringLiteral("bomber"), i18n("Bomber"),
                     QStringLiteral(BOMBER_VERSION_STRING),
                     i18n("Arcade bombing game"),
                     KAboutLicense::GPL, i18n("(C) 2007 John-Paul Stanford"),
                     QString(),
                     QStringLiteral("https://apps.kde.org//bomber"));
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
