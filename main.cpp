#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/thread.hpp>
#include <queue>
#include <mutex>

#include "common.h"
#include "datastream.h"
#include "asf_stream.h"

#ifdef _JPEG
#include "jpeg_decode.h"
#else
#include "openjp_decode.h"
#endif

using namespace std;
using namespace cv;
using namespace boost::asio::ip;

struct Frame{
	Frame(){
		done = false;
	}
	Frame(const bytearray& packet){
		done = false;
		data = packet;
	}
	Frame(const Frame& frame){
		done = false;
		data = frame.data;

		thread = boost::thread(boost::bind(&Frame::run, this));
	}

	void run(){
#ifdef _JPEG
		jpeg_decode dec;
#else
		openjp_decode dec;
#endif
		dec.decode(data, image);
		done = true;
	}

	boost::thread thread;
	Mat image;
	bytearray data;
	bool done;
};

class listen_asf{
public:
	enum{
		max_images = 20,
		max_threads = 1
	};

	listen_asf()
		: m_socket(0)
		, m_port(10000)
		, m_done(false)
	{

	}

	~listen_asf(){
		m_done = true;
		if(m_socket)
			delete m_socket;
	}

	void set_port(u_short port){
		m_port = port;

		if(!m_socket)
			return;

		m_socket->close();
		m_socket->open(udp::v4());

		m_socket->bind(udp::endpoint(boost::asio::ip::address::from_string("0.0.0.0"), port));
	}

	void close(){
		m_done = true;
		m_io.stop();
	}

	void run(){
		m_socket = new udp::socket(m_io, udp::endpoint(udp::v4(), m_port));

		boost::thread thread(boost::bind(&listen_asf::run2, this));

		start_receive();

		m_io.run();

		thread.join();
	}

	void run2(){
		while(!m_done){
			check_frames();
			_msleep(10);
		}
	}

	void check_frames(){
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

	void start_receive(){
		m_socket->async_receive(boost::asio::null_buffers(),
								boost::bind(&listen_asf::handleReceive, this,
											boost::asio::placeholders::error,
											boost::asio::placeholders::bytes_transferred));
	}

	void handleReceive(const boost::system::error_code &error, size_t size)
	{
		size_t available = m_socket->available();
		m_buffer.resize(available);

		size_t packetSize = m_socket->receive(boost::asio::buffer(m_buffer, available));

		bytearray packet;
		packet.resize(packetSize);
		std::copy(m_buffer.begin(), m_buffer.begin() + packetSize, packet.begin());

//		stringstream ss;
//		for(size_t i = 0; i < std::min(packetSize, (size_t)10); i++){
//			ss << (uint)packet[i] << ", ";
//		}

		analyze_data(packet);

		//cout << "byte received: " << packetSize << "; error code: " << error;
		//cout << "; packet size: " << packet.size() << " data: [" << ss.str() << "..]" << endl;
		start_receive();
	}

	void analyze_data(const bytearray& data){
		bytearray buffer = m_asf.add_packet(data);

		if(m_pool.size() >= max_threads)
			return;

		if(buffer.size()){
			m_pool.push(Frame(buffer));
		}
	}

	bool empty() const{
		return m_images.size() == 0;
	}

	Mat pop(){
		Mat m = m_images.front();
		m_images.pop();
		return m;
	}

private:
	udp::socket* m_socket;
	boost::asio::io_service m_io;
	asf_stream m_asf;
	u_short m_port;
	queue< Frame > m_pool;
	queue< Mat > m_images;
	mutex m_mutex;
	bytearray m_buffer;
	bool m_done;
};

int main()
{
	cout << "Hello World!" << endl;

	listen_asf listen;

	boost::thread thread(boost::bind(&listen_asf::run, &listen));

	namedWindow("webcam", WINDOW_NORMAL);

	bool done = false;
	while(!done){

		if(!listen.empty()){
			Mat m = listen.pop();

			imshow("webcam", m);
		}

		char key = waitKey(10);
		if(key == 27){
			break;
		}
	}
	listen.close();

	thread.join();

	return 0;
}

