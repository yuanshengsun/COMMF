#include"factory_component.h"

namespace YSSLM {
	namespace engine {
		namespace factory {
std::unordered_map<std::string, std::function<std::shared_ptr<NodeComponent>()>> factoryComponent::map_component;
		}
	}
}
