/***************************************************************************
 *   Copyright (C) %{CURRENT_YEAR} by %{AUTHOR} <%{EMAIL}>                            *
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

#ifndef VAPOR_H
#define VAPOR_H


#include <KXmlGuiWindow>

#include "ui_prefs_base.h"

class KToggleAction;
class KUrl;

class QListWidget;
class QListWidgetItem;
class QLineEdit;

class Collection;
class MainWidget;

/**
 * This class serves as the main window for Vapor.  It handles the
 * menus, toolbars and status bars, as well as the various dock widgets.
 *
 * @short Main window class
 * @author Martin Sandsmark <martin.sandsmark@kde.org>
 * @version 0.1
 */
class Vapor : public KXmlGuiWindow
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    Vapor();

    /**
     * Default Destructor
     */
    virtual ~Vapor();

private slots:
    void optionsPreferences();
    void playlistsUpdated();
    void playlistClicked(QListWidgetItem*);

private:
    void setupActions();

private:
    Ui::prefs_base ui_prefs_base ;

    KToggleAction *m_toolbarAction;
    KToggleAction *m_statusbarAction;
    
    Collection *m_collection;
    
    QListWidget *m_playlistView;
    QLineEdit *m_searchEdit;
    MainWidget *m_mainWidget;
};

#endif // _VAPOR_H_
