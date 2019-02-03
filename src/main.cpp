#include <QApplication>
#include <QFile>
#include <QDebug>
#include <MainWindow.hpp>

int main(int argc, char ** argv)
{
    QApplication * app = new QApplication(argc,argv);

    QFile style(":/qdarkstyle/style.qss");
    if(style.exists())
    {
        qDebug()<<"Setting style sheet";
        style.open(QFile::ReadOnly);
        qApp->setStyleSheet(style.readAll());
    }

    MainWindow * window = new MainWindow();
    window->show();

    return app->exec();
}
