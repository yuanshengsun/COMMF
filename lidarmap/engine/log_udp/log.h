#ifndef _YuanshengSun_LM_Engine_LogUdp_Log_H_
#define _YuanshengSun_LM_Engine_LogUdp_Log_H_

#include "log_udp.h"

#define _YSSLM_ULOG_STATR_(T , Y)   YSSLM::engine::logudp::CLOG::Initializa(T , Y)
#define _YSSLM_ULOG_CLOSE_   YSSLM::engine::logudp::CLOG::DeInitializa()

#endif