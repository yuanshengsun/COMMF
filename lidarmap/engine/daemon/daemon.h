#ifndef _YuanshengSun_LM_Engine_Daemon_Daemon_H_
#define _YuanshengSun_LM_Engine_Daemon_Daemon_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <csignal>
#include <string>

namespace YSSLM {
namespace engine {
namespace daemon_t {

class Daemon {

public:
    static bool Initializa(int argc, char **argv);

    static char GetDeamon(){
        return d_;
    }

    static std::string GetExeName();
private:
    static bool CreateDeamon();

    static char d_;

    static std::string name_;
};

}
}
}

#endif