/*
 * This file is part of Spokify.
 * Copyright (C) 2010 Rafael Fernández López <ereslibre@kde.org>
 *
 * Spokify is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Spokify is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Spokify.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwidget.h"
#include "slider.h"
#include "trackview.h"
#include "trackmodel.h"
#include "playpausebutton.h"
#include "trackviewdelegate.h"
#include "blockanalyzer.h"

#include <math.h>

#include <QtGui/QLabel>
#include <QtGui/QTabWidget>
#include <QtGui/QBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QSortFilterProxyModel>
#include <QTime>

#include <KIcon>
#include <KDebug>
#include <KLocale>
#include <KLineEdit>
#include <KPushButton>
#include <kdeversion.h>
#include "localscanner.h"


MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , m_state(Stopped)
    , m_currentCollection(0)
    , m_currentPlayingCollection(0)
{
    m_filter = new KLineEdit(this);
    m_filter->setClickMessage(i18n("Filter by title, artist or album"));
    m_filter->setClearButtonShown(true);

    m_trackView = new TrackView(this);
    m_trackView->verticalHeader()->hide();
    m_trackView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_trackView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_trackView->setAlternatingRowColors(true);
    m_trackView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_trackView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_trackView->setShowGrid(false);
    m_trackView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_trackView->setItemDelegate(new TrackViewDelegate(m_trackView));
    m_trackView->setDragEnabled(true);
    m_trackView->setDropIndicatorShown(true);
    m_trackView->setDragDropMode(QAbstractItemView::DragDrop);
    m_trackView->setSortingEnabled(true);
    
    TrackModel *trackmodel = new TrackModel;
    LocalScanner scanner("/home/sandsmark/musikk/Celldweller", trackmodel, this);
    m_trackView->setModel(trackmodel);
    
    scanner.rescan();

    m_playPauseButton = new PlayPauseButton(this);

    m_slider = new Slider(this);
    m_currTotalTime = new QLabel(this);

    m_analyzer = new BlockAnalyzer(this);

    connect(m_trackView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(trackRequested(QModelIndex)));
    connect(m_playPauseButton, SIGNAL(play()), this, SLOT(playSlot()));
    connect(m_playPauseButton, SIGNAL(pause()), this, SLOT(pauseSlot()));
    connect(m_slider, SIGNAL(maximumReached()), this, SIGNAL(currentTrackFinished()));
    connect(m_slider, SIGNAL(seek(float)), this, SLOT(sliderSeekSlot(float)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_filter);
    layout->addWidget(m_trackView);
    QGridLayout *hLayout = new QGridLayout;
    hLayout->addWidget(m_playPauseButton, 0, 0);
    hLayout->addWidget(m_analyzer, 0, 1);
    hLayout->addWidget(m_currTotalTime, 0, 2);
    m_analyzer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    hLayout->addWidget(m_slider, 1, 0, 1, 3);
    //hLayout->setStretchFactor(m_slider, 1);
    layout->addLayout(hLayout);
    setLayout(layout);
}

MainWidget::~MainWidget()
{
}

void MainWidget::loggedIn()
{
    m_filter->setEnabled(true);
    m_trackView->setEnabled(true);
    m_slider->setEnabled(true);
    m_currTotalTime->setEnabled(true);
}

void MainWidget::loggedOut()
{
    setState(Stopped);
    m_filter->setEnabled(false);
    m_filter->clear();
    m_trackView->setEnabled(false);
    m_trackView->setModel(0);
    m_slider->setEnabled(false);
    m_slider->setValue(0);
    m_slider->setCacheValue(0);
    m_currTotalTime->setText(i18n("<b>00:00</b><br/><b>00:00</b>"));
    m_currTotalTime->setEnabled(false);
    m_playPauseButton->setEnabled(false);
}

void MainWidget::clearFilter()
{
    if (m_currentCollection) {
        disconnect(m_filter, SIGNAL(textChanged(QString)), m_currentCollection->proxyModel, SLOT(setFilterFixedString(QString)));
    }
    m_filter->clear();
}

MainWidget::Collection &MainWidget::collection(sp_playlist *playlist)
{
    if (m_trackModelPlaylistCache.contains(playlist)) {
        Collection &res = m_trackModelPlaylistCache[playlist];
        m_trackView->setModel(res.proxyModel);
        m_trackView->sortByColumn(res.proxyModel->sortColumn(), res.proxyModel->sortOrder());

        if (m_currentCollection) {
            disconnect(m_filter, SIGNAL(textChanged(QString)), m_currentCollection->proxyModel, SLOT(setFilterFixedString(QString)));
            disconnect(m_filter, SIGNAL(textChanged(QString)), this, SLOT(layoutChangedSlot()));
        }

        m_filter->setText(res.proxyModel->filterRegExp().pattern());
        connect(m_filter, SIGNAL(textChanged(QString)), res.proxyModel, SLOT(setFilterFixedString(QString)));
        connect(m_filter, SIGNAL(textChanged(QString)), this, SLOT(layoutChangedSlot()));
        connect(m_trackView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChangedSlot(QItemSelection)));

        m_currentCollection = &m_trackModelPlaylistCache[playlist];

        m_trackView->highlightTrack(res.currentTrack);
        m_trackView->setFocus();

        return res;
    }

    Collection c;
    c.trackModel = new TrackModel(m_trackView);
    c.proxyModel = new QSortFilterProxyModel(m_trackView);
    c.proxyModel->setFilterKeyColumn(-1);
    c.proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    c.proxyModel->setSortRole(TrackModel::SortRole);
    c.proxyModel->setSourceModel(c.trackModel);
    c.needsToBeFilled = true;
    m_trackModelPlaylistCache[playlist] = c;

    m_trackView->setModel(c.proxyModel);
    m_trackView->sortByColumn(c.proxyModel->sortColumn(), c.proxyModel->sortOrder());

    if (m_currentCollection) {
        disconnect(m_filter, SIGNAL(textChanged(QString)), m_currentCollection->proxyModel, SLOT(setFilterFixedString(QString)));
    }

    m_filter->clear();
    connect(m_filter, SIGNAL(textChanged(QString)), c.proxyModel, SLOT(setFilterFixedString(QString)));
    connect(m_filter, SIGNAL(textChanged(QString)), this, SLOT(layoutChangedSlot()));
    connect(m_trackView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChangedSlot(QItemSelection)));

    m_currentCollection = &m_trackModelPlaylistCache[playlist];

    m_trackView->highlightTrack(c.currentTrack);
    m_trackView->setFocus();
    m_trackView->resizeRowsToContents();

    return m_trackModelPlaylistCache[playlist];
}

MainWidget::Collection &MainWidget::collection(sp_search *search)
{
    if (m_trackModelSearchCache.contains(search)) {
        Collection &res = m_trackModelSearchCache[search];
        m_trackView->setModel(res.proxyModel);
        m_trackView->sortByColumn(res.proxyModel->sortColumn(), res.proxyModel->sortOrder());

        if (m_currentCollection) {
            disconnect(m_filter, SIGNAL(textChanged(QString)), m_currentCollection->proxyModel, SLOT(setFilterFixedString(QString)));
            disconnect(m_filter, SIGNAL(textChanged(QString)), this, SLOT(layoutChangedSlot()));
        }

        m_filter->setText(res.proxyModel->filterRegExp().pattern());
        connect(m_filter, SIGNAL(textChanged(QString)), res.proxyModel, SLOT(setFilterFixedString(QString)));
        connect(m_filter, SIGNAL(textChanged(QString)), this, SLOT(layoutChangedSlot()));
        connect(m_trackView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChangedSlot(QItemSelection)));

        m_currentCollection = &m_trackModelSearchCache[search];

        m_trackView->highlightTrack(res.currentTrack);
        m_trackView->setFocus();

        return res;
    }

    Collection c;
    c.trackModel = new TrackModel(m_trackView);
    c.proxyModel = new QSortFilterProxyModel(m_trackView);
    c.proxyModel->setFilterKeyColumn(-1);
    c.proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    c.proxyModel->setSortRole(TrackModel::SortRole);
    c.proxyModel->setSourceModel(c.trackModel);
    c.needsToBeFilled = true;
    m_trackModelSearchCache[search] = c;

    m_trackView->setModel(c.proxyModel);
    m_trackView->sortByColumn(c.proxyModel->sortColumn(), c.proxyModel->sortOrder());

    if (m_currentCollection) {
        disconnect(m_filter, SIGNAL(textChanged(QString)), m_currentCollection->proxyModel, SLOT(setFilterFixedString(QString)));
    }

    m_filter->clear();
    connect(m_filter, SIGNAL(textChanged(QString)), c.proxyModel, SLOT(setFilterFixedString(QString)));
    connect(m_filter, SIGNAL(textChanged(QString)), this, SLOT(layoutChangedSlot()));
    connect(m_trackView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChangedSlot(QItemSelection)));

    m_currentCollection = &m_trackModelSearchCache[search];

    m_trackView->highlightTrack(c.currentTrack);
    m_trackView->setFocus();

    return m_trackModelSearchCache[search];
}

MainWidget::Collection *MainWidget::currentPlayingCollection() const
{
    if (m_currentPlayingCollection) {
        return m_currentPlayingCollection;
    }
    return 0;
}

void MainWidget::setCurrentPlayingCollection(Collection &collection)
{
    m_currentPlayingCollection = &collection;
}

TrackView *MainWidget::trackView() const
{
    return m_trackView;
}

void MainWidget::setState(State state)
{
    if (state != Playing) {
        emit pausedOrStopped();
    }
    m_state = state;
    m_playPauseButton->setIsPlaying(state == Playing);
}

MainWidget::State MainWidget::state() const
{
    return m_state;
}

void MainWidget::highlightCurrentTrack(Focus focus)
{
    if (!m_currentPlayingCollection) {
        return;
    }
    // Here we are sure that the current playing collection is the one being shown
    m_trackView->highlightTrack(m_currentPlayingCollection->currentTrack);
    if (focus == SetFocus) {
        m_trackView->setFocus();
    }
}

void MainWidget::setTotalTrackTime(int totalTrackTime)
{
    m_slider->setRange(0, totalTrackTime * (quint64) 44100);
    m_slider->setValue(0);
    m_slider->setCacheValue(0);

    m_totalTrackTime = totalTrackTime;

    QTime time;
    time = time.addMSecs(totalTrackTime);
#if KDE_IS_VERSION(4,5,66)
    m_currTotalTime->setText(i18n("<b>00:00</b><br/><b>%1</b>",
          KGlobal::locale()->formatLocaleTime(time, KLocale::TimeDuration | KLocale::TimeFoldHours)));
#else
    m_currTotalTime->setText(i18n("<b>00:00</b><br/><b>%1</b>",
          KGlobal::locale()->formatTime(time, true, true)));
#endif
}

void MainWidget::advanceCurrentTrackTime(const Chunk &chunk)
{
    if (chunk.m_dataFrames == -1) {
        m_slider->setValue(m_slider->maximum());
        QTime val;
        QTime total;
        val = total.addMSecs(m_totalTrackTime);
        total = val;
#if KDE_IS_VERSION(4,5,66)
        m_currTotalTime->setText(i18nc("Current time position, Total length","<b>%1</b><br/><b>%2</b>",
                        KGlobal::locale()->formatLocaleTime(val, KLocale::TimeDuration | KLocale::TimeFoldHours),
                        KGlobal::locale()->formatLocaleTime(total, KLocale::TimeDuration | KLocale::TimeFoldHours)));
#else
        m_currTotalTime->setText(i18nc("Current time position, Total length","<b>%1</b><br/><b>%2</b>",
                        KGlobal::locale()->formatTime(val, true, true),
                        KGlobal::locale()->formatTime(total, true, true)));
#endif
        return;
    }

    m_slider->setValue(m_slider->value() + chunk.m_dataFrames * 1000);
    const int curpos = (quint64) ((m_slider->value() / (chunk.m_rate * 1000)));
    const int totpos = (quint64) ((m_slider->maximum() / (chunk.m_rate * 1000)));

    QTime val;
    QTime total;
    val = val.addSecs(curpos);
    total = total.addSecs(totpos);
#if KDE_IS_VERSION(4,5,66)
    m_currTotalTime->setText(i18nc("Current time position, Total length","<b>%1</b><br/><b>%2</b>",
                    KGlobal::locale()->formatLocaleTime(val, KLocale::TimeDuration | KLocale::TimeFoldHours),
                    KGlobal::locale()->formatLocaleTime(total, KLocale::TimeDuration | KLocale::TimeFoldHours)));
#else
    m_currTotalTime->setText(i18nc("Current time position, Total length","<b>%1</b><br/><b>%2</b>",
                    KGlobal::locale()->formatTime(val, true, true),
                    KGlobal::locale()->formatTime(total, true, true)));
#endif
}

void MainWidget::advanceCurrentCacheTrackTime(const Chunk &chunk)
{
    if (chunk.m_dataFrames != -1) {
        m_slider->setCacheValue(m_slider->cacheValue() + chunk.m_dataFrames * 1000);
    } else {
        m_slider->setCacheValue(m_slider->maximum());
    }
}

void MainWidget::playSlot()
{
    if (m_state == Stopped) {
        if (m_trackView->currentIndex().isValid()) {
            trackRequested(m_trackView->currentIndex());
        }
    } else {
        m_state = Playing;
        emit resume();
    }
}

void MainWidget::pauseSlot()
{
    m_state = Paused;
    emit pausedOrStopped();
}

void MainWidget::trackRequested(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    m_state = Playing;
    m_playPauseButton->setIsPlaying(true);
    m_currentPlayingCollection = m_currentCollection;
    m_currentPlayingCollection->currentTrack = index.data(TrackModel::SpotifyNativeTrackRole).value<Track*>();
    emit play(index);
}

void MainWidget::selectionChangedSlot(const QItemSelection &selection)
{
    m_playPauseButton->setEnabled(!selection.isEmpty());
}

void MainWidget::layoutChangedSlot()
{
    highlightCurrentTrack(DoNotSetFocus);
}

void MainWidget::sliderSeekSlot(float position)
{
    emit seekPosition(position * m_currentPlayingCollection->currentTrack->duration);
}

void MainWidget::togglePlayPauseSlot()
{
    if (m_state == Playing) {
        pauseSlot();
        m_playPauseButton->setIsPlaying(false);
    } else {
        playSlot();
    }
}

void MainWidget::updateAnalyzer(const Chunk &chunk)
{
    m_analyzer->drawFrame(chunk);
}
