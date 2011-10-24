#ifndef LOCALSCANNER_H
#define LOCALSCANNER_H
#include <QDir>
#include <QStringList>
#include <taglib/audioproperties.h>

class TrackModel;
namespace TagLib {
    class String;
}

class LocalScanner : QObject
{
    Q_OBJECT
public:
    LocalScanner(QString path, TrackModel* model, QObject* parent=0);
    
public slots:
    void rescan();
    
private:
    static QString fromTString(TagLib::String);
    
    void scanDir(QDir dir);
    QString m_path;
    TrackModel *m_model;
    QStringList m_nameFilter;
};

#endif//LOCALSCANNER_H