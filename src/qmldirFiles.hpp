#pragma once
#include <QDir>
#include <QList>
#include <QStringList>

class Node : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString folder READ folder NOTIFY folder_changed )
    Q_PROPERTY( bool contains_qmldir READ contains_qmldir NOTIFY contains_qmldir_changed )

public:
    Node( QObject* parent = nullptr );
    Node( const QString& folder, bool contains_qmldir, QObject* parent = nullptr );
    Node( const Node& node );

    //    bool operator==( const Node& node ) const;

    QString folder( ) const;
    void set_folder( QString folder );
    bool contains_qmldir( ) const;

public slots:
    void set_contains_qmldir( bool contains_qmldir );

signals:
    void folder_changed( QString folder );
    void contains_qmldir_changed( bool contains_qmldir );

private:
    QString m_folder;
    bool m_contains_qmldir = false;
};

class qmldirFiles : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString qml_folder_string READ qml_folder_string WRITE set_qml_folder_string NOTIFY
                        qml_folder_string_changed )

public:
    qmldirFiles( QObject* parent = nullptr );

    Q_INVOKABLE void set_node( QString name, QObject* parent );
    Q_INVOKABLE void start_searching_qmldir( QString folder, QObject* paren = nullptr );
    Q_INVOKABLE void reset( );

    QString qml_folder_string( ) const;
    QStringList children_directory( QString folder );
    bool contains_qmldir( QString folder );

public slots:
    void set_qml_folder_string( QString qml_folder_string );

signals:
    void qml_folder_string_changed( QString qml_folder_string );

private:
    QString m_qml_folder_string = "";
    QDir m_qml_dir{};
    QList< Node > m_qmldir_list{};
};
