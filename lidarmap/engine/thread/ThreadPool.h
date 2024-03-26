#ifndef _YuanshengSun_LM_Engine_MessageCose_ThreadPool_H_
#define _YuanshengSun_LM_Engine_MessageCose_ThreadPool_H_

#include <vector>
#include <queue>
#include <atomic>
#include <future>
#include <stdexcept>
#include <unistd.h>
#include "thread_attr.h"

namespace YSSLM {
	namespace engine {
		namespace threadpool {
//#define THREADPOOL_AUTO_GROW
namespace{
			using namespace std;
}

				class threadpool
				{
					unsigned short _initSize;
					using Task = function<void()>; 
					vector<thread> _pool;        
					queue<Task> _tasks;           
					mutex _lock;                   
// #ifdef THREADPOOL_AUTO_GROW
					mutex _lockGrow;               
// #endif 
					condition_variable _task_cv;   
					atomic<bool> _run{ true };     
					atomic<int>  _idlThrNum{ 0 }; 

					bool THREADPOOL_AUTO_GROW { false };

					unsigned short THREADPOOL_MAX_NUM { 2000 };

					int Policy ;
					int Sched_priority ;
					int Cpu_id;

				public:
					inline threadpool(unsigned short size = 4 , unsigned short max_sz = 0 , int policy = -1, int sched_priority = 0, int cpu_id = -1) { 
						_initSize = size; 
						addThread(size); 
						Policy = policy;
						Sched_priority = sched_priority;
						Cpu_id = cpu_id;
						if( max_sz > size ){
							THREADPOOL_MAX_NUM = max_sz;
							THREADPOOL_AUTO_GROW = true;
						}
					}
					inline ~threadpool()
					{
						while(1){
							{
								lock_guard<mutex> lock{ _lock };
								if(_tasks.empty()){
									break;
								}
							}
							usleep(10000);
						}
						_run = false;
						_task_cv.notify_all(); 
						for (thread& thread : _pool) {
							
							if (thread.joinable())
								thread.join();
						}
					}

				public:
					
					template<class F, class... Args>
					auto commit(F&& f, Args&&... args) -> future<decltype(f(args...))>
					{
						if (!_run)    
							throw runtime_error("commit on ThreadPool is stopped.");

						using RetType = decltype(f(args...)); 
						auto task = make_shared<packaged_task<RetType()>>(
							bind(forward<F>(f), forward<Args>(args)...)
							); 
						future<RetType> future = task->get_future();
						{    
							lock_guard<mutex> lock{ _lock };
							_tasks.emplace([task]() { 
								(*task)();
							});
						}
if(THREADPOOL_AUTO_GROW){
						if (_idlThrNum < 1 && _pool.size() < THREADPOOL_MAX_NUM)
							addThread(1);
}
						_task_cv.notify_one(); 

						return future;
					}

					
					int idlCount() { return _idlThrNum; }
					
					int thrCount() { return _pool.size(); }

				private:

				
					void addThread(unsigned short size)
					{
if(THREADPOOL_AUTO_GROW){
						if (!_run)    
							throw runtime_error("Grow on ThreadPool is stopped.");
						unique_lock<mutex> lockGrow{ _lockGrow }; 
}
						for (; _pool.size() < THREADPOOL_MAX_NUM && size > 0; --size)
						{   
							_pool.emplace_back([this] { 
								if (!ThreadAttr::thread_attr(Policy , Sched_priority , Cpu_id , _YSSLM_THREADATTR_INIT_USING_))    
									throw runtime_error("error: thread_attr().");
								while (true)
								{
									Task task; 
									{
										unique_lock<mutex> lock{ _lock };
										_task_cv.wait(lock, [this] { 
											return !_run || !_tasks.empty();
										});
										if (!_run && _tasks.empty())
											return;
										_idlThrNum--;
										task = move(_tasks.front()); 
										_tasks.pop();
									}
									task();
if(THREADPOOL_AUTO_GROW){
									if (_idlThrNum > 0 && _pool.size() > _initSize) 
										return;
}
									{
										unique_lock<mutex> lock{ _lock };
										_idlThrNum++;
									}
								}
							});
							{
								unique_lock<mutex> lock{ _lock };
								_idlThrNum++;
							}
						}
					}
				};

			}
		}
	}

#endif