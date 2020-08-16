#include "qmlreloader.h"

#include <QQmlEngine>
#include <QQuickItem>
#include <QQmlContext>
#include <QQmlProperty>

#include <private/qqmldata_p.h>
#include <private/qqmlcontext_p.h>
#include <private/qquickpixmapcache_p.h>

#include <QStringList>
#include <QStack>
#include <QUrl>

#include <QDebug>

#include "qmlfileinterceptor.h"
#include "filewatcher.h"

namespace QMLreLoader {

class QMLreLoaderPrivate
{
public:
    QMLreLoaderPrivate(QQmlEngine *engine, const QString &watchDirectory)
        : engine(engine)
        , watchDirectory(watchDirectory)
        , fileInterceptor(new QMLFileInterceptor(watchDirectory))
        , fileWatcher(nullptr)
    {
    }

    ~QMLreLoaderPrivate()
    {
        if(fileWatcher) {
            delete fileWatcher;
            fileWatcher = nullptr;
        }
    }

    QQmlEngine *engine;
    QStack<QObject*> registeredLoaders;

    QString watchDirectory;
    QMLFileInterceptor *fileInterceptor;
    FileWatcher *fileWatcher;
};

QMLreLoader::QMLreLoader(QQmlEngine *engine, const QString &watchDirectory, QObject *parent)
    : QObject(parent)
    , d(new QMLreLoaderPrivate(engine, watchDirectory))
{
    if(!engine) {
        qDebug() << __PRETTY_FUNCTION__ << ": You must inform a QQmlEngine.";
        return;
    }
    engine->setUrlInterceptor(d->fileInterceptor);
    engine->rootContext()->setContextProperty("reloader", this);
}

QMLreLoader::~QMLreLoader()
{
    delete d;
}

QString QMLreLoader::watchDirectory() const
{
    return d->watchDirectory;
}

void QMLreLoader::setWatchDirectory(const QString &watchDirectory)
{
    d->watchDirectory = watchDirectory;
    d->fileInterceptor->setSourcePath(watchDirectory);
}

void QMLreLoader::start()
{
    if(d->fileWatcher == nullptr) {
        d->fileWatcher = new FileWatcher(this);
        d->fileWatcher->setWatchDirectory(d->watchDirectory);
        connect(d->fileWatcher, &FileWatcher::changedFiles, this, [this](const QStringList &changes){
            applyQmlChanges(changes);
        });
    }
}

void QMLreLoader::stop()
{
    if(d->fileWatcher) {
        delete d->fileWatcher;
        d->fileWatcher = nullptr;
    }
}

void QMLreLoader::registerLoader(QObject *loader)
{
    if(!d->engine) {
        qDebug() << __PRETTY_FUNCTION__ << ": Not ready: missing QQmlEngine.";
        return;
    }

    if(!loader) {
        qDebug() << __PRETTY_FUNCTION__ << ": Invalid object.";
        return;
    }

    if(d->watchDirectory.isEmpty()) {
        return;
    }

    bool notALoader = !QString(loader->metaObject()->className()).startsWith("QQuickLoader");
    if(notALoader) {
        qDebug() << __PRETTY_FUNCTION__ << ": Not a Loader." << loader;
        return;
    }

    if(!d->registeredLoaders.contains(loader))
        d->registeredLoaders.push(loader);
    else
        return;

    connect(loader, &QObject::destroyed, this, [&](){
        d->registeredLoaders.removeAll(loader);
    });

    if(d->fileWatcher == nullptr) {
        d->fileWatcher = new FileWatcher(this);
        d->fileWatcher->setWatchDirectory(d->watchDirectory);
        connect(d->fileWatcher, &FileWatcher::changedFiles, this, [this](const QStringList &changes){
            applyQmlChanges(changes);
        });
    }
}

void QMLreLoader::unregisterLoader(QObject *loader)
{
    d->registeredLoaders.removeAll(loader);
}

void QMLreLoader::applyQmlChanges(const QStringList &fileChanges)
{
    if(d->registeredLoaders.count() == 0 || fileChanges.isEmpty())
        return;

    QObject *destLoader = d->registeredLoaders.first();

    for(auto &loader : d->registeredLoaders) {
        QObjectList parents, children;
        parents << loader;

        do {
            children.clear();

            for(auto &parent : parents) {
                if(!parent) continue;

                for(auto &child : parent->findChildren<QObject*>(QString(), Qt::FindDirectChildrenOnly)) {
                    if(!d->registeredLoaders.contains(child))
                        children << child;
                }

                static QStringList qmlproperties = QStringList() << "item" << "contentItem" << "delegate";
                for(auto &qmlproperty : qmlproperties) {
                    QObject *item = qvariant_cast<QObject*>(QQmlProperty::read(parent, qmlproperty));
                    if(!d->registeredLoaders.contains(item)) {
                        for(auto &child : item->findChildren<QObject*>(QString(), Qt::FindDirectChildrenOnly)) {
                            if(!d->registeredLoaders.contains(child))
                                children << child;
                        }
                    }
                }
            }

            for(auto &child : children) {
                if(!child)
                    continue;

                QQmlData *data = QQmlData::get(child, false);
                if(data && data->context) {
                    QString qmlFile = data->context->url().toString();
                    if(qmlFile.contains('/'))
                        qmlFile = qmlFile.mid(qmlFile.lastIndexOf('/') + 1);

                    for(auto changed : fileChanges) {
                        if(changed.contains('/'))
                            changed = changed.mid(changed.lastIndexOf('/') + 1);

                        if(qmlFile == changed) {
                            //qDebug() << __PRETTY_FUNCTION__ << ": found" << qmlFile << "as the most near direct child of Loader" << loader;
                            destLoader = loader;
                            goto doQMLReload;
                        }
                    } // for changed
                } // qmldata
            } // for children
            parents = children;
        } while(children.count() > 0);
    }

doQMLReload:
    QString sourcePath = QQmlProperty::read(destLoader, "source").toString();
    //qDebug() << __PRETTY_FUNCTION__ << ": Re-applying source property on" << destLoader << "with" << sourcePath;
    QQmlProperty::write(destLoader, "source", QString());
    QQuickPixmap::purgeCache();
    d->engine->clearComponentCache();
    QQmlProperty::write(destLoader, "source", sourcePath);
}

}
