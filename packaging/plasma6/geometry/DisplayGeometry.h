#pragma once
#include <QObject>
#include <QRect>
class DisplayGeometry final : public QObject {
    Q_OBJECT
public:
    using QObject::QObject;
    Q_INVOKABLE QRect screenGeometry(QObject *item) const;
    Q_INVOKABLE QRect virtualGeometry(QObject *item) const;
    Q_INVOKABLE qreal elapsedSeconds() const;
};
