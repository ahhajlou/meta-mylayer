#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "bloc_dev_monitor.h"



extern "C" void callback_bridge() {
    qDebug() << "Hey, We are now in C++, it means callback function worked like charm!";
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->widget->hide();




    register_callback_with_data(callback_bridge);
    udev_dev_mon_main_loop();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    ui->widget->hide();
}


void MainWindow::on_pushButton_3_clicked()
{
    ui->widget->show();
}

