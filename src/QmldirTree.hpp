#pragma once
#include <QDir>
#include <QList>
#include <QStringList>
#include "QmldirFile.hpp"

//class Node : public QObject
//{
//    Q_OBJECT
//    Q_PROPERTY( QString folder READ folder NOTIFY folder_changed )
//    Q_PROPERTY( bool contains_qmldir READ contains_qmldir NOTIFY contains_qmldir_changed )

//public:
//    Node( QObject* parent = nullptr );
//    Node( const QString& folder, bool contains_qmldir, QObject* parent = nullptr );
//    Node( const Node& node );

//    //    bool operator==( const Node& node ) const;

//    QString folder( ) const;
//    void set_folder( QString folder );
//    bool contains_qmldir( ) const;

//public slots:
//    void set_contains_qmldir( bool contains_qmldir );

//signals:
//    void folder_changed( QString folder );
//    void contains_qmldir_changed( bool contains_qmldir );

//private:
//    QString m_folder;
//    bool m_contains_qmldir = false;
//};

//------------------------------------------------------------------------------------------------
using Tree = QMap<QString, QmldirFile>;


class QmldirTree : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString qml_folder_string READ qml_folder_string WRITE set_qml_folder_string NOTIFY
                        qml_folder_string_changed )
    Q_PROPERTY(bool searching_in_progress READ searching_in_progress WRITE set_searching_in_progress NOTIFY searching_in_progress_changed)
    Q_PROPERTY(Tree tree READ tree NOTIFY tree_changed)

public:
    QmldirTree( QObject* parent = nullptr );

    Q_INVOKABLE void start_searching_qmldir( QString folder );
    Q_INVOKABLE void reset( );

    QString qml_folder_string( ) const;
    bool contains_qmldir( QString folder ) const;
    bool searching_in_progress() const;

    Tree tree() const;

public slots:
    void set_qml_folder_string( QString qml_folder_string );
    void set_searching_in_progress(bool searching_in_progress);

signals:
    void qml_folder_string_changed( QString qml_folder_string );
    void searching_in_progress_changed(bool searching_in_progress);

    void tree_changed(Tree tree);

private:
    QString m_qml_folder_string = "";
    QDir m_qml_dir{};
    bool m_searching_in_progress = false;
    Tree m_tree;
};
