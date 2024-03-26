#ifndef _YuanshengSun_LM_Engine_MessageCose_Read_H_
#define _YuanshengSun_LM_Engine_MessageCose_Read_H_
#include <functional> 
#include <memory>
namespace YSSLM {
	namespace engine {
		namespace msg_c {

			class Read {
			public:
				virtual void null() = 0;
				bool ptr_read = false;
				int policy;
				int sched_priority;
				int cpu_id;
				int num_thr;
			};

			template<class T>
			class Reades: public Read {
			public:
				void null() { return; }
				std::function<void(std::shared_ptr<T>&)> call_back;
			};

			
			class CRead {
			public:
				template<class T>
				CRead(std::shared_ptr<std::vector<std::shared_ptr<Reades<T>>>> r){
					read = std::make_shared<std::vector<std::shared_ptr<Read>>>();
					for(auto &data : *r){
						read->push_back(data);
					}
				}
			private:
				std::shared_ptr<std::vector<std::shared_ptr<Read>>> read;
			};

		}
	}
}
#endif