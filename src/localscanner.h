#ifndef LOCALSCANNER_H
#define LOCALSCANNER_H
#include <QDir>
#include <QStringList>
#include <taglib/audioproperties.h>

class Collection;
namespace TagLib {
    class String;
}

class LocalScanner : QObject
{
    Q_OBJECT
public:
    LocalScanner(QString path, Collection* model, QObject* parent=0);
    
public slots:
    void rescan();
    
private:
    static QString fromTString(TagLib::String);
    
    void scanDir(QDir dir);
    QString m_path;
    Collection *m_collection;
    QStringList m_nameFilter;
};

#endif//LOCALSCANNER_H