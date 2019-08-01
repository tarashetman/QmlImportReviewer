#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QTextStream>
#include <QTime>
#include <iostream>

#include "src/QmldirTree.hpp"
//#define LOGGING_TO_FILE

#ifdef LOGGING_TO_FILE
static QFile S_LOG_FILE;
#endif

void
myMessageOutput( QtMsgType type, const QMessageLogContext& /*context*/, const QString& msg )
{
    QString txt;
    switch ( type )
    {
    case QtDebugMsg:
        txt = QString( "%1 [D] %2" )
                      .arg( QTime::currentTime( ).toString( Qt::ISODate ) )
                      .arg( msg );
        break;
    case QtWarningMsg:
        txt = QString( "%1 [W] %2" )
                      .arg( QTime::currentTime( ).toString( Qt::ISODate ) )
                      .arg( msg );
        break;
    case QtCriticalMsg:
        txt = QString( "%1 [C] %2" )
                      .arg( QTime::currentTime( ).toString( Qt::ISODate ) )
                      .arg( msg );
        break;
    case QtFatalMsg:
        txt = QString( "%1 [F] %2" )
                      .arg( QTime::currentTime( ).toString( Qt::ISODate ) )
                      .arg( msg );
        break;
    case QtInfoMsg:
        txt = QString( "%1 [I] %2" )
                      .arg( QTime::currentTime( ).toString( Qt::ISODate ) )
                      .arg( msg );
        break;
    }

#ifdef LOGGING_TO_FILE
    QTextStream ts( &S_LOG_FILE );
    ts << txt << endl;
#else
    std::cout << txt.toStdString( ) << std::endl;
#endif
}

int
main( int argc, char* argv[] )
{
#ifdef LOGGING_TO_FILE
    S_LOG_FILE.setFileName( "reviewer.log" );
    S_LOG_FILE.remove( );
    S_LOG_FILE.open( QIODevice::WriteOnly | QIODevice::Append );
#endif
    qInstallMessageHandler( myMessageOutput );

    QCoreApplication::setAttribute( Qt::AA_EnableHighDpiScaling );

    QGuiApplication app( argc, argv );

    app.setOrganizationName( "Normas" );
    app.setOrganizationDomain( "normas.taras" );

    qmlRegisterType< Tree >( "base.QmldirTree", 1, 0, "QmldirTree" );

    Tree* qmldir_tree = new Tree;

    QQmlApplicationEngine engine;

    engine.rootContext( )->setContextProperty( "qmldir_tree", qmldir_tree );
    const QUrl url( QStringLiteral( "qrc:/main.qml" ) );
    QObject::connect( &engine,
                      &QQmlApplicationEngine::objectCreated,
                      &app,
                      [url]( QObject* obj, const QUrl& objUrl ) {
                          if ( !obj && url == objUrl )
                              QCoreApplication::exit( -1 );
                      },
                      Qt::QueuedConnection );
    engine.load( url );

    return app.exec( );
}
