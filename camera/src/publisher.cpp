#include <iostream>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <string>


using boost::asio::ip::udp;

enum STATUS {
    READY,
    WAIT
} status_;


void connectionHandler(const boost::system::error_code& err){
    if(!err)
    {
        std::cout<<"Connection build successfully!!!"<<std::endl;
        status_ = WAIT;
    }
}

void waitHandler(const boost::system::error_code& err){
    if(!err)
    {
        std::cout<<"Connection is ready to send data!!!"<<std::endl;
        status_ = READY;
    }
}

void sendHandler(const boost::system::error_code& err){
    if(!err)
    {
       std::cout<<"Data completely sent!!!"<<std::endl;
    }
}
int main()
{

    try
    {

        
        char buffer[] = "Data to send\r\n";
        
        boost::asio::io_context io;
        uint16_t host_port{45449};
        uint16_t remote_port{45500};
        udp::endpoint host_ep{udp::v4(), host_port};
        udp::endpoint remote_ep{boost::asio::ip::address_v4::from_string("127.0.0.1"), 
                                remote_port};
        udp::socket socket_{io, host_ep};

        socket_.async_connect(remote_ep, connectionHandler);
        if(socket_.is_open())
        {
            socket_.async_wait(udp::socket::wait_write, waitHandler);
            if(status_ == READY)
            {
                socket_.async_send_to(boost::asio::buffer(buffer, 1024), 
                                    remote_ep,
                                    boost::bind(&sendHandler,
                                                boost::asio::placeholders::error));
                                        
            }
        }
        io.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}