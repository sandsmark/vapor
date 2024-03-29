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
#include "settings.h"

#include <QtGui/QDropEvent>
#include <QtGui/QPainter>
#include <QtGui/QPrinter>

#include <KConfigDialog>
#include <KStatusBar>

#include <KAction>
#include <KActionCollection>
#include <KStandardAction>

#include <KLocale>

#include <QTreeView>
#include <QListWidget>
#include <QDockWidget>

#include <mainwidget.h>
#include "collection.h"
#include "localscanner.h"

Vapor::Vapor()
    : KXmlGuiWindow(),
    m_collection(new Collection(this)),
    m_mainWidget(new MainWidget)
{
    // accept dnd
    setAcceptDrops(true);

    // tell the KXmlGuiWindow that this is indeed the main widget
    setCentralWidget(m_mainWidget);

    // then, setup our actions
    setupActions();

    // add a status bar
    statusBar()->show();

    // a call to KXmlGuiWindow::setupGUI() populates the GUI
    // with actions, using KXMLGUI.
    // It also applies the saved mainwindow settings, if any, and ask the
    // mainwindow to automatically save settings if changed: window size,
    // toolbar position, icon size, etc.
    setupGUI();
    
    
    connect(m_collection, SIGNAL(playlistsUpdated()), this, SLOT(playlistsUpdated()));
    
    LocalScanner *scanner = new LocalScanner("/home/sandsmark/musikk/Celldweller/Tragedy", m_collection, this);
    scanner->rescan();
    
    // Set up playlist list widget
    {
        m_playlistView = new QListWidget;
        QDockWidget *playlistDock = new QDockWidget(i18n("Playlists"), this);
        playlistDock->setObjectName("playlists");
        playlistDock->setWidget(m_playlistView);
        addDockWidget(Qt::LeftDockWidgetArea, playlistDock);
        connect(m_playlistView, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(playlistClicked(QListWidgetItem*)));
    }
        
}

Vapor::~Vapor()
{
}

void Vapor::setupActions()
{
    KStandardAction::quit(qApp, SLOT(closeAllWindows()), actionCollection());

    KStandardAction::preferences(this, SLOT(optionsPreferences()), actionCollection());

    // custom menu and menu item - the slot is in the class VaporView
    /*KAction *custom = new KAction(KIcon("colorize"), i18n("Swi&tch Colors"), this);
    actionCollection()->addAction( QLatin1String("switch_action"), custom );
    connect(custom, SIGNAL(triggered(bool)), m_view, SLOT(switchColors()));*/
}

void Vapor::optionsPreferences()
{
    // The preference dialog is derived from prefs_base.ui
    //
    // compare the names of the widgets in the .ui file
    // to the names of the variables in the .kcfg file
    //avoid to have 2 dialogs shown
    if ( KConfigDialog::showDialog( "settings" ) )  {
        return;
    }
    KConfigDialog *dialog = new KConfigDialog(this, "settings", Settings::self());
    QWidget *generalSettingsDlg = new QWidget;
    ui_prefs_base.setupUi(generalSettingsDlg);
    dialog->addPage(generalSettingsDlg, i18n("General"), "package_setting");
    //connect(dialog, SIGNAL(settingsChanged(QString)), m_view, SLOT(settingsChanged()));
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    dialog->show();
}

void Vapor::playlistsUpdated()
{
    m_playlistView->addItems(m_collection->playlists());
}

void Vapor::playlistClicked(QListWidgetItem* item)
{
    m_mainWidget->setCurrentPlayingCollection(m_collection->tracksForPlaylist(item->text()));
}


#include "vapor.moc"
