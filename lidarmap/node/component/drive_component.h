#pragma once
#include"../../engine/engine_include.h"
#include"../../engine/thread/ThreadPool.h"

#include"../message/angle_message.h"
#include <condition_variable>
#include <queue>
namespace YSSLM {
	namespace node {
		namespace component {
			namespace drive {
				using namespace YSSLM::engine::include;
				using threadpool = ::YSSLM::engine::threadpool::threadpool;
				using AngleMessage = ::YSSLM::node::message::angle::AngleMessage;

				class DriveComponent :public NodeComponent {
					
				public:
					bool Init();

					bool Close();

				private:

					bool start();

					bool stop();

					std::shared_ptr<threadpool> threadpool_ = nullptr;
					std::shared_ptr<Write> write_ = nullptr;
					std::shared_ptr<Read> read_ = nullptr;
					std::shared_ptr<Read> read2_ = nullptr;

					std::mutex mut_;
					std::condition_variable cv_;

					std::ofstream fff;

					std::queue< std::shared_ptr<AngleMessage>> que_;
				};

REGISTER_COMPONENT(DriveComponent);
			}
		}
	}
}


