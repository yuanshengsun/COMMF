#ifndef _YuanshengSun_LM_Engine_Udp_Recvfrom_H_
#define _YuanshengSun_LM_Engine_Udp_Recvfrom_H_

#include<sys/select.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <thread>
#include <functional>
#include <unordered_map>
#include <memory>
#include <atomic>
#include"../thread/ThreadPool.h"
#include "../crc/crc.h"
#include"../log.h"

namespace YSSLM {
namespace engine {
namespace udp {

using threadpool = YSSLM::engine::threadpool::threadpool;

class Recvfrom
{

public:
    int Init( int port_out_c, uint32_t addr_c = 0){
        port_out_c_ = port_out_c;
        addr_c_ = addr_c;

        sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if(-1==sockfd_){
            ULOG_ERROR("Recvfrom::Init() Failed to create socket !");
            return -1;
        }

        addr_len_ = sizeof(addr_);

        memset(&addr_, 0, sizeof(addr_));
        addr_.sin_family = AF_INET;       // Use IPV4
        addr_.sin_port = htons(port_out_c_);    //
        addr_.sin_addr.s_addr = (addr_c_ == 0 )? htonl(INADDR_ANY) : addr_c_;

        // int opt = 1;
        // setsockopt(sockfd_ , SOL_SOCKET , SO_REUSEPORT , (const void*)&opt , sizeof(opt));
        if (bind(sockfd_, (struct sockaddr*)&addr_, addr_len_) == -1){
            close(sockfd_);
            ULOG_ERROR("Recvfrom::Init() Failed to bind socket on addr %u and port %d !" , addr_c_ , port_out_c_);
            return -2;
        }
        memset(buffer_, 0, sizeof(buffer_));
        
        stop2_ = std::make_shared<std::atomic<bool>>();
        stop_  = std::make_shared<std::atomic<bool>>();

        *stop_ = true;

        return 0;
    }
    void Start(){
        Load();
    }
    void RecvfromAdd(std::string name, std::function<void(std::shared_ptr<std::string>&)> fun,
                int policy0,
				int sched_priority0,
				int cpu_id0,
				int num_thr0){
        if(fun_.find(name) != fun_.end()){
            fun_[name].push_back(fun);
            threadpool_[name].push_back(std::make_shared<threadpool>(num_thr0 , 0 , policy0 , sched_priority0 , cpu_id0));
            return;
        }
        fun_.emplace(name, std::vector<std::function<void(std::shared_ptr<std::string>&)>>());
        fun_[name].push_back(fun);

        threadpool_.emplace(name , std::vector<std::shared_ptr<threadpool>>());
        threadpool_[name].push_back(std::make_shared<threadpool>(num_thr0 , 0 , policy0 , sched_priority0 , cpu_id0));
    }
    void Close(){
        *stop_ = false;

        if(thread01_ != nullptr){
            while (*stop2_)
            {
                sendto(sockfd_, "end", 4, 0, (sockaddr*)&addr_, addr_len_);
                usleep(10000);
            }
            
            if(thread01_->joinable()){
                thread01_->join();
            }
            thread01_ = nullptr;
        }
    
        threadpool_.clear();
        close(sockfd_);
    }

private:
    void Load(){

        thread01_ = std::make_shared<std::thread>([this]{
            *stop2_ = true;
            
            while(*stop_){
                
                src_len_ = sizeof(src_);
                memset(&src_, 0, sizeof(src_));
                
                int sz = recvfrom(sockfd_, buffer_, sizeof(buffer_), 0, (sockaddr*)&src_, &src_len_);
                if (sz >= 6  && buffer_[sz-4] == 'Y' 
                             && buffer_[sz-3] == 'S'
                             && buffer_[sz-2] == 'S'){
 
                    std::shared_ptr<std::string> data_b = std::make_shared<std::string>();
                    data_b->resize(sz);
                    memcpy((char*)data_b->c_str() , buffer_ , sz);

                    if(data_b->back() == crc::CRC::CRC8(*data_b, data_b->size() - 1 )){
                        // data_b->pop_back();
                        short num_b = 0;
                        ((char*)&num_b)[0] = (*data_b)[sz-6];
                        ((char*)&num_b)[1] = (*data_b)[sz-5];

                        if(num_b > 0 && sz >= 6 + num_b){
                            std::string name_b(std::begin(*data_b) + sz - 6 - num_b,std::begin(*data_b) + sz - 6);
                            if(fun_.find(name_b) != fun_.end()){
                                data_b->resize(sz - 6 - num_b);
                                for(int i = 0 ; i < (int)fun_[name_b].size() ; ++i){
                                    auto &fun_b = fun_[name_b][i];
                                    threadpool_[name_b][i]->commit([fun_b](std::shared_ptr<std::string> data_b){
                                        fun_b(data_b);
                                    },data_b);
                                }
                            }
                        }
                    }
                }
            }
            
            *stop2_ = false;
            
        });
        // is_stop_ = true;
    }

    /* data */
    int port_out_c_;
    uint32_t addr_c_;
    int sockfd_;

    struct sockaddr_in addr_;
    socklen_t addr_len_;
    struct sockaddr_in src_;
    socklen_t src_len_;

    char buffer_[5001] = {0};
    std::shared_ptr<std::atomic<bool>> stop_;
    std::shared_ptr<std::atomic<bool>> stop2_;
    // bool is_stop_ = true;

    std::shared_ptr<std::thread> thread01_ = nullptr;
    std::unordered_map<std::string , std::vector<std::function<void(std::shared_ptr<std::string>&)>>> fun_;

    std::unordered_map<std::string , std::vector<std::shared_ptr<threadpool>>> threadpool_;

};


}}}
#endif