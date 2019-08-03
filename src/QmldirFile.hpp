#pragma once
#include <QDebug>
#include <QFile>
#include <QMultiMap>
#include <QObject>
#include <QPair>

struct ContentComponent : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString name_of_component READ name_of_component )
    Q_PROPERTY( QString related_qml_file_name READ related_qml_file_name )
    Q_PROPERTY( int count_of_using READ count_of_using WRITE set_count_of_using NOTIFY
                        count_of_using_changed )

public:
    ContentComponent( QObject* parent = nullptr ) = delete;
    ContentComponent( QString name_of_component,
                      QString related_qml_file,
                      QObject* parent = nullptr )
        : QObject( parent )
        , m_name_of_component( name_of_component )
        , m_related_qml_file_name( related_qml_file )
    {
    }

    ContentComponent( const ContentComponent& component )
        : QObject( component.parent( ) )
        , m_name_of_component( component.name_of_component( ) )
        , m_related_qml_file_name( component.related_qml_file_name( ) )
        , m_count_of_using( component.count_of_using( ) )
    {
    }

    ContentComponent&
    operator=( const ContentComponent& component )
    {
        if ( this == &component )
        {
            return *this;
        }
        setParent( component.parent( ) );
        m_name_of_component = component.name_of_component( );
        m_related_qml_file_name = component.related_qml_file_name( );
        m_count_of_using = component.count_of_using( );
        return *this;
    }

    bool
    operator==( const ContentComponent& l )
    {
        return m_name_of_component == l.name_of_component( )
               && m_related_qml_file_name == l.related_qml_file_name( )
               && m_count_of_using == l.count_of_using( );
    }

    void
    increment_count_of_using( )
    {
        m_count_of_using += 1;
        emit count_of_using_changed( m_count_of_using );
    }

    QString
    name_of_component( ) const
    {
        return m_name_of_component;
    }

    QString
    related_qml_file_name( ) const
    {
        return m_related_qml_file_name;
    }

    int
    count_of_using( ) const
    {
        return m_count_of_using;
    }

public slots:
    void
    set_count_of_using( int count_of_using )
    {
        if ( m_count_of_using == count_of_using )
        {
            return;
        }

        m_count_of_using = count_of_using;
        emit count_of_using_changed( m_count_of_using );
    }

signals:
    void count_of_using_changed( int count_of_using );

private:
    QString m_name_of_component = "";
    QString m_related_qml_file_name = "";
    int m_count_of_using = 0;
};

//----------------------------------------------------------------------------------------------------------

using QmldirContentMap = QMultiMap< QString /*version*/, QSharedPointer< ContentComponent > >;

class QmldirFile : public QFile
{
    Q_OBJECT

    Q_PROPERTY( QmldirContentMap components READ components NOTIFY components_changed )
    Q_PROPERTY( QmldirContentMap singletons READ singletons NOTIFY singletons_changed )

public:
    QmldirFile( QObject* parent = nullptr );
    QmldirFile( const QString& name, QObject* parent = nullptr );
    QmldirFile( const QmldirFile& file );
    QmldirFile& operator=( const QmldirFile& file );

    QmldirContentMap components( ) const;
    QmldirContentMap singletons( ) const;

    QString path( );

    void delete_component_or_singleton( const QString& name, const QString& version );

private:
    void read_file( );
    QPair< QString, int > find( QRegExp reg, QString line, int pos = 0 );

signals:
    void components_changed( QmldirContentMap components );
    void singletons_changed( QmldirContentMap singletons );

private:
    QmldirContentMap m_components{};
    QmldirContentMap m_singletons{};
};
