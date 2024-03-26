#ifndef _YuanshengSun_LM_Engine_EngineInclude_H_
#define _YuanshengSun_LM_Engine_EngineInclude_H_

#include"factory/factory_component.h"
#include"message_core/write.h"
#include"udp/recvfrom.h"
#include"udp/sendto.h"
#include"yaml-cpp_063/include/yaml-cpp/yaml.h"

namespace YSSLM {
	namespace engine {
		namespace include {
			using NodeComponent = YSSLM::engine::base::NodeComponent;
			using Write = YSSLM::engine::msg_c::CWrite;
			using Read = YSSLM::engine::msg_c::CRead;
		}
	}
}

#endif