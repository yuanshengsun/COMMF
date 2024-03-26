#ifndef _YuanshengSun_LM_Engine_Daemon_Dmn_H_
#define _YuanshengSun_LM_Engine_Daemon_Dmn_H_
#include "daemon.h"

#define _YSSLM_DAEMON_STATR_(T , Y)   YSSLM::engine::daemon_t::Daemon::Initializa(T , Y)
#define _YSSLM_DAEMON_GET_   YSSLM::engine::daemon_t::Daemon::GetDeamon()

#define _YSSLM_DAEMON_GETNAME_   YSSLM::engine::daemon_t::Daemon::GetExeName()

#endif