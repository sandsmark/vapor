/***************************************************************************
 *   Copyright (C) 2011 by Martin T. Sandsmark <martin.sandsmark@kde.org>  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "vapor.h"
#include <KDE/KApplication>
#include <KDE/KAboutData>
#include <KDE/KCmdLineArgs>
#include <KDE/KLocale>

static const char description[] =
    I18N_NOOP("A KDE 4 cloud music player");

static const char version[] = "0.1";

int main(int argc, char **argv)
{
    KAboutData about("vapor", 0, ki18n("Vapor"), version, ki18n(description),
                     KAboutData::License_GPL, ki18n("(C) 2011 Martin T. Sandsmark"), KLocalizedString(), 0, "martin.sandsmark@kde.org");
    about.addAuthor( ki18n("Martin T. Sandsmark"), KLocalizedString(), "martin.sandsmark@kde.org" );
    about.addCredit(ki18n("Rafael Fernández López"), ki18n("Progress bar, play/pause button, inspiration"), "ereslibre@kde.org");
    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    options.add("+[URL]", ki18n( "Document to open" ));
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;

    Vapor *widget = new Vapor;

    // see if we are starting with session management
    if (app.isSessionRestored())
    {
        RESTORE(Vapor);
    }
    else
    {
        widget->show();
    }

    return app.exec();
}
