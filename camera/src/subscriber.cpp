#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/asio/buffer.hpp>
#include <string>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <cstdlib>
#include <vector>
#include <utility>
#include <chrono>
#include <future> 

using boost::asio::ip::udp;

using namespace std::chrono_literals;



enum STATUS {
    READY,
    WAIT
} status_;

class UDPListener{

    public:
        UDPListener(boost::asio::io_context& io_,uint16_t  host_port,uint16_t remote_port):
            remote_endp_(boost::asio::ip::address::from_string("127.0.0.1"), remote_port),
            socket_(io_, udp::endpoint(udp::v4(), host_port))
        {
            if (socket_.is_open()) {
                std::cout<<"Listener was connected to the port 45449!!!"<<std::endl;
            }
            else{
                std::cerr<<"Listener could not connect to the port!!!"<<std::endl;
            }
           
        }
    
        void read()
        {
            start_receive();
        }

    
    private:
        udp::socket socket_;
        udp::endpoint remote_endp_;        
        boost::array<unsigned char, 400000> recv_buffer{};
        
        void start_receive()       
        {                   
            socket_.async_receive_from( 
                boost::asio::buffer(recv_buffer),
                remote_endp_,
                boost::bind(&UDPListener::handle_receive, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred)
            );  
                              
        }

        void handle_receive(const boost::system::error_code& error,
                            std::size_t d)
        {
            if(!error)
            {
                std::cout<<"Data size received: "<<d<<std::endl; 
                start_receive();          
            }
            else{
                std::cerr<<"Error occurred!!!"<<std::endl;
            }
        }
        
}; 


int main()
{
    try
    {   
        uint16_t host_port{45500};
        uint16_t remote_port{45449};
        boost::asio::io_context io;
        UDPListener listener(io, host_port, remote_port);
        std::thread thr(&UDPListener::read,std::ref(listener));
        if(thr.joinable()) thr.join();
        
        io.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}


