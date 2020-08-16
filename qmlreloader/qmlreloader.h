#ifndef QMLRELOADER_H
#define QMLRELOADER_H

#include <QObject>

class QQmlEngine;
class QQuickItem;

namespace QMLreLoader {

class QMLreLoaderPrivate;

class QMLreLoader : public QObject
{
    Q_OBJECT

public:
    explicit QMLreLoader(QQmlEngine *engine
                         , const QString &watchDirectory = PROJECT_SOURCE_DIR
                         , QObject *parent = nullptr);
    ~QMLreLoader();

    QString watchDirectory() const;
    void setWatchDirectory(const QString &watchDirectory);

public slots:
    void start();
    void stop();

    void registerLoader(QObject *loader);
    void unregisterLoader(QObject *loader);

private:
    QMLreLoaderPrivate *d;
    void applyQmlChanges(const QStringList &fileChanges);
};

}

#endif // QMLRELOADER_H
