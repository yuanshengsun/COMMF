#ifndef _YuanshengSun_LM_Engine_Log_H_
#define _YuanshengSun_LM_Engine_Log_H_

#include "log_udp/log_udp.h"


#define _YSSLM_ULOG_GetStartTimes_string_           YSSLM::engine::logudp::CLOG::GetStartTimes()
#define _YSSLM_ULOG_GetRelativeTime_                YSSLM::engine::logudp::CLOG::GetRelativeTime()
#define _YSSLM_ULOG_GetProcessName_                 YSSLM::engine::logudp::CLOG::GetProcessName()
#define _YSSLM_ULOG_SetTimingRelativeTime_(T)       YSSLM::engine::logudp::CLOG::SetPTPRelativeTime(T)


#define ULOG_DEBUG(T,...)                           YSSLM::engine::logudp::CLOG::Send(0 ,T, ##__VA_ARGS__)
#define ULOG_INFO(T,...)                            YSSLM::engine::logudp::CLOG::Send(1 ,T, ##__VA_ARGS__)
#define ULOG_WARNIG(T,...)                          YSSLM::engine::logudp::CLOG::Senf(2 ,__FILE__,__LINE__,T, ##__VA_ARGS__)
#define ULOG_ERROR(T,...)                           YSSLM::engine::logudp::CLOG::Senf(3 ,__FILE__,__LINE__,T, ##__VA_ARGS__)


#endif