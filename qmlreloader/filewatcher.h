#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include <QObject>
#include <QStringList>

namespace QMLreLoader {

class FileWatcherPrivate;

class FileWatcher : public QObject
{
    Q_OBJECT

public:
    explicit FileWatcher(QObject *parent = nullptr);
    ~FileWatcher();

    QString watchDirectory() const;
    void setWatchDirectory(const QString &watchDirectory);

signals:
    void changedFiles(const QStringList &changedFiles);

private:
    FileWatcherPrivate *d;
};

}

#endif // FILEWATCHER_H
