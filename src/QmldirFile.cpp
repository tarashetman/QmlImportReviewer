#include "QmldirFile.hpp"
#include <QDebug>
#include <QRegExp>

static const QRegExp S_RE_COMPONENT( "[a-zA-Z]+" );
static const QRegExp S_RE_VERSION( "[1-9]+(.)+[0-9]+" );
static const QRegExp S_RE_FILE( "[a-zA-Z]+(.qml)" );

QmldirFile::QmldirFile( QObject* parent )
    : QFile( parent )
{
}

QmldirFile::QmldirFile( const QString& name, QObject* parent )
    : QFile( name, parent )
{
    read_file( );
}

QmldirFile::QmldirFile( const QmldirFile& file )
    : QFile( file.fileName( ), file.parent( ) )
    , m_qml_components( file.qml_components( ) )
    , m_qml_singletons( file.qml_singletons( ) )
{
}

QmldirFile&
QmldirFile::operator=( const QmldirFile& file )
{
    if ( this == &file )
    {
        return *this;
    }
    setParent( file.parent( ) );
    setFileName( file.fileName( ) );
    m_qml_components = file.qml_components( );
    m_qml_singletons = file.qml_singletons( );
    return *this;
}

QmldirContentMap
QmldirFile::qml_components( ) const
{
    return m_qml_components;
}

QmldirContentMap QmldirFile::qml_singletons() const
{
    return m_qml_singletons;
}

void
QmldirFile::read_file( )
{
    if(fileName().isEmpty())
    {
        return;
    }

    m_qml_components.clear( );
    open( QIODevice::ReadOnly );
    while ( !atEnd( ) )
    {
        QString line = readLine( );
        bool is_singleton = false;

        QPair< QString, int > component = find( S_RE_COMPONENT, line );
        if ( component.first.isEmpty( ) )
        {
            continue;
        }
        if ( component.first == "singleton" )
        {
            is_singleton = true;
            component = find( S_RE_COMPONENT, line, component.second );
        }

        QPair< QString, int > version = find( S_RE_VERSION, line );
        if ( version.first.isEmpty( ) )
        {
            continue;
        }
        QPair< QString, int > file = find( S_RE_FILE, line );
        if ( file.first.isEmpty( ) )
        {
            continue;
        }

        if( is_singleton )
        {
            m_qml_singletons.insert( version.first, ContentComponent( component.first, file.first ) );
        }
        else
        {
            m_qml_components.insert( version.first, ContentComponent( component.first, file.first ) );
        }
    }
    close( );
}

QPair< QString, int >
QmldirFile::find( QRegExp reg, QString line, int pos )
{
    if ( reg.indexIn( line, pos ) != -1 )
    {
        return QPair< QString, int >( reg.cap( 0 ), pos + reg.matchedLength( ) );
    }
    else
    {
        return QPair< QString, int >( "", 0 );
    }
}
