#include "QmldirTree.hpp"
#include <QDebug>
#include <QDirIterator>

//Node::Node( QObject* parent )
//    : QObject( parent )
//{
//}

//Node::Node( const QString& folder, bool contains_qmldir, QObject* parent )
//    : QObject( parent )
//    , m_folder( folder )
//    , m_contains_qmldir( contains_qmldir )
//{
//}

//Node::Node( const Node& node )
//{
//    setParent( node.parent( ) );
//    m_folder = node.folder( );
//    m_contains_qmldir = node.contains_qmldir( );
//}

//// bool
//// Node::operator==( const Node& node ) const
////{
////}

//QString
//Node::folder( ) const
//{
//    return m_folder;
//}

//void
//Node::set_folder( QString folder )
//{
//    if ( m_folder == folder )
//    {
//        return;
//    }

//    m_folder = folder;
//    emit folder_changed( m_folder );
//}

//bool
//Node::contains_qmldir( ) const
//{
//    return m_contains_qmldir;
//}

//void
//Node::set_contains_qmldir( bool contains_qmldir )
//{
//    if ( m_contains_qmldir == contains_qmldir )
//    {
//        return;
//    }

//    m_contains_qmldir = contains_qmldir;
//    emit contains_qmldir_changed( m_contains_qmldir );
//}

//------------------------------------------------------------------------------------------------

QmldirTree::QmldirTree( QObject* parent )
    : QObject( parent )
{
}

void
QmldirTree::start_searching_qmldir( QString folder )
{
    m_tree.clear();
    QString root = "";
    set_searching_in_progress( true );
    if ( contains_qmldir( folder ) )
    {
        root = folder;
        root = root.remove(0, root.lastIndexOf("/") + 1);
        m_tree[root] = QmldirFile(folder + "/qmldir");

    }

    QDirIterator dir( folder, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories );
    while ( dir.hasNext( ) )
    {
        if ( contains_qmldir( dir.next() ) )
        {
            m_tree[ (!root.isEmpty() ? (root + ".") : ("")) + dir.filePath().remove(folder + "/")
                    .replace("/", ".")] = QmldirFile(dir.filePath() + "/qmldir");
        }
    }
    emit tree_changed(m_tree);

    Tree::iterator end = m_tree.end();
    for (Tree::iterator begin = m_tree.begin() ; begin != end ; ++begin) {
        qDebug() << begin.key() << begin.value().qml_content().values("1.0");
    }
    set_searching_in_progress( false );
}

void
QmldirTree::reset( )
{
    m_tree.clear( );
    m_qml_folder_string = "";
}

QString
QmldirTree::qml_folder_string( ) const
{
    return m_qml_folder_string;
}

bool
QmldirTree::contains_qmldir( QString folder ) const
{
    return QDir( folder ).exists( "qmldir" );
}

bool QmldirTree::searching_in_progress() const
{
    return m_searching_in_progress;
}

Tree QmldirTree::tree() const
{
    return m_tree;
}

void
QmldirTree::set_qml_folder_string( QString qml_folder_string )
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

void QmldirTree::set_searching_in_progress(bool searching_in_progress)
{
    if (m_searching_in_progress == searching_in_progress)
    {
        return;
    }

    m_searching_in_progress = searching_in_progress;
    emit searching_in_progress_changed(m_searching_in_progress);
}
