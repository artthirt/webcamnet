#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <asio/ip/udp.hpp>
#include <asio/io_service.hpp>
#include <asio/placeholders.hpp>
#include <thread>
#include <queue>
#include <mutex>
#include <functional>

#include <QApplication>

#include "mainwindow.h"

#include "common.h"
#include "datastream.h"
#include "asf_stream.h"

#include "listen_asf.h"

using namespace std;
using namespace cv;
using namespace asio::ip;

bool done = false;

typedef std::function<void(const cv::Mat&)> funsendmat;

void listen_thread(listen_asf* listen, funsendmat sendmat)
{
    while(!done){

        if(!listen->empty()){
            Mat m = listen->pop();
            if(!m.empty() && sendmat){
                sendmat(m);
            }
        }

        char key = waitKey(10);
        if(key == 27){
            break;
        }
    }
    listen->close();
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MainWindow w;
    w.show();

	listen_asf listen;

    std::thread thread(std::bind(&listen_asf::run, &listen));

    auto fn = [&w](const cv::Mat& m){
        w.sendMat(m);
    };

    auto setexp = [&listen](const Cntrl& val){
        listen.set_cntrl(val);
    };
    auto setaddr = [&listen](const std::string& ip, uint16_t p){
        listen.setAddress(ip, p);
    };

    w.setFunSetExposure(setexp);
    w.setFunSetAddress(setaddr);

    std::thread thread2(listen_thread, &listen, fn);

    int res =  app.exec();

    done = true;

    listen.close();

    thread.join();
    thread2.join();

    return res;
}

