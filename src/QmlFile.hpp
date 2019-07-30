#pragma once
#include <QFile>
#include "QmldirFile.hpp"

using ComponentStatus = QPair< QString /*full import name*/, QString /*error*/ >;
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

public slots:
    void set_import_map( QmlImportMap import_map );
    void set_used_components( QStringList used_components );

private:
    void read_file( );
    QPair< QString, int > find( QRegExp reg, QString line, int pos = 0, int cap = 0 );

signals:
    void import_map_changed( QmlImportMap import_map );
    void used_components_changed( QStringList used_components );

private:
    QmlImportMap m_import_map{};
    QStringList m_used_components{};
};
