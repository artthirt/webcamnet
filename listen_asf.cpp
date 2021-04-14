#include "listen_asf.h"

using namespace std;
using namespace cv;
using namespace asio::ip;

Frame::Frame(){
    done = false;
}

Frame::Frame(const bytearray &packet){
    done = false;
    data = packet;
}

Frame::Frame(const Frame &frame){
    done = false;
    data = frame.data;

    thread = std::thread(std::bind(&Frame::run, this));
}

void Frame::run(){
#ifdef _JPEG
    jpeg_decode dec;
#else
    openjp_decode dec;
#endif
    dec.decode(data, image);
    done = true;
}

/////////////////////

listen_asf::listen_asf()
    : m_socket(0)
    , m_port(10000)
    , m_done(false)
{
    mTimeStart = getNow();
}

listen_asf::~listen_asf(){
    m_done = true;
    if(m_socket)
        m_socket.reset();
}

void listen_asf::set_port(u_short port){
    m_port = port;

    if(!m_socket)
        return;

    m_socket->close();
    m_socket->open(udp::v4());

    m_socket->bind(udp::endpoint(asio::ip::address::from_string("0.0.0.0"), port));
}

void listen_asf::close(){
    m_done = true;
    m_io.stop();
}

void listen_asf::run(){
    m_socket.reset(new udp::socket(m_io, udp::endpoint(udp::v4(), m_port)));

    std::thread thread(std::bind(&listen_asf::run2, this));

    std::thread thrrecv(std::bind(&listen_asf::doReceive, this));

    //start_receive();

    m_io.run();

    doTimeout();

    thread.join();
    thrrecv.join();
}

void listen_asf::run2(){
    while(!m_done){
        check_frames();
        _msleep(10);
    }
}

void listen_asf::check_frames(){
    m_mutex.lock();
    if(m_pool.size()){
        if(m_pool.front().done){
            m_images.push(m_pool.front().image);
            m_pool.front().thread.join();
            m_pool.pop();
        }
    }
    m_mutex.unlock();

    if(m_images.size() > max_images){
        while(m_images.size() > max_images){
            m_images.pop();
        }
    }
}

void listen_asf::start_receive(){
    m_socket->async_receive_from(asio::null_buffers(), mRemoteEndpoint,
                            std::bind(&listen_asf::handleReceive, this,
                                      std::placeholders::_1, std::placeholders::_2, mRemoteEndpoint));
}

void listen_asf::handleReceive(const asio::error_code &error, size_t size, udp::endpoint ep)
{
    size_t available = m_socket->available();
    if(!available)
        return;
    m_buffer.resize(available);

    mRemoteEndpoint = ep;

    std::cout << ep.port() << "\n";

    size_t packetSize = m_socket->receive(asio::buffer(m_buffer, available));

    bytearray packet;
    packet.resize(packetSize);
    std::copy(m_buffer.begin(), m_buffer.begin() + packetSize, packet.begin());

    analyze_data(packet);

    start_receive();
}

void listen_asf::analyze_data(const bytearray &data)
{
    mTimeStart = getNow();

    bytearray buffer = m_asf.add_packet(data);

    if(m_pool.size() >= max_threads)
        return;

    if(buffer.size()){
        m_pool.push(Frame(buffer));
    }
}

void listen_asf::set_cntrl(const Cntrl &val)
{
    bytearray data;
    data.resize(4 * 10, 0);
    int* di = reinterpret_cast<int*>(data.data());
    di[0] = val.exposure;
    di[1] = val.quality;
    di[2] = val.resolutionid;

    m_socket->send_to(asio::buffer(data), mRemoteEndpoint);
}

void listen_asf::setAddress(const string &ip, uint16_t port)
{
    mRemoteEndpoint.address(asio::ip::address_v4::from_string(ip));
    mRemoteEndpoint.port(port);
}

bool listen_asf::empty() const{
    return m_images.size() == 0;
}

Mat listen_asf::pop(){
    Mat m = m_images.front();
    m_images.pop();
    return m;
}

void listen_asf::doReceive()
{
    bytearray packet, copypkt;
    packet.resize(65536);
    size_t packetSize = 0;

    asio::ip::udp::endpoint ep;

    asio::socket_base::message_flags flags = 0;
    asio::error_code err;
    for(;!m_done;){
        packetSize = m_socket->receive_from(asio::buffer(packet), ep, flags, err);
        if(packetSize){
            copypkt.resize(packetSize);
            std::copy(packet.begin(), packet.begin() + packetSize, copypkt.begin());
            analyze_data(copypkt);
        }else{
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
}

void listen_asf::doTimeout()
{
    for(;!m_done;){
        sendLive();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void listen_asf::sendLive()
{
    if(!mRemoteEndpoint.port())
        return;

    bytearray data;
    data.resize(4 * 10, 0);
    int* di = reinterpret_cast<int*>(data.data());
    di[0] = 0xffffffff;

    m_socket->send_to(asio::buffer(data), mRemoteEndpoint);
}
