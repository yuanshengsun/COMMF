#include "daemon.h"

namespace YSSLM {
namespace engine {
namespace daemon_t {

char Daemon::d_;
std::string Daemon::name_;

bool Daemon::Initializa(int argc, char **argv){
    name_ = argv[0];

    if(argc < 4){
        return false;
    }
    std::string type = argv[3];
    if(type.size()!=1 || (type[0] != 'd' && type[0] != 't') ){
        return false;
    }
    d_ = type[0] ;
    if(type[0] == 'd'){
        return CreateDeamon();
    }
    return true;
}

bool Daemon::CreateDeamon(){
    if(daemon(1,0) == -1){
        printf("error:[Daemon] daemon \n");
        return false;
    }
    signal(SIGCHLD , SIG_IGN);
    return true;
}

std::string Daemon::GetExeName(){
    return name_;
}

}
}
}
