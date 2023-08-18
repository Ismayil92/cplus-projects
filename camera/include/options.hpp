#ifndef OPTIONS_H
#define OPTIONS_H

#include <cxxopts.hpp>
#include <cstdlib>

class ArgParser{
    public:
        ArgParser():
                options_{"camera", "Parsing arguments"}
        {
            options_.add_options()
                ("remote-device", "Ip address of the remote device",
                cxxopts::value<std::string>()->default_value("127.0.0.1"))
                ("remote-port", "port number of the remote device",
                cxxopts::value<uint16_t>()->default_value("45449"))
                ("host-port", "port number of the host device",
                cxxopts::value<uint16_t>()->default_value("45500"))
                ("h,help", "Print usage");
        }
        virtual ~ArgParser()
        {

        }
        void parse(int argc, char** argv)
        {
            result_ = options_.parse(argc, argv);
            remote_ip = result_["remote-device"].as<std::string>();
            remote_port = result_["remote-port"].as<uint16_t>();
            host_port = result_["host-port"].as<uint16_t>();
        }


        void help()
        {
            if (result_.count("help"))
            {
                std::cout << options_.help() << std::endl;
                std::exit(EXIT_SUCCESS);
            }
        }

        std::string getRemoteIP() const {return remote_ip;}
        uint16_t getRemotePort() const {return remote_port;}
        uint16_t getHostPort() const {return host_port;}


    private:
            cxxopts::Options options_;
            cxxopts::ParseResult result_;
            uint16_t remote_port;
            uint16_t host_port;
            std::string remote_ip; 
    

};


#endif