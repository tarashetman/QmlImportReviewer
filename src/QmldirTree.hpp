#pragma once
#include <QDir>
#include <QList>
#include <QStringList>
#include "QmlFile.hpp"
#include "QmldirFile.hpp"

using QmldirTree = QMap< QString /*full import name*/, QmldirFile >;
using QmlTree = QList< QmlFile >;

class Tree : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString qml_folder_string READ qml_folder_string WRITE set_qml_folder_string NOTIFY
                        qml_folder_string_changed )
    Q_PROPERTY( bool searching_in_progress READ searching_in_progress WRITE
                        set_searching_in_progress NOTIFY searching_in_progress_changed )
    Q_PROPERTY( QmldirTree qmldir_tree READ qmldir_tree NOTIFY qmldir_tree_changed )
    Q_PROPERTY( QmlTree qml_tree READ qml_tree NOTIFY qml_tree_changed )

public:
    Tree( QObject* parent = nullptr );

    Q_INVOKABLE void reset( );
    Q_INVOKABLE void start_searching( const QString& folder );
    Q_INVOKABLE void start_review( );
    void start_searching_qmldir_files( const QString& folder );
    void start_searching_qml_files( const QString& folder );

    QString qml_folder_string( ) const;
    bool contains_qmldir_files( QString folder ) const;
    //    bool contains_qml_files( QString folder ) const;
    bool searching_in_progress( ) const;

    QmldirTree qmldir_tree( ) const;
    QmlTree qml_tree( ) const;

public slots:
    void set_qml_folder_string( QString qml_folder_string );
    void set_searching_in_progress( bool searching_in_progress );

signals:
    void qml_folder_string_changed( QString qml_folder_string );
    void searching_in_progress_changed( bool searching_in_progress );
    void qmldir_tree_changed( QmldirTree qmldir_tree );
    void qml_tree_changed( QmlTree qml_tree );

private:
    QString m_qml_folder_string = "";
    QDir m_qml_dir{};
    bool m_searching_in_progress = false;
    QmldirTree m_qmldir_tree{};
    QmlTree m_qml_tree{};
};
