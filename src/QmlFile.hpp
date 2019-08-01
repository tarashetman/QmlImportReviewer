#pragma once
#include <QFile>
#include "QmldirFile.hpp"

struct ComponentStatus : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString full_import_name READ full_import_name )
    Q_PROPERTY( QString error READ error )

public:
    ComponentStatus( QObject* parent = nullptr ) = delete;
    ComponentStatus( QString full_import_name, QString related_qml_file, QObject* parent = nullptr )
        : QObject( parent )
        , m_full_import_name( full_import_name )
        , m_error( related_qml_file )
    {
    }
    ComponentStatus( const ComponentStatus& component )
        : QObject( component.parent( ) )
        , m_full_import_name( component.full_import_name( ) )
        , m_error( component.error( ) )
    {
    }

    ComponentStatus&
    operator=( const ComponentStatus& component )
    {
        if ( this == &component )
        {
            return *this;
        }
        setParent( component.parent( ) );
        m_full_import_name = component.full_import_name( );
        m_error = component.error( );
        return *this;
    }

    bool
    operator==( const ComponentStatus& l )
    {
        return m_full_import_name == l.full_import_name( ) && m_error == l.error( );
    }

    QString
    full_import_name( ) const
    {
        return m_full_import_name;
    }

    QString
    error( ) const
    {
        return m_error;
    }

    void
    set_error( QString error )
    {
        m_error = error;
    }

private:
    QString m_full_import_name{};
    QString m_error{};
};

//----------------------------------------------------------------------------------------------------------

// using ComponentStatus = QPair< QString /*full import name*/, QString /*error*/ >;
using QmlImportMap = QMultiMap< QString /*version*/, ComponentStatus >;

class QmlFile : public QFile
{
    Q_OBJECT
    Q_PROPERTY( QmlImportMap import_map READ import_map NOTIFY import_map_changed )
    Q_PROPERTY( QStringList used_components READ used_components NOTIFY used_components_changed )

public:
    QmlFile( QObject* parent = nullptr );
    QmlFile( const QString& name, QObject* parent = nullptr );
    QmlFile( const QmlFile& file );
    QmlFile& operator=( const QmlFile& file );

    QmlImportMap import_map( ) const;
    QStringList used_components( ) const;
    bool find_singleton(const QString &singleton );

public slots:
    void set_used_components( QStringList used_components );
    void set_import_map( const QmlImportMap& map );

private:
    void find_imports( );
    QPair< QString, int > find( QRegExp reg, QString line, int pos = 0, int cap = 0 );

signals:
    void import_map_changed( QmlImportMap import_map );
    void used_components_changed( QStringList used_components );

private:
    QmlImportMap m_import_map{};
    QStringList m_used_components{};
};
