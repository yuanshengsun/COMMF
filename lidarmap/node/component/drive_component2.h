#pragma once
#include"../../engine/engine_include.h"
#include"../../engine/thread/ThreadPool.h"

#include"../message/angle_message.h"

namespace YSSLM {
	namespace node {
		namespace component {
			namespace drive {
				using namespace YSSLM::engine::include;
				using threadpool = ::YSSLM::engine::threadpool::threadpool;
				using AngleMessage = ::YSSLM::node::message::angle::AngleMessage;

				class DriveComponent2 :public NodeComponent {
					
				public:
					bool Init();

					bool Close();

				private:

	
					std::shared_ptr<Read> read_ = nullptr;
					std::shared_ptr<std::thread> aaaaaa = nullptr;
				};

REGISTER_COMPONENT(DriveComponent2);
			}
		}
	}
}


