#include "localscanner.h"
#include "trackmodel.h"

#include <QDebug>


#include <taglib/fileref.h>
#include <taglib/tag.h>

LocalScanner::LocalScanner(QString path, TrackModel* model, QObject *parent) : QObject(parent),
    m_path(path),
    m_model(model)
{
    m_nameFilter << "*.mp3";
}

void LocalScanner::rescan()
{
    qWarning() << "lollert";
    scanDir(QDir(m_path));
}

void LocalScanner::scanDir(QDir dir)
{
    dir.setNameFilters(m_nameFilter);
    dir.setFilter(QDir::Files);
    
    if (dir.count() > 0) {
        Q_FOREACH(QString filename, dir.entryList()) {
            TagLib::FileRef file(QString(dir.path() + filename).toLocal8Bit());
            
            if (file.isNull() || !file.audioProperties() || !file.tag())
                continue;
            
            QString title = fromTString(file.tag()->title());
            QString artist= fromTString(file.tag()->artist());
            QString album = fromTString(file.tag()->album());
            int duration = file.audioProperties()->length();
            if (title.isEmpty() || artist.isEmpty())
                continue;
            
            Track *track = new Track;
            track->title = title;
            track->artist = artist;
            track->album = album;
            track->duration = duration;
            m_model->addTrack(track);
            qWarning() << title << artist << album << duration;
        }
    }
    
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::AllDirs | QDir::Executable);
                                                                      // QDir::AllDirs ignores name filter

    foreach(const QFileInfo &subdir, dir.entryInfoList()) {
        scanDir(QDir(subdir.filePath())); // Recursively scan
    }
}

QString LocalScanner::fromTString(TagLib::String string)
{
    return QString::fromStdWString(string.toWString());
}
