#ifndef _YuanshengSun_LM_Engine_Udp_Sendto_H_
#define _YuanshengSun_LM_Engine_Udp_Sendto_H_

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
#include <unordered_map>
#include <memory>
#include "../crc/crc.h"
#include"../log.h"

namespace YSSLM {
namespace engine {
namespace udp {

class Sendto
{

public:
    Sendto(){}
    int Init(){
        sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if(-1==sockfd_){
            ULOG_ERROR("Sendto::Init() Failed to create socket !");
            return -1;
        }

        addr_len_=sizeof(addr_);

        return 0;
    }

    void Close(){
        close(sockfd_);
    }

    void Send(std::shared_ptr<std::string> &data ,int port_out_c, uint32_t addr_c = 0){
        *data += crc::CRC::CRC8(*data, data->size());
        
        memset(&addr_, 0, sizeof(addr_));
        addr_.sin_family = AF_INET;
        addr_.sin_port   = htons(port_out_c);
        addr_.sin_addr.s_addr = (addr_c == 0 )? htonl(INADDR_ANY) : addr_c;
        sendto(sockfd_, data->c_str(), data->size(), 0, (sockaddr*)&addr_, addr_len_);
    }

private:

    int sockfd_;

    struct sockaddr_in addr_;
    socklen_t addr_len_;

};

}}}
#endif