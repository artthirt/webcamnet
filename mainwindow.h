#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <opencv2/opencv.hpp>

#include <functional>

typedef std::function<void(int)> funsetexposure;

namespace Ui{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

    void sendMat(const cv::Mat& mat);

    void setFunSetExposure(funsetexposure val);

signals:

private slots:
    void on_sbExposure_valueChanged(int arg1);

    void on_hsRed_valueChanged(int value);

    void on_hsGreen_valueChanged(int value);

    void on_hsBlue_valueChanged(int value);

private:
    Ui::MainWindow *ui = nullptr;

    funsetexposure mSetExposure;
};

#endif // MAINWINDOW_H
