#include "QmldirFile.hpp"
#include <QDebug>
#include <QRegExp>

static const QRegExp S_RE_COMPONENT("[a-zA-Z]+");
static const QRegExp S_RE_VERSION("[1-9]+(.)+[0-9]+");
static const QRegExp S_RE_FILE("[a-zA-Z]+(.qml)");

QmldirFile::QmldirFile(QObject *parent)
    : QFile(parent)
{
    read_file();
}

QmldirFile::QmldirFile(const QString &name, QObject *parent)
    : QFile (name, parent)
{
    read_file();
}

QmldirFile::QmldirFile(const QmldirFile &file)
    : QFile (file.parent())
    , m_qml_content( file.qml_content())
{
}

QmldirFile& QmldirFile::operator=(const QmldirFile &file)
{
    if (this == &file) {
        return *this;
    }
    setParent(file.parent());
    setFileName(file.fileName());
    m_qml_content = file.qml_content();
    return *this;
}

QmlFileMap QmldirFile::qml_content() const
{
    return m_qml_content;
}

void QmldirFile::set_qml_content(QmlFileMap qml_content)
{
    if (m_qml_content == qml_content)
    {
        return;
    }

    m_qml_content = qml_content;
    emit qml_content_changed(m_qml_content);
}

void QmldirFile::read_file()
{
    open(QIODevice::ReadOnly);
    while (!atEnd()) {
        QString line = readLine();

        QPair<QString, int> component = find(S_RE_COMPONENT, line);
        if(component.first.isEmpty())
        {
            continue;
        }
        if(component.first == "singleton")
        {
            component = find(S_RE_COMPONENT, line, component.second);
        }

        QPair<QString, int> version = find(S_RE_VERSION, line);
        if( version.first.isEmpty() )
        {
            continue;
        }
        QPair<QString, int> file = find(S_RE_FILE, line);
        if( file.first.isEmpty() )
        {
            continue;
        }

//        qDebug() << version.first << component.first << file.first;
        m_qml_content.insert(version.first, Content(component.first, file.first));
    }
}

QPair<QString, int> QmldirFile::find(QRegExp reg, QString line, int pos)
{
    if( reg.indexIn( line, pos ) != -1 )
    {
        return QPair<QString, int>(reg.cap( 0 ), pos + reg.matchedLength());
    }
    else
    {
        return QPair<QString, int>( "", 0 );
    }
}
