#ifndef _YuanshengSun_LM_Engine_Crc_Crc_H_
#define _YuanshengSun_LM_Engine_Crc_Crc_H_

#include <string>

namespace YSSLM {
namespace engine {
namespace crc{

class CRC
{
    static const uint8_t CrcTable[256];
public:

    static char CRC8(std::string data , int len){
        unsigned char crc = 0;
        for(int i = 0 ; i < len ; i++){
            crc += (unsigned char)data[i];
        }
        return *(char*)&crc;
    }

    static char CRC8(const uint8_t* data, uint16_t data_len) {        
        uint8_t crc = 0;             
        while (data_len--) {                 
            crc = CrcTable[(crc ^ *data) & 0xff];                 
            data++;             
        }             
        return *(char*)&crc;      
    }
};



}}}

#endif