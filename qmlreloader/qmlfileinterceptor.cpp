#include "qmlfileinterceptor.h"

namespace QMLreLoader {

QUrl QMLFileInterceptor::intercept(const QUrl &path, QQmlAbstractUrlInterceptor::DataType type)
{
    Q_UNUSED(type)

    if(Q_LIKELY(path.scheme() == QStringLiteral("qrc"))) {
        QString _path = path.toString();

        if(_path.startsWith(QStringLiteral("qrc:/qt-project.org")))
            return path;

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        if(type == QQmlAbstractUrlInterceptor::QmldirFile)
            return path;
#endif
        _path.replace(QStringLiteral("qrc:/"), sourcePath);

        return QUrl::fromUserInput(_path);
    }
    return path;
}

}
