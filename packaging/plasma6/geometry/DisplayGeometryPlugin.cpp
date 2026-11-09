#include <QQmlEngine>
#include <QJSEngine>
#include <QQmlExtensionPlugin>
#include <qqml.h>
#include "DisplayGeometry.h"
class GalacticOrbitGeometryPlugin final : public QQmlExtensionPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)
public:
    void registerTypes(const char *uri) override {
        qmlRegisterSingletonType<DisplayGeometry>(uri, 1, 0, "DisplayGeometry",
            [](QQmlEngine *, QJSEngine *) -> QObject * { return new DisplayGeometry; });
    }
};
#include "DisplayGeometryPlugin.moc"
