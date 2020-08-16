# QML reLoader


Português | [English (United States)](./README.md)

### Overview
O **QML reLoader** permite aos desenvolvedores para rapidamente recarregar a interface do usuário a cada documento salvo, por monitorar a hierarquia dos arquivos carregados quando se usa o componente Loader do Qt. A principal diferença desta para as outras soluções é que nós não iremos recarregar o aplicativo inteiro, mas apenas as partes que foram modificadas (e suas dependências). É voltado para acelerar o desenvolvimento de projetos, dos pequenos aos grandes.

Suporta Linux, macOS e Windows. Foi testado com Qt 5.12 ao 5.15. Licença utilizada: MIT.


## Demonstração

![QML reLoader em ação](demo.gif)



## Como usar

Há apenas 3 passos:

**1:** Adicione as pasta **qmlreloader/** ao seu projeto e inclua o **qmlreloader.pri** no seu arquivo ".pro", como a seguir:


```qmake
QT += quick

CONFIG += c++11 silent

INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/main.cpp

RESOURCES += \
    $$PWD/qml.qrc

# QML reLoader
include($$PWD/../qmlreloader/qmlreloader.pri)```
```


**2:** Depois inclua o cabeçalho **qmlreloader.h** no seu main.cpp, e depois da declaração da engine, registre-a no *QML reLoader*:

```c++
#include <QGuiApplication>
#include <QQmlApplicationEngine>

/// Inclua o cabeçalho
#include "qmlreloader.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    
    /// Registre a engine QML com o QML reLoader
    QMLreLoader::QMLreLoader reloader(&engine);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
```

**3:** E então para casa uso do tradicional *Loader do QML*, registre-o no *QML reLoader* no sinal *Component.onCompleted {}*, e estará pronto para uso:

```qml
Loader {
    id: loadCRM

    anchors.fill: parent
    source: "qrc:/modules/crm/StartCRM.qml"

    Component.onCompleted: {
        reloader.registerLoader(loadCRM); // este loader agora será monitorado
    }
}
```

Copyright (c) 2020 Arthur Turrini.

