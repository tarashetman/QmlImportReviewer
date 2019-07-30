#include "QmlFile.hpp"
#include <QDebug>

static const QRegExp S_RE_IMPORTS( "[a-zA-Z.]+" );
static const QRegExp S_RE_VERSION( "[1-9]+(.)+[0-9]+" );
static const QRegExp S_RE_COMPONENTS( "([a-zA-Z]+)(\\s){0,1}(\\{){1}" );

QmlFile::QmlFile( QObject* parent )
    : QFile( parent )
{
    setFileName( "" );
    read_file( );
}

QmlFile::QmlFile( const QString& name, QObject* parent )
    : QFile( name, parent )
{
    read_file( );
}

QmlFile::QmlFile( const QmlFile& file )
    : QFile( file.fileName( ), file.parent( ) )
    , m_import_map( file.import_map( ) )
    , m_used_components( file.used_components( ) )
{
}

QmlFile&
QmlFile::operator=( const QmlFile& file )
{
    if ( this == &file )
    {
        return *this;
    }
    setParent( file.parent( ) );
    setFileName( file.fileName( ) );
    set_import_map( file.import_map( ) );
    set_used_components( file.used_components( ) );
    return *this;
}

QmlImportMap
QmlFile::import_map( ) const
{
    return m_import_map;
}

QStringList
QmlFile::used_components( ) const
{
    return m_used_components;
}

void
QmlFile::set_import_map( QmlImportMap import_map )
{
    if ( m_import_map == import_map )
    {
        return;
    }

    m_import_map = import_map;
    emit import_map_changed( m_import_map );
}

void
QmlFile::set_used_components( QStringList used_components )
{
    if ( m_used_components == used_components )
    {
        return;
    }

    m_used_components = used_components;
    emit used_components_changed( m_used_components );
}

void
QmlFile::read_file( )
{
    m_import_map.clear( );
    m_used_components.clear( );

    open( QIODevice::ReadOnly );
    while ( !atEnd( ) )
    {
        QString line = readLine( );
        QPair< QString, int > import{};
        QPair< QString, int > version{};
        QPair< QString, int > component{};

        if ( line.contains( "import " ) )
        {
            line.remove( "import " );
            import = find( S_RE_IMPORTS, line );
            version = find( S_RE_VERSION, line );

            if ( !import.first.isEmpty( ) && !version.first.isEmpty( ) )
            {
                m_import_map.insert( version.first, ComponentStatus( import.first, "" ) );
            }
            continue;
        }
        else
        {
            component = find( S_RE_COMPONENTS, line, 0, 1 );
            if ( !component.first.isEmpty( ) && !m_used_components.contains( component.first ) )
            {
                m_used_components << component.first;
            }
        }
    }
}

QPair< QString, int >
QmlFile::find( QRegExp reg, QString line, int pos, int cap )
{
    if ( reg.indexIn( line, pos ) != -1 )
    {
        return QPair< QString, int >( reg.cap( cap ), pos + reg.matchedLength( ) );
    }
    else
    {
        return QPair< QString, int >( "", 0 );
    }
}
