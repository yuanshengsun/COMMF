#include "log_udp.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include "../log.h"

namespace YSSLM {
namespace engine {
namespace logudp{


namespace {
    std::string GetCurrentTime(int a1,int a2,int a3,int a4,int a5,int a6){
        log_udp_time log_udp_time_da;
        log_udp_time_da.tm_year = a1;
        log_udp_time_da.tm_mon = a2;
        log_udp_time_da.tm_mday = a3;
        log_udp_time_da.tm_hour = a4;
        log_udp_time_da.tm_min = a5;
        log_udp_time_da.tm_sec = a6;
        return log_udp_time_da.get();
    }
    std::string GetCurrentTime(const char *startTimes, std::string & startTimes_binry){
        if(startTimes[0] == 'n' && startTimes[1] == 'u' && startTimes[2] == 'l' && startTimes[3] == 'l'){
            std::time_t cur_time = std::time(nullptr);
            std::tm* local_time = std::localtime(&cur_time);
            std::stringstream ss;
            ss << std::setfill('0') << std::setw(4) << local_time->tm_year + 1900 <<
                std::setw(2) << local_time->tm_mon + 1 << 
                std::setw(2) << local_time->tm_mday << "_" <<
                std::setw(2) << local_time->tm_hour << 
                std::setw(2) << local_time->tm_min << 
                std::setw(2) << local_time->tm_sec;

            startTimes_binry = GetCurrentTime( 
                                                local_time->tm_year,
                                                local_time->tm_mon,
                                                local_time->tm_mday,
                                                local_time->tm_hour,
                                                local_time->tm_min,
                                                local_time->tm_sec
                );

            return ss.str();
        }
        else{
            log_udp_time log_udp_time_da;
            log_udp_time_da.set(startTimes+8);

            int tm_year = log_udp_time_da.tm_year;
            int tm_mon = log_udp_time_da.tm_mon;
            int tm_mday = log_udp_time_da.tm_mday;
            int tm_hour = log_udp_time_da.tm_hour;
            int tm_min = log_udp_time_da.tm_min;
            int tm_sec = log_udp_time_da.tm_sec;
            
            std::stringstream ss;
            ss << std::setfill('0') << std::setw(4) << tm_year + 1900 <<
                std::setw(2) << tm_mon + 1 << 
                std::setw(2) << tm_mday << "_" <<
                std::setw(2) << tm_hour << 
                std::setw(2) << tm_min << 
                std::setw(2) << tm_sec;
            
            startTimes_binry = GetCurrentTime( 
                                                tm_year , tm_mon , tm_mday , tm_hour , tm_min , tm_sec
                );

            return ss.str();
        }
    }
}

bool CLogUdp::Initializa(const char *startTimes , const YAML::Node& node2){
    printf(" \033[33m==>\033[0m \033[32m[CLogUdp] Initializa() in\033[0m\n");

    if (!node2["Name"].IsDefined()) {
        return false;
    }

    if (!node2["File"].IsDefined()) {
        return false;
    }
 
    if (!node2["Terminal"].IsDefined()) {
        return false;
    }

    if (!node2["UDP"].IsDefined()) {
        return false;
    }

    if (!node2["Terminal"]["OutputLevel"].IsDefined()) {
        return false;
    }
 
    if (!node2["File"]["SavePath"].IsDefined()) {
        return false;
    }

    if (!node2["File"]["OutputLevel"].IsDefined()) {
        return false;
    }

    if (!node2["UDP"]["SendAddrPort"].IsDefined()) {
        return false;
    }

    if (!node2["UDP"]["OutputLevel"].IsDefined()) {
        return false;
    }

    if(node2["Name"].IsNull()){
        return false;
    }
    name_ = node2["Name"].as<std::string>();


    if(node2["Terminal"]["OutputLevel"].IsNull()){
        return false;
    }
    level_ctrl_ = node2["Terminal"]["OutputLevel"].as<int>();


    if(node2["File"]["SavePath"].IsNull()){
        save_file_path_.clear();
    }
    else{
        save_file_path_ = node2["File"]["SavePath"].as<std::string>();
    }
    save_file_path_dir_ = save_file_path_;
    
    if(node2["File"]["OutputLevel"].IsNull()){
        return false;
    }
    level_file_ = node2["File"]["OutputLevel"].as<int>();
    

    if(node2["UDP"]["OutputLevel"].IsNull()){
        return false;
    }
    level_udp_ = node2["UDP"]["OutputLevel"].as<int>();


    uint32_t udp_addr;
    int udp_port;
    if(node2["UDP"]["SendAddrPort"].IsNull()){
        udp_addr = 0;
        udp_port = -1;
    }
    else{
        auto udp_s = node2["UDP"]["SendAddrPort"].as<std::string>();
        int udp_s_i = 0;
        for( ; udp_s_i< (int)udp_s.size() ; ++udp_s_i ){
            if(udp_s[udp_s_i] == ':'){
                break;
            }
        }
        if(udp_s_i==0 || udp_s_i == (int)udp_s.size()){
            return false;
        }
        std::string udp_addr_s(std::begin(udp_s) + 0 ,std::begin(udp_s) + udp_s_i);
        udp_addr = (udp_addr_s == "0")? 0 :inet_addr(udp_addr_s.c_str());
        std::string udp_port_s(std::begin(udp_s) + udp_s_i +1 , std::end(udp_s));
        udp_port = atoi(udp_port_s.c_str());
        
    }

    addr_c_ = udp_addr;
    port_c_ = udp_port;

    if (!Initializa(startTimes)){
        return false;
    }
    printf(" \033[33m==>\033[0m \033[32m[CLogUdp] Initializa() out\033[0m\n");
    return true;
}
bool CLogUdp::Initializa(const char *startTimes){
    save_file_path_b_ = false;
    if((int)save_file_path_.size() != 0){
        save_file_path_b_ = true;
    }

    StartGetTime(startTimes);
    GetCurrentTime_ = GetCurrentTime(startTimes , startTimes_binry_);
    if(save_file_path_b_){
        
        // save_file_path_ += "/log_" + name_ +".txt";
        save_file_path_ += "/log_"  +   name_  + "_"  +  GetCurrentTime_  +".txt";

        save_file_.open(save_file_path_,std::ios::trunc);
        if(!save_file_.is_open()){
            printf(" \033[33m==>\033[0m \033[31m[CLogUdp] Initializa() save_file_(%s) is not open\033[0m\n" , save_file_path_.c_str());
            return false;
        }
    }

    if(port_c_ >= 0){
        sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if(-1==sockfd_){
            printf(" \033[33m==>\033[0m \033[31m[CLogUdp] Initializa() Failed to create socket\033[0m\n");
            return false;
            puts("Failed to create socket");
        }
        addr_len_=sizeof(addr_);

        memset(&addr_, 0, sizeof(addr_));
        addr_.sin_family = AF_INET;
        addr_.sin_port   = htons(port_c_);
        addr_.sin_addr.s_addr = (addr_c_ == 0 )? htonl(INADDR_ANY) : addr_c_;
    }

    c_thread_pool_ = std::make_shared<threadpool::threadpool>(1,0,
                _YSSLM_THREADATTR_INIT_policy_ulog_,
                _YSSLM_THREADATTR_INIT_sched_priority_ulog_,
                _YSSLM_THREADATTR_INIT_cpu_id_ulog_);

    return true;
}

void CLogUdp::DeInitializa(){

    if(save_file_path_b_ && save_file_.is_open()){
        save_file_.close();
    }
    c_thread_pool_ = nullptr;
    if(port_c_ >= 0){
        close(sockfd_);
    }
}
std::string CLogUdp::GetStartTimes(){
    std::string startTimes;
    startTimes.resize(8);
    for(int i = 0; i < 8 ;  ++i){
        startTimes[i] = ((char*)&startTime_)[i];
    } 
    startTimes += startTimes_binry_;

    for(int i = 0; i < 32 ;  ++i){
        if(startTimes[i] == '\0'){
            startTimes[i] = '*';
        }
    } 
    return startTimes;

}

bool CLogUdp::SetPTPRelativeTime(double time){
    ULOG_INFO("CLogUdp::SetPTPRelativeTime() startTime_[%lf] + time[%lf] = [%lf] !" , startTime_  , time , startTime_ + time);
    if(time < 1e4 && time > -1e4 ){
        return true;
    }

    std::lock_guard<std::mutex> lock (mutex_GetRelativeTime_);
    startTime_ = startTime_ + time;

    GetCurrentTime_ = GetCurrentTime("null" , startTimes_binry_);

    if(save_file_path_b_){
        if(!save_file_.is_open()){
            ULOG_ERROR("CLogUdp::SetPTPRelativeTime() save_file_(%s) is not open error !" , save_file_path_.c_str());
            return false;
        }
        save_file_.close();

        auto path2 = save_file_path_;
        save_file_path_ = save_file_path_dir_ + "/log_"  +   name_  + "_"  +  GetCurrentTime_  +".txt";
        if(rename(path2.c_str() , save_file_path_.c_str()) != 0){
            ULOG_ERROR("CLogUdp::SetPTPRelativeTime() rename(%s,%s) error !" ,path2.c_str() , save_file_path_.c_str());
            return false;
        }
        
        save_file_.open(save_file_path_,std::ios::app);
        if(!save_file_.is_open()){
            ULOG_ERROR("CLogUdp::SetPTPRelativeTime() save_file_(%s) is not open error !" , save_file_path_.c_str());
            return false;
        }

        ULOG_INFO("CLogUdp::SetPTPRelativeTime() rename(%s,%s) finish !" ,path2.c_str() , save_file_path_.c_str());
    }
    return true;
}

std::string CLogUdp::GetProcessName(){
    return name_;
}

CLogUdp CLOG::log;


}}}
