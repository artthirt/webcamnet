#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <QSettings>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
  , ui(new Ui::MainWindow())
{
    ui->setupUi(this);

    loadSettings();

    mTimer.start(60 * 1000);

    connect(&mTimer, &QTimer::timeout, this, [=](){
        saveSettings();
    });
}

MainWindow::~MainWindow()
{
    saveSettings();
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
        mCntrl.exposure = arg1;
        mSetExposure(mCntrl);
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

void MainWindow::on_sbQuality_valueChanged(int arg1)
{
    if(mSetExposure){
        mCntrl.quality = arg1;
        mSetExposure(mCntrl);
    }
}

void MainWindow::on_chbGamma_clicked(bool checked)
{
    ui->widget->setGamma(checked);
}

void MainWindow::loadSettings()
{
    QSettings settings(QSettings::UserScope, "webcamnetapp");
    ui->hsRed->setValue(settings.value("red").toInt());
    ui->hsGreen->setValue(settings.value("green").toInt());
    ui->hsBlue->setValue(settings.value("blue").toInt());
}

void MainWindow::saveSettings()
{
    QSettings settings(QSettings::UserScope, "webcamnetapp");
    settings.setValue("red", ui->hsRed->value());
    settings.setValue("green", ui->hsGreen->value());
    settings.setValue("blue", ui->hsBlue->value());

}

void MainWindow::on_chbResolution_currentIndexChanged(int index)
{
    if(mSetExposure){
        mCntrl.resolutionid = index;
        mSetExposure(mCntrl);
    }
}
