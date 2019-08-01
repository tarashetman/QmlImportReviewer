#include "QmldirTree.hpp"
#include <QDebug>
#include <QDirIterator>

Tree::Tree( QObject* parent )
    : QObject( parent )
{
}

void
Tree::start_searching( const QString& folder )
{
    //    qDebug( ) << contains_qml_files( folder );
    set_searching_in_progress( true );

    start_searching_qmldir_files( folder );
    start_searching_qml_files( folder );

    set_searching_in_progress( false );

    qDebug( ) << "qmldir files count:" << m_qmldir_tree.count( );
    qDebug( ) << "qml files count:" << m_qml_tree.count( );

    start_review_of_imports( );
}

void
Tree::start_review_of_imports( )
{
    QmlTree::iterator end = m_qml_tree.end( );
    // for all *.qml get import map
    for ( QmlTree::iterator qml_tree_it = m_qml_tree.begin( ); qml_tree_it != end; ++qml_tree_it )
    {
        qDebug( ) << qml_tree_it->fileName( );
        QStringList module_versions;
        module_versions << qml_tree_it->import_map( ).keys( );
        module_versions.removeDuplicates( );

        // for all individual import versions get imports
        for ( auto& module_version : module_versions )
        {
            QList< ComponentStatus > imports = qml_tree_it->import_map( ).values( module_version );

            // for all imports get qmldir components
            for ( auto& import : imports )
            {
                QList< ContentComponent > available_components;
                available_components << qmldir_tree( )
                                                .value( import.full_import_name( ) )
                                                .qml_components( )
                                                .values( module_version );

                QList< ContentComponent > available_singletons;
                available_singletons << qmldir_tree( )
                                                .value( import.full_import_name( ) )
                                                .qml_singletons( )
                                                .values( module_version );

                if ( available_components.isEmpty( ) && available_singletons.isEmpty( ) )
                {
                    import.set_error( "EMPTY COMPONENT LIST" );
                    continue;
                }
                // And now I can compare it

                bool is_using = false;

                // components
                if ( !available_components.isEmpty( ) )
                {
                    for ( const auto& available_component : available_components )
                    {
                        if ( qml_tree_it->used_components( ).contains(
                                     available_component.name_of_component( ) ) )
                        {
                            is_using = true;
                            break;
                        }
                    }
                }

                // singletons
                if ( !available_singletons.isEmpty( ) && !is_using )
                {
                    for ( const auto& available_singleton : available_singletons )
                    {
                        if ( qml_tree_it->find_singleton(
                                     available_singleton.name_of_component( ) ) )
                        {
                            is_using = true;
                            break;
                        }
                    }
                }

                if ( !is_using )
                {
                    import.set_error( "DON'T USES" );
                    qDebug( ) << import.full_import_name( ) << module_version << import.error( );

                    qml_tree_it->delete_import( import.full_import_name( ) );
                }
            }
        }
        qDebug( ) << "\n";
    }
}

void
Tree::start_review_of_usable_qml_files( )
{
}

void
Tree::start_searching_qmldir_files( const QString& folder )
{
    m_qmldir_tree.clear( );
    QString root = "";
    if ( contains_qmldir_files( folder ) )
    {
        root = folder;
        root = root.remove( 0, root.lastIndexOf( "/" ) + 1 );
        m_qmldir_tree[ root ] = QmldirFile( folder + "/qmldir", this );
    }

    QDirIterator dir( folder, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories );
    while ( dir.hasNext( ) )
    {
        if ( contains_qmldir_files( dir.next( ) ) )
        {
            m_qmldir_tree[ ( !root.isEmpty( ) ? ( root + "." ) : ( "" ) )
                           + dir.filePath( ).remove( folder + "/" ).replace( "/", "." ) ]
                    = QmldirFile( dir.filePath( ) + "/qmldir", this );
        }
    }
    emit qmldir_tree_changed( m_qmldir_tree );

    //    QmldirTree::iterator end = m_qmldir_tree.end( );
    //    for ( QmldirTree::iterator begin = m_qmldir_tree.begin( ); begin != end; ++begin )
    //    {
    //        qDebug( ) << begin.module_version( ) << begin.value( ).qml_components( ).values( "1.0"
    //        );
    //    }
}

void
Tree::start_searching_qml_files( const QString& folder )
{
    m_qml_tree.clear( );
    QStringList filters;
    filters << "*.qml";

    QDirIterator root_dir( folder, filters, QDir::Files | QDir::NoSymLinks );
    while ( root_dir.hasNext( ) )
    {
        m_qml_tree.append( QmlFile( root_dir.next( ), this ) );
    }

    QDirIterator sub_dirs(
            folder, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories );
    while ( sub_dirs.hasNext( ) )
    {
        QDirIterator sub_dir( sub_dirs.next( ), filters, QDir::Files | QDir::NoSymLinks );
        while ( sub_dir.hasNext( ) )
        {
            m_qml_tree.append( QmlFile( sub_dir.next( ), this ) );
        }
    }

    emit qml_tree_changed( m_qml_tree );

    //    QmlTree::iterator end = m_qml_tree.end( );
    //    for ( QmlTree::iterator begin = m_qml_tree.begin( ); begin != end; ++begin )
    //    {
    //        qDebug( ) << begin->fileName( );
    //    }
}

void
Tree::reset( )
{
    m_qmldir_tree.clear( );
    m_qml_tree.clear( );
    m_qml_folder_string = "";
}

QString
Tree::qml_folder_string( ) const
{
    return m_qml_folder_string;
}

bool
Tree::contains_qmldir_files( QString folder ) const
{
    return QDir( folder ).exists( "qmldir" );
}

bool
Tree::searching_in_progress( ) const
{
    return m_searching_in_progress;
}

QmldirTree
Tree::qmldir_tree( ) const
{
    return m_qmldir_tree;
}

QmlTree
Tree::qml_tree( ) const
{
    return m_qml_tree;
}

QVariantList
Tree::qml_error_imports( ) const
{
    return m_qml_error_imports;
}

void
Tree::set_qml_folder_string( QString qml_folder_string )
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

void
Tree::set_searching_in_progress( bool searching_in_progress )
{
    if ( m_searching_in_progress == searching_in_progress )
    {
        return;
    }

    m_searching_in_progress = searching_in_progress;
    emit searching_in_progress_changed( m_searching_in_progress );
}
