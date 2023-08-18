#include <iostream>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <future>
#include <vector>
#include <queue>
#include <thread>
#include "options.hpp"

using namespace std::chrono_literals;
using boost::asio::ip::udp;

static std::vector<unsigned char> img_buffer;
static std::queue<cv::Mat> img_queue;



enum STATUS {
    READY,
    WAIT
} status_;


void connectionHandler(const boost::system::error_code& err){
    if(!err)
    {
        std::cout<<"Connection build successfully!!!"<<std::endl;
        status_ = READY;
    }
}


void sendHandler(const boost::system::error_code& err){
    if(!err)
    {
       std::cout<<"Data completely sent!!!"<<img_buffer.size()<<std::endl;
    }
}

bool transferData(const std::string remote_ip_,
                    const uint16_t host_port_,
                    const uint16_t remote_port_)
{
    boost::asio::io_context io;
    udp::endpoint host_ep{udp::v4(), host_port_};
    udp::endpoint remote_ep{boost::asio::ip::address_v4::from_string(remote_ip_), 
                                    remote_port_};
    udp::socket socket_{io, host_ep};

    socket_.async_connect(remote_ep, connectionHandler);
    if(socket_.is_open())
    {
        while(true){
            std::cout<<"Data with size: "<<
                        img_buffer.size()<<" is being transferred!!!\n";
    
            if(status_ == READY)
            {
               
                socket_.async_send_to(boost::asio::buffer(img_buffer, img_buffer.size()), 
                                        remote_ep,
                                        boost::bind(&sendHandler,
                                                    boost::asio::placeholders::error));
                io.run();
                                            
            }
            std::this_thread::sleep_for(3ms);
        }        
    }
    else{
        std::cerr<<"Socket is not open!!!";
        std::exit(EXIT_FAILURE);
    }
    return true;
}

bool getStream()
{   
    cv::VideoCapture cap("/dev/video0");
    cv::Mat img;
    if(!cap.isOpened())
    {
        std::cerr<<"Video device could not open!!!\n";
        std::exit(EXIT_FAILURE);
    }
    
    while(true)
    {
        std::cout<<"Video Streaming is running!!!\n";
        cap.read(img);
        img_queue.push(img);
        cv::imshow("image", img);
        cv::waitKey(3);
    }
    cap.release(); 
    return true;
}




int main(int argc, char** argv)
{
    ArgParser args;
    args.parse(argc, argv);

    cv::Mat img_;
    const uint16_t host_port{args.getHostPort()};
    const uint16_t remote_port{args.getRemotePort()};
    const std::string remote_ip{args.getRemoteIP()};
    try
    {

        std::future<bool> video_streamer{std::async(std::launch::async,
                                         getStream)};

        std::future<bool> data_publisher{std::async(std::launch::async,
                                         transferData,
                                         remote_ip,
                                         host_port,
                                         remote_port
                                         )};
        
        while(true)
        {        
            std::future_status publisher_status_{data_publisher.wait_for(1ms)};  
            std::future_status streamer_status_{video_streamer.wait_for(1ms)};
            
            if(!img_queue.empty())
            {
                std::cout<<"Image Encoder running!!!\n";
                img_ = img_queue.front();
                cv::imencode(".jpg",
                        img_,
                        img_buffer, //this is std::vector
                        {cv::IMWRITE_JPEG_QUALITY, 100});
                if(img_queue.size()>1)
                {
                   img_queue.pop();       
                }                              
            }
            else
            {
                std::cout<<"Image buffer queue is empty!!!\n";
            }  

            std::this_thread::sleep_for(5ms);                 
        }
                
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}