#ifndef _YuanshengSun_LM_Engine_Wait_Wait_H_
#define _YuanshengSun_LM_Engine_Wait_Wait_H_

#include "wait_base.h"

#define _YSSLM_WAIT_INIT_   YSSLM::engine::wait::CWait::Init()
#define _YSSLM_WAIT_STATR_   YSSLM::engine::wait::CWait::Wait()
#define _YSSLM_WAIT_CLOSE_   YSSLM::engine::wait::CWait::Stop()

#define _YSSLM_WAIT_GetIsWaitStop_   YSSLM::engine::wait::CWait::GetIsWaitStop()

#endif