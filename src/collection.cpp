#include "collection.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegExp>
#include <QDebug>
#include <QUrl>

Collection::Collection(QObject *parent) : QObject(parent)
{
    QNetworkAccessManager *accessManager = new QNetworkAccessManager(this);
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(gotPlaylists(QNetworkReply*)));
    accessManager->get(QNetworkRequest(QUrl("http://music:hahafuckyou@music.mts.ms/cgi-bin/get-playlists.pl")));
}

void Collection::gotPlaylists(QNetworkReply* reply)
{
    QString playlistsList = reply->readAll();
    QRegExp playlistMatcher("\\[(\\d+),\"(\\w+)\"\\]");
    
    int pos = 0;
    Playlist *playlist;
    while ((pos = playlistMatcher.indexIn(playlistsList, pos)) != -1) {
        playlist = new Playlist;
        playlist->id = playlistMatcher.cap(1).toInt();
        playlist->name = playlistMatcher.cap(2);
        m_playlists.append(playlist);
        pos += playlistMatcher.matchedLength();
    }
    
    emit playlistsUpdated();
    
    reply->manager()->deleteLater();
    reply->deleteLater();
}


void Collection::addTrack(Track* track)
{
    m_tracks.append(track);
}

const QStringList Collection::albums()
{
    QStringList albums;
    Q_FOREACH(Track *track, m_tracks) {
        albums << track->album;
    }
    return albums;
}

const QStringList Collection::artists()
{
    QStringList artists;
    Q_FOREACH(Track *track, m_tracks) {
        artists << track->artist;
    }
    return artists;
}

const QStringList Collection::playlists()
{
    QStringList playlists;
    Q_FOREACH(Playlist *playlist, m_playlists) {
        playlists << playlist->name;
    }
    return playlists;
}

const QList< Track* > Collection::tracksForAlbum(QString album)
{
    QList <Track*> tracks;
    Q_FOREACH(Track *track, m_tracks) {
        if (track->album == album)
            tracks << track;
    }
    qSort(tracks);
    return tracks;
}

const QList< Track* > Collection::tracksForArtist(QString artist)
{
    QList <Track*> tracks;
    Q_FOREACH(Track *track, m_tracks) {
        if (track->artist == artist)
            tracks << track;
    }
    qSort(tracks);
    return tracks;
}

const QList< Track* > Collection::tracksForSearch(QString query)
{
    QList <Track*> tracks;
    Q_FOREACH(Track *track, m_tracks) {
        if (track->artist.contains(query) || track->album.contains(query) || track->title.contains(query))
            tracks << track;
    }
    return tracks;
}

const QList< Track* > Collection::tracksForPlaylist(QString playlistName)
{
    Q_FOREACH(Playlist *playlist, m_playlists) {
        if (playlist->name == playlistName)
            return playlist->tracks;
    }
    return QList<Track*>();
}

bool Track::operator<(const Track& other)
{
    if (artist != other.artist) {
        return artist < other.artist;
    } else if (album != other.album) {
        return album < other.album;
    } else {
        return track < other.track;
    }
}
