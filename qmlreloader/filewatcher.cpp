#include "filewatcher.h"

#include <QFileSystemWatcher>
#include <QDirIterator>
#include <QTimer>
#include <QDir>
#include <QUrl>

#include <QDebug>

#define WAIT_FOR_CHANGES_TIMEOUT 120

namespace QMLreLoader {

class FileWatcherPrivate
{
public:
    FileWatcherPrivate()
        : fsWatcher(nullptr)
        , directoryChangesTimeout(nullptr)
        , fileChangesTimeout(nullptr)
    {
    }

    ~FileWatcherPrivate()
    {
        if(fsWatcher) {
            delete fsWatcher;
            fsWatcher = nullptr;
        }

        if(directoryChangesTimeout) {
            delete directoryChangesTimeout;
            directoryChangesTimeout = nullptr;
        }

        if(fileChangesTimeout) {
            delete fileChangesTimeout;
            fileChangesTimeout = nullptr;
        }
    }

    QString watchDirectory;

    QFileSystemWatcher *fsWatcher;
    QTimer *directoryChangesTimeout, *fileChangesTimeout;

    QStringList changedDirs, changedFiles;
    void scanDirectory(const QString &directory);
};

FileWatcher::FileWatcher(QObject *parent)
    : QObject(parent)
    , d(new FileWatcherPrivate())
{
    d->fsWatcher = new QFileSystemWatcher(this);

    connect(d->fsWatcher, &QFileSystemWatcher::directoryChanged, this, [&](const QString &dirPath){
        if(!d->changedDirs.contains(dirPath))
            d->changedDirs.append(dirPath);
        d->directoryChangesTimeout->start();
    });

    connect(d->fsWatcher, &QFileSystemWatcher::fileChanged, this, [&](const QString &filePath){
        if(!d->changedFiles.contains(filePath))
            d->changedFiles.append(filePath);
        d->fileChangesTimeout->start();
    });

    /// Changed Directories (rescan)
    d->directoryChangesTimeout = new QTimer(this);
    connect(d->directoryChangesTimeout, &QTimer::timeout, this, [&]() {
        d->changedDirs.sort();
        for(auto &directory : d->changedDirs)
            d->scanDirectory(directory);
    });
    d->directoryChangesTimeout->setInterval(WAIT_FOR_CHANGES_TIMEOUT);
    d->directoryChangesTimeout->setSingleShot(true);

    /// Changed Files
    d->fileChangesTimeout = new QTimer(this);
    connect(d->fileChangesTimeout, &QTimer::timeout, this, [&]() {
        d->changedFiles.sort();
        emit changedFiles(d->changedFiles);
        d->changedFiles.clear();
    });
    d->fileChangesTimeout->setInterval(WAIT_FOR_CHANGES_TIMEOUT);
    d->fileChangesTimeout->setSingleShot(true);
}

FileWatcher::~FileWatcher()
{
    delete d;
    d = nullptr;
}

QString FileWatcher::watchDirectory() const
{
    return d->watchDirectory;
}

void FileWatcher::setWatchDirectory(const QString &watchDirectory)
{
    d->watchDirectory = watchDirectory;
    if(!d->fsWatcher->files().isEmpty())
        d->fsWatcher->removePaths(d->fsWatcher->files());
    if(!d->fsWatcher->directories().isEmpty())
        d->fsWatcher->removePaths(d->fsWatcher->directories());
    d->scanDirectory(watchDirectory);
}

void FileWatcherPrivate::scanDirectory(const QString &directory)
{
    QString absolutePath = QDir(directory).absolutePath();
    if(fsWatcher->directories().contains(absolutePath)) {
        if(!QDir(absolutePath).exists()) {
            fsWatcher->removePath(absolutePath);
            return;
        }
    }

    static QStringList fileTypes = QStringList() << "*.qml" << "*.js" << "*.json" << "*.xml" << "*.txt";
    fsWatcher->addPath(QDir(absolutePath).absolutePath());
    QDirIterator i(directory, QDir::Dirs | QDir::NoDotAndDotDot | QDir::Files, QDirIterator::Subdirectories);
    while(i.hasNext()) {
        QString x = i.next();
        fsWatcher->addPath(QDir(x).absolutePath());
    }
}

}
