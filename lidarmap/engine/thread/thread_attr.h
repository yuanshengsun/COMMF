#ifndef _YuanshengSun_LM_Engine_MessageCose_ThreadAttr_H_
#define _YuanshengSun_LM_Engine_MessageCose_ThreadAttr_H_

#include <thread>
#include <iostream>

namespace YSSLM {
namespace engine {
namespace threadpool {

class ThreadAttr
{

public:
	static bool thread_attr(int policy , int sched_priority , int cpu_id , bool is_used = false){
		if(!is_used){
			return true;
		}
		if(policy != -1){
			
			if(policy < 0 || policy > 2){
				printf("error:[ThreadAttr] policy = %d \n", policy);
				return false;
			}
			if(policy == 0){
				sched_priority = 0;
			}

			pthread_attr_t attr;
			if(0 != pthread_attr_init(&attr)){
				printf("error:[ThreadAttr] pthread_attr_init \n");
				return false;
			}
			if(0 != pthread_attr_setinheritsched(&attr , PTHREAD_INHERIT_SCHED)){
				printf("error:[ThreadAttr] pthread_attr_setinheritsched \n");
				return false;
			}
			if(0 != pthread_attr_destroy(&attr)){
				printf("error:[ThreadAttr] pthread_attr_destroy \n");
				return false;
			}

			if(sched_priority < sched_get_priority_min (policy) || sched_priority > sched_get_priority_max (policy)){
				printf("error:[ThreadAttr] sched_priority = %d \n", sched_priority);
				return false;
			}

			struct sched_param    param;
			
			param.sched_priority = sched_priority;

			if(0 != pthread_setschedparam(pthread_self(), policy, &param)){
				printf("error:[ThreadAttr] pthread_setschedparam \n");
				return false;
			}
		}

		if(cpu_id != -1){
			int cpu_core_num = sysconf(_SC_NPROCESSORS_CONF);

			if(cpu_id < 0 || cpu_id > cpu_core_num - 1){
				printf("error:[ThreadAttr] cpu_id = %d \n", cpu_id);
				return false;
			}

			cpu_set_t mask;
			CPU_ZERO(&mask);
			CPU_SET(cpu_id, &mask);
			
			if(sched_setaffinity(pthread_self(), sizeof(cpu_set_t), &mask) == -1)
			{
				printf("error:[ThreadAttr] sched_setaffinity \n");
				return false;
			}
		}

		return true;
	};
};

}
}
}

#define _YSSLM_THREADATTR_INIT_USING_						true

#define _YSSLM_THREADATTR_INIT_main_   						YSSLM::engine::threadpool::ThreadAttr::thread_attr(SCHED_OTHER , 0 , -1 , _YSSLM_THREADATTR_INIT_USING_)

//SCHED_OTHER SCHED_FIFO SCHED_RR
#define _YSSLM_THREADATTR_INIT_policy_   					SCHED_RR
#define _YSSLM_THREADATTR_INIT_sched_priority_   			50
#define _YSSLM_THREADATTR_INIT_cpu_id_   					-1

#define _YSSLM_THREADATTR_INIT_policy_ulog_   				SCHED_OTHER
#define _YSSLM_THREADATTR_INIT_sched_priority_ulog_   		0
#define _YSSLM_THREADATTR_INIT_cpu_id_ulog_   				-1

#endif