#pragma once
#include <QFile>
#include <QMultiMap>
#include <QObject>
#include <QPair>

struct ContentComponent : public QObject
{
    Q_GADGET
    Q_PROPERTY( QString name_of_component READ name_of_component )
    Q_PROPERTY( QString related_qml_file_name READ related_qml_file_name )

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
        return *this;
    }

    bool
    operator==( const ContentComponent& l )
    {
        return m_name_of_component == l.name_of_component( )
               && m_related_qml_file_name == l.related_qml_file_name( );
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

private:
    QString m_name_of_component;
    QString m_related_qml_file_name;
};

//----------------------------------------------------------------------------------------------------------

using QmldirContentMap = QMultiMap< QString /*version*/, ContentComponent >;

class QmldirFile : public QFile
{
    Q_OBJECT
    Q_PROPERTY( QmldirContentMap qml_content READ qml_content NOTIFY qml_content_changed )

    enum class ErrorCode
    {
        NONE,
        EMPTY
    };

public:
    QmldirFile( QObject* parent = nullptr );
    QmldirFile( const QString& name, QObject* parent = nullptr );
    QmldirFile( const QmldirFile& file );
    QmldirFile& operator=( const QmldirFile& file );

    QmldirContentMap qml_content( ) const;

private:
    void read_file( );
    QPair< QString, int > find( QRegExp reg, QString line, int pos = 0 );

signals:
    void qml_content_changed( QmldirContentMap qml_content );

private:
    QmldirContentMap m_qml_content{};
    ErrorCode m_error = ErrorCode::NONE;
};
