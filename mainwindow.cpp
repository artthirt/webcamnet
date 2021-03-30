#include "mainwindow.h"

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
  , ui(new Ui::MainWindow())
{
    ui->setupUi(this);
}

void MainWindow::sendMat(const cv::Mat &mat)
{
    QTimer::singleShot(0, this, [=](){
        ui->widget->setImage(mat);
    });
}

void MainWindow::setFunSetExposure(funsetexposure val)
{
    mSetExposure = val;
}

void MainWindow::on_sbExposure_valueChanged(int arg1)
{
    if(mSetExposure){
        mSetExposure(arg1);
    }
}

void MainWindow::on_hsRed_valueChanged(int value)
{
    float val = 1 + 1. * value / 2000;
    ui->widget->setRed(val);
}

void MainWindow::on_hsGreen_valueChanged(int value)
{
    float val = 1 + 1. * value / 2000;
    ui->widget->setGreen(val);

}

void MainWindow::on_hsBlue_valueChanged(int value)
{
    float val = 1 + 1. * value / 2000;
    ui->widget->setBlue(val);
}
