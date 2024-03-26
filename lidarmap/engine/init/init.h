#ifndef _YuanshengSun_LM_Engine_Init_Init_H_
#define _YuanshengSun_LM_Engine_Init_Init_H_

#include <sys/types.h>
#include <unistd.h>
#include"../factory/factory_component.h"


namespace YSSLM {
	namespace engine {
		namespace init {

using NodeComponent = YSSLM::engine::base::NodeComponent;
using factoryComponent = YSSLM::engine::factory::factoryComponent;
using CLOG = YSSLM::engine::logudp::CLOG;

class InitBase {

public:
	void init(std::string firing_path) {
		firing_path_ = firing_path;
		id_frok_ = false;
	}

	bool Init_() ;

	bool Close_() ;

private:

	std::unordered_map<std::string, std::shared_ptr<NodeComponent>> Compenent_;
	std::string firing_path_;

	bool id_frok_ = false;

			};

class Init{

public:
	static bool init(int argc, char **argv);

	static bool Close_(){
		return init_base_.Close_();
	}
private:
	static InitBase init_base_;
};
		}
	}
}

#define _YSSLM_INIT_STATR_(T , Y)   YSSLM::engine::init::Init::init(T , Y)
#define _YSSLM_INIT_CLOSE_   YSSLM::engine::init::Init::Close_()

#endif
