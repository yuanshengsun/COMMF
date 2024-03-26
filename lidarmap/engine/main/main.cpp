#include"../init/init.h"
#include "../log.h"
#include "../wait/wait.h"
#include "../log_udp/log.h"
#include "../daemon/dmn.h"
#include "../thread/thread_attr.h"

#include <sys/types.h>
#include <signal.h>

int main(int argc, char **argv) {
	bool returns = true;

	if (!returns || !_YSSLM_DAEMON_STATR_(argc, argv)) {
		printf(" \033[33m==>\033[0m \033[31m[main] deamon fell !\033[0m\n");
		returns = false;
	}

	if (returns){
		if (!_YSSLM_THREADATTR_INIT_main_) {
			printf(" \033[33m==>\033[0m \033[31m[main] thread attr fell !\033[0m\n");
			returns = false;
		}
	}

	if (returns){
		if (!_YSSLM_ULOG_STATR_(argc, argv)) {
			printf(" \033[33m==>\033[0m \033[31m[main] ulog fell !\033[0m\n");
			returns = false;
		}
	}

	ULOG_INFO("");
	ULOG_INFO("--------------------------------------------------");
	ULOG_INFO("");

	_YSSLM_WAIT_INIT_;
	
	if (returns){
		ULOG_INFO("Main::Init Init_() start !");
		if (!_YSSLM_INIT_STATR_(argc, argv)) {
			ULOG_ERROR("Main::Init Init_() fell !");
			returns = false;
		}
		ULOG_INFO("Main::Init Init_() succed !");
	}

	if (returns){
		_YSSLM_WAIT_STATR_;
	}
	
	ULOG_INFO("Main::Init Close_() start !");
	if (!_YSSLM_INIT_CLOSE_){
		ULOG_ERROR("Main::Init close() fell !");
		returns = false;
	}
	ULOG_INFO("Main::Init close() succed !");

	ULOG_INFO("");
	ULOG_INFO("--------------------------------------------------");
	ULOG_INFO("");
	
	_YSSLM_ULOG_CLOSE_;

	kill(getpid(), SIGKILL);

	return 0;
}

