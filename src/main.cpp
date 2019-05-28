#include <QCoreApplication>
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <iostream>
#include "ConfigParser.hpp"
#include "SettingsProvider.hpp"
#include "StaticDocuments.hpp"

void logMessageHandler(QtMsgType type, const QMessageLogContext &,
                       const QString &msg) {
  switch (type) {
    case QtDebugMsg: {
#ifdef QT_QML_DEBUG
      std::cout << "DEBUG: " << msg.toStdString() << std::endl;
#endif
    } break;
    case QtWarningMsg: {
#ifdef QT_QML_DEBUG
      std::cout << "WARN: " << msg.toStdString() << std::endl;
#endif
    } break;
    case QtInfoMsg: {
#ifdef QT_QML_DEBUG
      std::cout << "INFO: " << msg.toStdString() << std::endl;
#endif
    } break;
    case QtCriticalMsg: {
#ifdef QT_QML_DEBUG
      std::cout << "CRITICAL: " << msg.toStdString() << std::endl;
#endif
    } break;
    default:
      break;
  }
}

int main(int argc, char **argv) {
  // install out own message logger
  qInstallMessageHandler(logMessageHandler);

  // force using smaller controls before creating
  // the qt application instance
  qputenv("QT_QUICK_CONTROLS_MATERIAL_VARIANT", "Dense");

  QGuiApplication app(argc, argv);
  app.setWindowIcon(QIcon(":/images/logo.png"));
  QQuickStyle::setStyle("Material");
  // register config parser to be importable in qml
  qmlRegisterType<ConfigParser>("USBasp.pp", 1, 0, "ConfigParser");

  QQmlApplicationEngine qmlEngine;
  // register various c++ objects/instances for usage from within qml source
  // before loading main qml application
  qmlEngine.rootContext()->setContextProperty("staticDocuments",
                                              StaticDocuments::getInstance());
  qmlEngine.rootContext()->setContextProperty("settingsProvider",
                                              SettingsProvider::getInstance());
  // load qml application
  qmlEngine.load(QUrl("qrc:/ui/MainWindow.qml"));
  return app.exec();
}
