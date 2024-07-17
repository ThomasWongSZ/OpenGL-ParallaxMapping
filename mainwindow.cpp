#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setFixedSize(QSize(800,600));
    ui->glWidget->resize(QSize(800,600));
    ui->glWidget->move(0,0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

