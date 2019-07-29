#include "qmldirFiles.hpp"
#include <QDebug>
#include <QDirIterator>

Node::Node( QObject* parent )
    : QObject( parent )
{
}

Node::Node( const QString& folder, bool contains_qmldir, QObject* parent )
    : QObject( parent )
    , m_folder( folder )
    , m_contains_qmldir( contains_qmldir )
{
}

Node::Node( const Node& node )
{
    setParent( node.parent( ) );
    m_folder = node.folder( );
    m_contains_qmldir = node.contains_qmldir( );
}

// bool
// Node::operator==( const Node& node ) const
//{
//}

QString
Node::folder( ) const
{
    return m_folder;
}

void
Node::set_folder( QString folder )
{
    if ( m_folder == folder )
    {
        return;
    }

    m_folder = folder;
    emit folder_changed( m_folder );
}

bool
Node::contains_qmldir( ) const
{
    return m_contains_qmldir;
}

void
Node::set_contains_qmldir( bool contains_qmldir )
{
    if ( m_contains_qmldir == contains_qmldir )
    {
        return;
    }

    m_contains_qmldir = contains_qmldir;
    emit contains_qmldir_changed( m_contains_qmldir );
}

//------------------------------------------------------------------------------------------------

qmldirFiles::qmldirFiles( QObject* parent )
    : QObject( parent )
{
}

void
qmldirFiles::set_node( QString name, QObject* parent )
{
}

void
qmldirFiles::start_searching_qmldir( QString folder, QObject* parent )
{
    //    for ( QString dir : children_directory( folder ) )
    //    {
    //        dir.prepend( folder + "/" );

    //        Node node( dir, contains_qmldir( dir ), parent );

    //        if ( children_directory( dir ).count( ) > 0 )
    //        {
    //            start_searching_qmldir( dir,
    //                                    ( &m_qmldir_list.last( ) ) ? &m_qmldir_list.last( ) :
    //                                    nullptr );
    //        }
    //    }

    QDirIterator dir( folder, QDir::Dirs | QDir::NoDotAndDotDot );
    while ( dir.hasNext( ) )
    {
        qDebug( ) << dir.next( );

        if ( children_directory( dir.filePath( ) ).count( ) > 0 )
        {
            start_searching_qmldir( dir.filePath( ) );
        }
    }
}

void
qmldirFiles::reset( )
{
    m_qmldir_list.clear( );
    m_qml_folder_string = "";
    m_qmldir_list.clear( );
}

QString
qmldirFiles::qml_folder_string( ) const
{
    return m_qml_folder_string;
}

QStringList
qmldirFiles::children_directory( QString folder )
{
    return QDir( folder ).entryList( QDir::Dirs | QDir::NoDotAndDotDot );
}

bool
qmldirFiles::contains_qmldir( QString folder )
{
    return QDir( folder ).exists( "qmldir" );
}

void
qmldirFiles::set_qml_folder_string( QString qml_folder_string )
{
    qml_folder_string.remove( 0, 7 );
    if ( m_qml_folder_string == qml_folder_string )
    {
        return;
    }

    m_qml_folder_string = qml_folder_string;
    m_qml_dir.setPath( m_qml_folder_string );
    emit qml_folder_string_changed( m_qml_folder_string );
}
