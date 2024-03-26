#ifndef _YuanshengSun_LM_Engine_LogUdp_LogUdp_H_
#define _YuanshengSun_LM_Engine_LogUdp_LogUdp_H_

#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>

#include <atomic>
#include <mutex>
#include <sys/time.h>
#include <memory>
#include "../thread/ThreadPool.h"
#include "../yaml-cpp_063/include/yaml-cpp/yaml.h"
#include "../crc/crc.h"

namespace YSSLM {
namespace engine {
namespace logudp{

namespace{
    const std::string LEVELS[4] = {"Debug" , "\033[1;32mInfo\033[0m" , "\033[1;33mWarning\033[0m" , "\033[1;31mError\033[0m"};
    const std::string LEVELS_clc_b[4] = {"" , "\033[1;32m" , "\033[1;33m" , "\033[1;31m"};
    const std::string LEVELS_clc_e[4] = {"" , "\033[0m" , "\033[0m" , "\033[0m"};
    struct log_udp_time
    {
        int tm_year = 0;
        int tm_mon = 0;
        int tm_mday = 0;
        int tm_hour = 0;
        int tm_min = 0;
        int tm_sec = 0;
        void set(const char * data){
            memcpy(((char*)this), data , sizeof(log_udp_time));
        }
        std::string get(){
            std::string data;
            data.resize(sizeof(log_udp_time));
            memcpy( (char*)data.c_str() , ((char*)this), sizeof(log_udp_time));
            return data;
        }
    };
    
}

class CLogUdp {

    std::mutex mutex_;

public:
    bool Initializa(const char *startTimes , const YAML::Node& node2);

    void DeInitializa();

    template <class... Args>
    void Senf(int level, const char* filenamef,int fileline,std::string &data, Args&&... args){
        if(level < level_udp_ && level < level_ctrl_ && level < level_file_){
            return;
        }
        if(c_thread_pool_ != nullptr){
            std::string data_buff;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                snprintf(buff_ ,sizeof(buff_), data.c_str() , args...);
                data_buff = buff_;
            }

            std::shared_ptr<std::string> data2 = std::make_shared<std::string>("ULOG: ");
            std::string filenames = filenamef;
            int i;
            for(i = filenames.size() - 1 ; i>=0 ;--i){
                if(filenames[i] == '/' || filenames[i] == '\\'){
                    break;
                }
            }
            std::string filename2(filenames.begin()+ i + 1, filenames.end());
            std::string time_l = std::to_string(GetRelativeTime());
            time_l.insert( time_l.begin() , 15 - time_l.size(), ' ');
            *data2 += "\033[1;35m" + time_l + "\033[0m" + 
                        " [\033[1;34m" + name_ + "\033[0m][" + LEVELS[level] + "] ";
            *data2 = *data2 + LEVELS_clc_b[level] + filename2 + ":" + std::to_string(fileline) + " ";
            *data2 = *data2 + data_buff + LEVELS_clc_e[level] + "\n";
            
            if(level >= level_ctrl_){
                printf("%s",data2->c_str());
                fflush(stdout);
            }

            c_thread_pool_->commit([this,level](std::shared_ptr<std::string>data2){
                Send2(level , *data2);
            },data2 );
        }
    }
    void Senf(int level, const char* filenamef,int fileline, std::string &data){
        if(level < level_udp_ && level < level_ctrl_ && level < level_file_){
            return;
        }
        if(c_thread_pool_ != nullptr){
   
            if(data.size()>sizeof(buff_)-1){
                data.resize(sizeof(buff_)-1);
            }
            std::string data2 = "ULOG: ";

            std::string filenames = filenamef;
            int i;
            for(i = filenames.size() - 1 ; i>=0 ;--i){
                if(filenames[i] == '/' || filenames[i] == '\\'){
                    break;
                }
            }
            std::string filename2(filenames.begin()+ i + 1, filenames.end());
            std::string time_l = std::to_string(GetRelativeTime());
            time_l.insert( time_l.begin() , 15 - time_l.size(), ' ');
            data2 += "\033[1;35m" + time_l + "\033[0m" + 
                        " [\033[1;34m" + name_ + "\033[0m][" + LEVELS[level] + "] ";
            data2 = data2 + LEVELS_clc_b[level] + filename2 + ":" + std::to_string(fileline) + " ";
            data2 = data2 + data + LEVELS_clc_e[level] + "\n";

            if(level >= level_ctrl_){
                printf("%s",data2.c_str());
                fflush(stdout);
            }

            c_thread_pool_->commit([this,level](std::string &data2){
                Send2(level , data2);
            } , data2);
        }
    }
    template <class... Args>
    void Send(int level, std::string &data, Args&&... args){
        if(level < level_udp_ && level < level_ctrl_ && level < level_file_){
            return;
        }
        if(c_thread_pool_ != nullptr){
            std::string data_buff;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                snprintf(buff_ ,sizeof(buff_), data.c_str() , args...);
                data_buff = buff_;
            }

            std::shared_ptr<std::string> data2 = std::make_shared<std::string>("ULOG: ");
            std::string time_l = std::to_string(GetRelativeTime());
            time_l.insert( time_l.begin() , 15 - time_l.size(), ' ');
            *data2 += "\033[1;35m" + time_l + "\033[0m" + 
                        " [\033[1;34m" + name_ + "\033[0m][" + LEVELS[level] + "] " +
                        LEVELS_clc_b[level] + data_buff + LEVELS_clc_e[level] + "\n";

            if(level >= level_ctrl_){
                printf("%s",data2->c_str());
                fflush(stdout);
            }

            c_thread_pool_->commit([this,level](std::shared_ptr<std::string>data2){
                Send2(level , *data2);
            },data2 );
        }
    }
    void Send(int level, std::string &data){
        if(level < level_udp_ && level < level_ctrl_ && level < level_file_){
            return;
        }
        if(c_thread_pool_ != nullptr){
            
            if(data.size()>sizeof(buff_)-1){
                data.resize(sizeof(buff_)-1);
            }
            std::string data2 = "ULOG: ";
            std::string time_l = std::to_string(GetRelativeTime());
            time_l.insert( time_l.begin() , 15 - time_l.size(), ' ');
            data2 += "\033[1;35m" + time_l + "\033[0m" + 
                        " [\033[1;34m" + name_ + "\033[0m][" + LEVELS[level] + "] " +
                        LEVELS_clc_b[level] + data + LEVELS_clc_e[level] + "\n";

            if(level >= level_ctrl_){
                printf("%s",data2.c_str());
                fflush(stdout);
            }

            c_thread_pool_->commit([this,level](std::string &data2){
                Send2(level , data2);
            },data2);
        }
    }

    std::string GetStartTimes();
private:
    bool Initializa(const char *startTimes);

    void Send2(int level , std::string& data2){
        if(port_c_ >= 0 && level >= level_udp_){
            short num_data = data2.size();
            data2.insert(data2.begin(),((char*)&num_data)[1]);
            data2.insert(data2.begin(),((char*)&num_data)[0]);
            // data2 += crc::CRC::CRC8(data2, data2.size());
            sendto(sockfd_ , data2.c_str() , data2.size() , 0 , (sockaddr*)&addr_ , addr_len_);
        }
        if(save_file_path_b_ && level >= level_file_){
            // data2.pop_back();
            data2.erase(data2.begin() , data2.begin()+2);
            save_file_<<data2<<std::flush;
        }
    }

private:   

    int sockfd_;
    struct sockaddr_in addr_;
    socklen_t addr_len_;
    std::string name_;
//----
    uint32_t addr_c_;
    int port_c_;


//----
    double startTime_ , endTime_;
    // struct timeval  endTime_;
//----
    std::string save_file_path_ , save_file_path_dir_;
    bool save_file_path_b_;
    std::ofstream save_file_;
//----
    int level_udp_;
    int level_file_;
    int level_ctrl_;
//-----
    std::string GetCurrentTime_;
    std::string startTimes_binry_;

private://
    void StartGetTime(const char *startTimes){
        if(startTimes[0] == 'n' && startTimes[1] == 'u' && startTimes[2] == 'l' && startTimes[3] == 'l'){
            // gettimeofday(&endTime_ , NULL);
            startTime_ = std::chrono::system_clock::now().time_since_epoch().count() * 1e-9;
        }
        else{
            startTime_ = *(double*)startTimes;   
        }
    }
public:
    double GetRelativeTime(){
        std::lock_guard<std::mutex> lock (mutex_GetRelativeTime_);
        // gettimeofday(&endTime_ , NULL);
        return std::chrono::system_clock::now().time_since_epoch().count() * 1e-9 - startTime_;
    }

    bool SetPTPRelativeTime(double time);

    std::string GetProcessName();

    std::mutex mutex_GetRelativeTime_;
//-----
private:
    std::shared_ptr<threadpool::threadpool> c_thread_pool_= nullptr;

    char buff_[5001] = {0};
    // bool stop_thr_;
};

class CLOG {
public:
    static bool Initializa(int argc, char **argv){
		if(argc < 3){
			return false;
		}
		std::string startTimes = argv[2];
        // if(startTimes != "null" ){
        //     if(startTimes.size() != 8){
        //         return false;
        //     }
        // }
        if(startTimes != "null" ){
            if(startTimes.size() != 32){
                return false;
            }
            for(int i = 0; i < 32 ;  ++i){
                if(startTimes[i] == '*'){
                    startTimes[i] = '\0';
                }
            } 
        }
        auto node = YAML::LoadFile(argv[1]);
        if (!node["LogUdp"].IsDefined()) {
			return false;
		}
		auto nodelog = node["LogUdp"];
		return Initializa(startTimes.c_str() , nodelog);
    }

    static bool Initializa(const char *startTimes , const YAML::Node& node){
        return log.Initializa(startTimes  , node);
    }
    static void DeInitializa(){
        log.DeInitializa();
    }
    template <class... Args>
    static void Senf(int level, const char* filenamef,int fileline,std::string data, Args&&... args){
        log.Senf(level , filenamef,fileline,data , args...);
    }

    static void Senf(int level, const char* filenamef,int fileline,std::string data){
        log.Senf(level , filenamef,fileline,data);
    }

    template <class... Args>
    static void Send(int level, std::string data, Args&&... args){
        log.Send(level , data ,args...);
    }

    static void Send(int level, std::string data){
        log.Send(level , data);
    }
    static std::string GetStartTimes(){
        return log.GetStartTimes();
    }
    static double GetRelativeTime(){
        return log.GetRelativeTime();
    }
    static bool SetPTPRelativeTime(double time){
        return log.SetPTPRelativeTime(time);
    }
    static std::string GetProcessName(){
        return log.GetProcessName();
    }

private:
    static CLogUdp log;
};

}}}

#endif