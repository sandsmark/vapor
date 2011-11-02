#ifndef COLLECTION_H
#define COLLECTION_H
#include <qstringlist.h>
#include <QObject>

struct Track {
    qint64    duration;
    QString   title;
    QString   artist;
    QString   album;
    bool      local;
    QString   url;
    int       track;
    bool operator<(const Track &other);
    int id;
};


class QNetworkReply;

struct Playlist {
    QString name;
    QList<Track*> tracks;
    int id;
};

class Collection : public QObject {
    Q_OBJECT
    
public:
    Collection(QObject *parent);
    const QStringList albums();
    const QStringList artists();
    const QStringList playlists();
    
    const QList<Track*> tracksForArtist(QString artist);
    const QList<Track*> tracksForAlbum(QString album);
    const QList<Track*> tracksForSearch(QString query);
    const QList<Track*> tracksForPlaylist(QString playlist);
    
    void addTrack(Track *track);

signals:
    void playlistsUpdated();
    
private slots:
    void gotPlaylists(QNetworkReply*);
    
private:
    QList<Track*> m_tracks;
    QList<Playlist*> m_playlists;
};

#endif//COLLECTION_H