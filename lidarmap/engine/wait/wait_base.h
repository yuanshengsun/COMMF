#ifndef _YuanshengSun_LM_Engine_Wait_WaitBase_H_
#define _YuanshengSun_LM_Engine_Wait_WaitBase_H_

#include <memory>
#include <iostream>
#include <thread>
#include <mutex>
#include <sys/time.h>
#include <unistd.h>
#include <condition_variable>
#include <csignal>
#include <cstring>
#include <signal.h>
#include <atomic>

namespace YSSLM {
namespace engine {
namespace wait {

class WaitBase {

public:
    void Wait();
    void Stop();

private:
    std::condition_variable cv_;
    std::mutex mutex_;
    std::atomic<bool> stop_ = {false};
};

class CWait {

public:
    static void Init();

    static void Wait();

    static void Stop(){
        stop_ = true;
        wait_base_.Stop();
    }

    static bool GetIsWaitStop(){return stop_;}

private:
    static WaitBase wait_base_;

    static std::atomic<bool> stop_;
};

}
}
}

#endif