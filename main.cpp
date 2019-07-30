#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>

#include "src/QmldirTree.hpp"

int
main( int argc, char* argv[] )
{
    QCoreApplication::setAttribute( Qt::AA_EnableHighDpiScaling );

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
