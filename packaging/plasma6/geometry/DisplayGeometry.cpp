#include "DisplayGeometry.h"
#include <QElapsedTimer>
#include <QQuickItem>
#include <QQuickWindow>
#include <QScreen>
namespace {
QElapsedTimer &clockRef() {
    static QElapsedTimer clock = [] { QElapsedTimer t; t.start(); return t; }();
    return clock;
}
}
QRect DisplayGeometry::screenGeometry(QObject *item) const {
    auto *q = qobject_cast<QQuickItem *>(item);
    if (!q || !q->window() || !q->window()->screen()) return {};
    return q->window()->screen()->geometry();
}
QRect DisplayGeometry::virtualGeometry(QObject *item) const {
    auto *q = qobject_cast<QQuickItem *>(item);
    if (!q || !q->window() || !q->window()->screen()) return {};
    return q->window()->screen()->virtualGeometry();
}
qreal DisplayGeometry::elapsedSeconds() const {
    return static_cast<qreal>(clockRef().nsecsElapsed()) / 1000000000.0;
}
