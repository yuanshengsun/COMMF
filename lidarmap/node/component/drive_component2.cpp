#include"drive_component2.h"
#include"../../engine/log.h"
namespace YSSLM {
namespace node {
namespace component {
namespace drive {
	
bool DriveComponent2::Init() {

	read_= NodeRead< AngleMessage >("AngleMessage", [this](std::shared_ptr< AngleMessage >& msg) {
			ULOG_INFO("-----------------[DriveComponent 0 2 ]----- thr1 --- %s", msg->name.c_str());
	});

	printf("---------%d\n",111);

	int iii = 0;
	printf("---------%d\n",iii);

	return true;
}
bool DriveComponent2::Close() {

	return true;
}



}
}
}
}



