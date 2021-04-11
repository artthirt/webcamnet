#ifndef LISTEN_ASF_H
#define LISTEN_ASF_H

#include <thread>
#include <vector>
#include <queue>

#include <asio/ip/udp.hpp>
#include <asio/io_service.hpp>
#include <asio/placeholders.hpp>

#include <opencv2/opencv.hpp>

#include "common.h"
#include "datastream.h"
#include "asf_stream.h"

#include "structs.h"

#ifdef _JPEG
#include "jpeg_decode.h"
#else
#include "openjp_decode.h"
#endif

struct Frame{
    Frame();
    Frame(const bytearray& packet);
    Frame(const Frame& frame);

    void run();

    std::thread thread;
    cv::Mat image;
    bytearray data;
    bool done;
};

class listen_asf{
public:
    enum{
        max_images = 20,
        max_threads = 1
    };

    listen_asf();

    ~listen_asf();

    void set_port(uint16_t port);

    void close();

    void run();

    void run2();

    void check_frames();

    void start_receive();

    void handleReceive(const asio::error_code &error, size_t size, asio::ip::udp::endpoint ep);

    void analyze_data(const bytearray& data);

    void set_cntrl(const Cntrl &val);

    bool empty() const;

    cv::Mat pop();

private:
    std::shared_ptr<asio::ip::udp::socket> m_socket;
    asio::io_service m_io;
    asf_stream m_asf;
    u_short m_port;
    std::queue< Frame > m_pool;
    std::queue< cv::Mat > m_images;
    std::mutex m_mutex;
    bytearray m_buffer;

    asio::ip::udp::endpoint mRemoteEndpoint;

    bool m_done;

    void doReceive();
};

#endif // LISTEN_ASF_H
