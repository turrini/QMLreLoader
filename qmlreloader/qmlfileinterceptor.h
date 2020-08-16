#ifndef QMLFILEINTERCEPTOR_H
#define QMLFILEINTERCEPTOR_H

#include <QQmlAbstractUrlInterceptor>

namespace QMLreLoader {

class QMLFileInterceptor : public QQmlAbstractUrlInterceptor
{
public:
    QMLFileInterceptor() { }
    QMLFileInterceptor(const QString &sourcePath) : sourcePath(sourcePath) { }
    virtual ~QMLFileInterceptor() { }

    void setSourcePath(const QString &sourcePath) { this->sourcePath = sourcePath; }

    QUrl intercept(const QUrl &path, DataType type);
private:
    QString sourcePath;
};

}

#endif // QMLFILEINTERCEPTOR_H
