#include"drive_component.h"
#include"../../engine/log.h"

#include <sys/mman.h>

namespace YSSLM {
namespace node {
namespace component {
namespace drive {
	int iii=0;
bool DriveComponent::Init() {
	threadpool_ = std::make_shared<threadpool>(2);

	read_= NodeRead< AngleMessage >("AngleMessage", [this](std::shared_ptr< AngleMessage >& msg) {
			ULOG_DEBUG("-----------------[DriveComponent 0 1 ]----- thr1 ---%d    %s", iii ++ , msg->name.c_str());
	});

	write_ = NodeWrite<AngleMessage>("AngleMessage");

	if (read_ != nullptr && write_ != nullptr){
		start();
		return true;
	}

	
	return false;
}

bool DriveComponent::start() {
	

	threadpool_->commit([this] {
		
		while (1)
		{	
			std::unique_lock<std::mutex> lock(mut_);

			cv_.wait_for(lock,std::chrono::milliseconds(500) , [this]{
					return (!que_.empty());
			});
			
			if(que_.empty()){
				ULOG_WARNIG("-----------------[DriveComponent 0 1 ]----- -");
				continue;
			}

			write_->Send(que_.front());
			que_.pop();
		}
	});
	threadpool_->commit([this] {
		while (1)
		{	
			for(int i = 0;i<1;++i)
			{
				std::shared_ptr<AngleMessage> aaaa = std::make_shared<AngleMessage>();
				aaaa->name = "aaa!!";
				{
					std::lock_guard<std::mutex> lock(mut_);
					que_.emplace(aaaa);
				}
			}
			cv_.notify_one();
			usleep(1e6);
		
		}
	});
	
	return true;
}
bool DriveComponent::stop() {

	return true;
}
bool DriveComponent::Close() {

	stop();

	if (!threadpool_) {
		threadpool_ = nullptr;
	}

	return true;
}



}
}
}
}



