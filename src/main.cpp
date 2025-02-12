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

#define HAVE_KICONTHEME __has_include(<KIconTheme>)
#if HAVE_KICONTHEME
#include <KIconTheme>
#endif

#define HAVE_STYLE_MANAGER __has_include(<KStyleManager>)
#if HAVE_STYLE_MANAGER
#include <KStyleManager>
#endif

// Bomber
#include "bomber.h"
#include "bomber_version.h"

int main(int argc, char ** argv)
{
#if HAVE_KICONTHEME
    KIconTheme::initTheme();
#endif

    QApplication app(argc, argv);
#if HAVE_STYLE_MANAGER
    KStyleManager::initStyle();
#else // !HAVE_STYLE_MANAGER
#if defined(Q_OS_MACOS) || defined(Q_OS_WIN)
    QApplication::setStyle(QStringLiteral("breeze"));
#endif // defined(Q_OS_MACOS) || defined(Q_OS_WIN)
#endif // HAVE_STYLE_MANAGER


    KLocalizedString::setApplicationDomain(QByteArrayLiteral("bomber"));

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
    KAboutData::setApplicationData(about);

    KCrash::initialize();

    QCommandLineParser parser;
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
