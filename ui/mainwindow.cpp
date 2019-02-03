#include "MainWindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    container = new TabContainer();
    this->setCentralWidget(container);
}

MainWindow::~MainWindow()
{
    delete ui;
}
