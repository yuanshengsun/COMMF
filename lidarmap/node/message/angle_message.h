#pragma once
#include"../../engine/engine_include.h"

namespace YSSLM {
	namespace node {
		namespace message {
			namespace angle {
				

				struct AngleMessage {

				public:

					std::string name;
		

					void operator=(const AngleMessage& data);
					void Set(const std::shared_ptr<const std::string>& data);
					std::shared_ptr<std::string> Get();

				private:


				};
			}
		}
	}
}