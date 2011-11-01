#ifndef COLLECTION_H
#define COLLECTION_H
#include <qstringlist.h>

struct Track {
    qint64    duration;
    QString   title;
    QString   artist;
    QString   album;
    bool      local;
    QString   url;
    int       track;
    const bool operator<(const Track &other);
};



struct Playlist {
    QString name;
    QList<Track*> tracks;
};

class Collection {
public:
    Collection();
    const QStringList albums();
    const QStringList artists();
    const QStringList playlists();
    
    const QList<Track*> tracksForArtist(QString artist);
    const QList<Track*> tracksForAlbum(QString album);
    const QList<Track*> tracksForSearch(QString query);
    const QList<Track*> tracksForPlaylist(QString playlist);
    
    void addTrack(Track *track);
    
private:
    QList<Track*> m_tracks;
    QList<Playlist*> m_playlists;
};

#endif//COLLECTION_H