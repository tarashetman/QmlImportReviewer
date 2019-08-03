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
    // for all *.qml get import map
    for ( auto& qml_file : m_qml_tree )
    {
        QStringList module_versions;
        module_versions << qml_file.import_map( ).keys( );
        module_versions.removeDuplicates( );

        // for all individual import versions get imports
        for ( const auto& module_version : module_versions )
        {
            QList< QSharedPointer< ComponentStatus > > imports
                    = qml_file.import_map( ).values( module_version );

            // for all imports get qmldir components
            for ( auto& import : imports )
            {
                QList< QSharedPointer< ContentComponent > > available_components;
                available_components << qmldir_tree( )
                                                .value( import->full_import_name( ) )
                                                .components( )
                                                .values( module_version );

                QList< QSharedPointer< ContentComponent > > available_singletons;
                available_singletons << qmldir_tree( )
                                                .value( import->full_import_name( ) )
                                                .singletons( )
                                                .values( module_version );

                if ( available_components.isEmpty( ) && available_singletons.isEmpty( ) )
                {
                    import->set_error( "EMPTY COMPONENT LIST" );
                    continue;
                }
                // And now I can compare it

                bool is_using = false;

                // components
                if ( !available_components.isEmpty( ) )
                {
                    for ( auto& available_component : available_components )
                    {
                        if ( qml_file.used_components( ).contains(
                                     available_component->name_of_component( ) ) )
                        {
                            is_using = true;
                            available_component->increment_count_of_using( );
                        }
                    }
                }

                // singletons
                if ( !available_singletons.isEmpty( ) )
                {
                    for ( auto& available_singleton : available_singletons )
                    {
                        if ( qml_file.find_singleton( available_singleton->name_of_component( ) ) )
                        {
                            is_using = true;
                            available_singleton->increment_count_of_using( );
                        }
                    }
                }

                if ( !is_using )
                {
                    import->set_error( "DON'T USES" );
                }
            }
        }
    }
}

void
Tree::delete_unusable_qml_files( )
{
    for ( auto& qmldir_file : qmldir_tree( ) )
    {
        QStringList module_versions;
        module_versions << qmldir_file.components( ).keys( );
        module_versions.removeDuplicates( );

        for ( const auto& module_version : module_versions )
        {
            for ( auto& component : qmldir_file.components( ).values( module_version ) )
            {
                if ( component->count_of_using( ) == 0 )
                {
                    delete_qml_file_and_component_from_qmldir(
                            &qmldir_file, component, module_version );
                }
            }
        }

        QStringList singleton_versions;
        singleton_versions << qmldir_file.singletons( ).keys( );
        singleton_versions.removeDuplicates( );

        for ( const auto& singleton_version : singleton_versions )
        {
            for ( auto& singleton : qmldir_file.singletons( ).values( singleton_version ) )
            {
                if ( singleton->count_of_using( ) == 0 )
                {
                    delete_qml_file_and_component_from_qmldir(
                            &qmldir_file, singleton, singleton_version );
                }
            }
        }
    }
}

void
Tree::delete_unusable_imports( )
{
    // for all *.qml get import map
    for ( auto& qml_file : m_qml_tree )
    {
        QStringList module_versions;
        module_versions << qml_file.import_map( ).keys( );
        module_versions.removeDuplicates( );

        // for all individual import versions get imports
        for ( const auto& module_version : module_versions )
        {
            QList< QSharedPointer< ComponentStatus > > imports
                    = qml_file.import_map( ).values( module_version );

            // for all imports get qmldir components
            for ( auto& import : imports )
            {
                if ( import->error( ) == "DON'T USES" )
                {
                    qDebug( ) << import->error( );
                    qml_file.delete_import( import->full_import_name( ) );  // delete unusable
                                                                            // imports
                }
            }
        }
    }
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
}

QString
Tree::get_import_name_from_dir( const QString& folder )
{
    QmldirTree::iterator end = m_qmldir_tree.end( );
    for ( QmldirTree::iterator qmldir = m_qmldir_tree.begin( ); qmldir != end; ++qmldir )
    {
        if ( qmldir.value( ).path( ) == folder )
        {
            return qmldir.key( );
        }
    }
    return QString( );
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
        QmlFile qml_file( root_dir.next( ), this );

        QString import = get_import_name_from_dir( root_dir.path( ) );
        if ( !import.isEmpty( ) )
        {
            qml_file.emulate_path_qmldir_import( import, "1.0" );
        }

        m_qml_tree.append( qml_file );
    }

    QDirIterator sub_dirs(
            folder, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories );
    while ( sub_dirs.hasNext( ) )
    {
        QDirIterator sub_dir( sub_dirs.next( ), filters, QDir::Files | QDir::NoSymLinks );
        while ( sub_dir.hasNext( ) )
        {
            QmlFile qml_file( sub_dir.next( ), this );

            QString import = get_import_name_from_dir( sub_dir.path( ) );
            if ( !import.isEmpty( ) )
            {
                qml_file.emulate_path_qmldir_import( import, "1.0" );
            }

            m_qml_tree.append( qml_file );
        }
    }

    emit qml_tree_changed( m_qml_tree );
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

void
Tree::delete_qml_file_and_component_from_qmldir( QmldirFile* qmldir,
                                                 QSharedPointer< ContentComponent > component,
                                                 const QString& version )
{
    qDebug( ) << component->name_of_component( ) << version << component->count_of_using( );
    qmldir->delete_component_or_singleton( component->name_of_component( ), version );
}
