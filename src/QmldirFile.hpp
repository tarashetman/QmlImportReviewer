#pragma once
#include <QObject>
#include <QFile>
#include <QMultiMap>
#include <QPair>

using Content =  QPair<QString, QString>;
using QmlFileMap = QMultiMap<QString, Content>;

class QmldirFile : public  QFile
{
    Q_OBJECT
    Q_PROPERTY( QmlFileMap qml_content READ qml_content WRITE set_qml_content NOTIFY qml_content_changed)

    enum class ErrorCode {
        NONE,
        EMPTY
    };

public:
    QmldirFile( QObject* parent = nullptr );
    QmldirFile(const QString& name, QObject* parent = nullptr);
    QmldirFile(const QmldirFile& file);
    QmldirFile& operator=(const QmldirFile& file);

    QmlFileMap qml_content() const;

public slots:
    void set_qml_content(QmlFileMap qml_content);

private:
    void read_file();
    QPair<QString, int> find( QRegExp reg, QString line, int pos = 0);

signals:
    void qml_content_changed(QmlFileMap qml_content);

private:
    QmlFileMap m_qml_content{};
    ErrorCode m_error = ErrorCode::NONE;
};
