#include "wait_base.h"
#include "../log.h"

namespace YSSLM {
namespace engine {
namespace wait {

WaitBase CWait::wait_base_;
std::atomic<bool> CWait::stop_ = {false};

namespace{
static void Term(int sig){
    ULOG_WARNIG("Term() CWait::Stop() !");
    CWait::Stop();
}
}

void WaitBase::Wait(){
    stop_= false;
    // usleep(10000);
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock,[this]{
        return stop_.load();
    });
}
void WaitBase::Stop(){
    std::unique_lock<std::mutex> lock(mutex_);
    ULOG_WARNIG("WaitBase::Stop() stop_ = true cv_.notify_one() !");
    stop_ = true;
    cv_.notify_one();
}

void CWait::Init(){
    signal(SIGINT, [](int sig){
        if(SIGINT == sig){
            ULOG_WARNIG("CWait::Wait() wait_base_.Stop() !");
            Stop();
    }});

    struct sigaction ac;
    memset(&ac , 0 , sizeof(ac));

    ac.sa_handler = &Term;

    sigaction(SIGTERM, &ac , NULL);
}

void CWait::Wait(){

    wait_base_.Wait();
}

}
}
}