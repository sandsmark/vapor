#include "collection.h"

Collection::Collection()
{

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

const bool Track::operator<(const Track& other)
{
    if (artist != other.artist) {
        return artist < other.artist;
    } else if (album != other.album) {
        return album < other.album;
    } else {
        return track < other.track;
    }
}
