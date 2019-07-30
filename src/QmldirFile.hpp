#pragma once
#include <QFile>
#include <QMultiMap>
#include <QObject>
#include <QPair>

using ContentComponent = QPair< QString /* component */, QString /* .qml file */ >;
using QmldirContentMap = QMultiMap< QString /*version*/, ContentComponent >;

class QmldirFile : public QFile
{
    Q_OBJECT
    Q_PROPERTY( QmldirContentMap qml_content READ qml_content WRITE set_qml_content NOTIFY
                        qml_content_changed )

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

public slots:
    void set_qml_content( QmldirContentMap qml_content );

private:
    void read_file( );
    QPair< QString, int > find( QRegExp reg, QString line, int pos = 0 );

signals:
    void qml_content_changed( QmldirContentMap qml_content );

private:
    QmldirContentMap m_qml_content{};
    ErrorCode m_error = ErrorCode::NONE;
};
