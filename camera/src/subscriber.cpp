#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio/buffer.hpp>
#include <string>
#include <algorithm>
#include <cstdint>
#include <cstring>

using boost::asio::ip::udp;


class UDPListener{

    public:
        UDPListener(boost::asio::io_context& io_):
            remote_endp_(boost::asio::ip::address::from_string("127.0.0.1"), 45449),
            socket_(io_, udp::endpoint(udp::v4(), 45500))
        {
            if (socket_.is_open()) {
                std::cout<<"Listener was connected to the port 45449!!!"<<std::endl;
            }
            else{
                std::cerr<<"Listener could not connect to the port!!!"<<std::endl;
            }
           
        }
    
    void readData(){
        start_receive();
    }
    private:
        udp::socket socket_;
        udp::endpoint remote_endp_;
        
        boost::array<char, 1024> recv_buffer_{};
        
        void start_receive()       
        {
            
            socket_.async_receive_from( 
                boost::asio::buffer(recv_buffer_),
                remote_endp_,
                boost::bind(&UDPListener::handle_receive, this, 
                        recv_buffer_,
                        boost::asio::placeholders::error,
                        sizeof(recv_buffer_))
            );                     
            
        }

        void handle_receive(boost::array<char, 1024> buffer_,
                            const boost::system::error_code& error,
                            std::size_t d)
        {
            if(!error)
            {
                std::cout<<"Data size received: "<<d<<std::endl;
                for(const char& c : buffer_){
                    if(isascii(c))  std::cout<<c;
                }
                 
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
        boost::asio::io_context io;
        UDPListener listener(io);
        listener.readData();
        io.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}