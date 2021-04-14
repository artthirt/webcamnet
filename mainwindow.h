#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include <opencv2/opencv.hpp>

#include "structs.h"

namespace Ui{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void sendMat(const cv::Mat& mat);

    void setFunSetExposure(funsetexposure val);
    void setFunSetAddress(funsetaddress val);

signals:

private slots:
    void on_sbExposure_valueChanged(int arg1);

    void on_hsRed_valueChanged(int value);

    void on_hsGreen_valueChanged(int value);

    void on_hsBlue_valueChanged(int value);

    void on_sbQuality_valueChanged(int arg1);

    void on_chbGamma_clicked(bool checked);

    void on_chbResolution_currentIndexChanged(int index);

    void on_pbSetAddress_clicked();

private:
    Ui::MainWindow *ui = nullptr;

    QTimer mTimer;
    Cntrl mCntrl;

    funsetexposure mSetExposure;
    funsetaddress mSetAddress;

    void loadSettings();
    void saveSettings();
};

#endif // MAINWINDOW_H
