#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QTextStream>

#include "src/QmldirTree.hpp"

void
myMessageOutput( QtMsgType type, const QMessageLogContext& /*context*/, const QString& msg )
{
    QString txt;
    switch ( type )
    {
    case QtDebugMsg:
        txt = QString( "[D] %1" ).arg( msg );
        break;
    case QtWarningMsg:
        txt = QString( "[W] %1" ).arg( msg );
        break;
    case QtCriticalMsg:
        txt = QString( "[C] %1" ).arg( msg );
        break;
    case QtFatalMsg:
        txt = QString( "[I] %1" ).arg( msg );
        break;
    }
    QFile outFile( "log" );
    outFile.open( QIODevice::WriteOnly | QIODevice::Append );
    QTextStream ts( &outFile );
    ts << txt << endl;
}

int
main( int argc, char* argv[] )
{
    QCoreApplication::setAttribute( Qt::AA_EnableHighDpiScaling );

    qInstallMessageHandler( myMessageOutput );  // Install the handler
    QGuiApplication app( argc, argv );

    app.setOrganizationName( "Normas" );
    app.setOrganizationDomain( "normas.taras" );

    qmlRegisterType< Tree >( "base.QmldirTree", 1, 0, "QmldirTree" );
    //    qmlRegisterType< Node >( "base.Node", 1, 0, "Node" );

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
