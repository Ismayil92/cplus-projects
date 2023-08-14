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
#include <mutex>
#include <condition_variable>
#include <thread>

using namespace std::chrono_literals;
using boost::asio::ip::udp;

static std::vector<unsigned char> img_buffer;

static std::mutex mx;
static std::queue<cv::Mat> img_queue;
boost::asio::io_context io;
static uint16_t host_port{45449};
static uint16_t remote_port{45500};
static udp::endpoint host_ep{udp::v4(), host_port};
static udp::endpoint remote_ep{boost::asio::ip::address_v4::from_string("127.0.0.1"), 
                                remote_port};
static udp::socket socket_{io, host_ep};


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
       std::cout<<"Data completely sent!!!"<<img_buffer.size()<<std::endl;
    }
}

bool transferData()
{
    socket_.async_connect(remote_ep, connectionHandler);
    if(socket_.is_open())
    {
        while(true){
            std::cout<<"Data is being transferred!!!\n";
            socket_.async_wait(udp::socket::wait_write, waitHandler);
            if(status_ == READY)
            {
                socket_.async_send_to(boost::asio::buffer(img_buffer), 
                                        remote_ep,
                                        boost::bind(&sendHandler,
                                                    boost::asio::placeholders::error));
                                            
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




int main()
{
    cv::Mat img_;
    try
    {

        std::future<bool> video_streamer{std::async(std::launch::async,
                                         getStream)};

        std::future<bool> data_publisher{std::async(std::launch::async,
                                         transferData)};
        
        while(true)
        {        
            std::future_status publisher_status_{data_publisher.wait_for(1ms)};  
            std::future_status streamer_status_{video_streamer.wait_for(1ms)};
            
            if(img_queue.size()<12 && img_queue.size()>1)
            {
                std::cout<<"Image Encoder running!!!\n";
                img_ = img_queue.front();
                cv::imencode(".jpg",
                        img_,
                        img_buffer, //this is std::vector
                        {cv::IMWRITE_JPEG_QUALITY, 100});
                img_queue.pop();                
            }
            else if(img_queue.size()==0) 
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