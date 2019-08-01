#include "QmlFile.hpp"
#include <QDebug>

static const QRegExp S_RE_IMPORTS( "[a-zA-Z.]+" );
static const QRegExp S_RE_VERSION( "[1-9]+(.)+[0-9]+" );
static const QRegExp S_RE_COMPONENTS( "([a-zA-Z]+)(\\s){0,1}(\\{){1}" );
static const QRegExp S_RE_COMPONENTS_AS_PROPERTY( "(property)+(\\s)([a-zA-Z]+)" );

QmlFile::QmlFile( QObject* parent )
    : QFile( parent )
{
}

QmlFile::QmlFile( const QString& name, QObject* parent )
    : QFile( name, parent )
{
    find_imports( );
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
QmlFile::set_import_map( const QmlImportMap& map )
{
    m_import_map = map;
}

bool
QmlFile::find_singleton( const QString& singleton )
{
    //    qDebug() << "check singleton" << singleton;
    open( QIODevice::ReadOnly );
    while ( !atEnd( ) )
    {
        QString line = readLine( );

        if ( !line.contains( "import " ) )
        {
            if ( line.contains( singleton + "." ) )
            {
                //                qDebug() << "contains singleton"
                //                         << singleton;
                close( );
                return true;
            }
        }
    }
    close( );
    return false;
}

void
QmlFile::delete_import( const QString& import )
{
    open( QIODevice::ReadWrite | QIODevice::Text );
    QString s;
    QTextStream t( this );
    while ( !t.atEnd( ) )
    {
        QString line = t.readLine( );
        if ( !line.contains( "import " + import ) )
            s.append( line + "\n" );
    }
    resize( 0 );
    t << s;
    close( );
}

void
QmlFile::find_imports( )
{
    if ( fileName( ).isEmpty( ) )
    {
        return;
    }

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
            component = find( S_RE_COMPONENTS_AS_PROPERTY, line, 0, 3 );
            if ( !component.first.isEmpty( ) && !m_used_components.contains( component.first ) )
            {
                m_used_components << component.first;
            }
        }
    }
    close( );
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
