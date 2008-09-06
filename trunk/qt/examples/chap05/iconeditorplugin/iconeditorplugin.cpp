#include <qwidgetplugin.h>

#include "../iconeditor/iconeditor.h"

class IconEditorPlugin : public QWidgetPlugin
{
public:
    QStringList keys() const;
    QWidget *create(const QString &key, QWidget *parent,
                    const char *name);
    QString includeFile(const QString &key) const;
    QString group(const QString &key) const;
    QIconSet iconSet(const QString &key) const;
    QString toolTip(const QString &key) const;
    QString whatsThis(const QString &key) const;
    bool isContainer(const QString &key) const;
};

QStringList IconEditorPlugin::keys() const
{
    return QStringList() << "IconEditor";
}

QWidget *IconEditorPlugin::create(const QString &, QWidget *parent,
                                  const char *name)
{
    return new IconEditor(parent, name);
}

QString IconEditorPlugin::includeFile(const QString &) const
{
    return "iconeditor.h";
}

bool IconEditorPlugin::isContainer(const QString &) const
{
    return false;
}

QString IconEditorPlugin::group(const QString &) const
{
    return "Plugin Widgets";
}

QIconSet IconEditorPlugin::iconSet(const QString &) const
{
    return QIconSet(QPixmap::fromMimeSource("iconeditor.png"));
}

QString IconEditorPlugin::toolTip(const QString &) const
{
    return "Icon Editor";
}

QString IconEditorPlugin::whatsThis(const QString &) const
{
    return "Widget for creating and editing icons";
}

Q_EXPORT_PLUGIN(IconEditorPlugin)
