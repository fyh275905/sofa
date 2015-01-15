#ifndef TOOLS_H
#define TOOLS_H

#include <QQmlApplicationEngine>
#include <QApplication>
#include <QSettings>

class QOpenGLDebugLogger;

namespace sofa
{

namespace qtquick
{

class Tools : public QObject
{
    Q_OBJECT

public:
    Tools(QObject* parent = 0);
    ~Tools();

public:
    Q_PROPERTY(int overrideCursorShape READ overrideCursorShape WRITE setOverrideCursorShape NOTIFY overrideCursorShapeChanged);

public:
    int overrideCursorShape() const {return QApplication::overrideCursor() ? QApplication::overrideCursor()->shape() : Qt::ArrowCursor;}
    void setOverrideCursorShape(int newCursorShape);

signals:
    void overrideCursorShapeChanged();

public:
	Q_INVOKABLE void trimCache(QObject* object = 0);
	Q_INVOKABLE void clearSettingGroup(const QString& group);

public:
    static void setOpenGLDebugContext();    // must be call before the window has been shown
    static void useOpenGLDebugLogger();     // must be call after a valid opengl debug context has been made current

    static void useDefaultSettingsAtFirstLaunch(const QString& defaultSettingsPath = QString());
    static void copySettings(const QSettings& src, QSettings& dst);

};

}

}

#endif // TOOLS_H
