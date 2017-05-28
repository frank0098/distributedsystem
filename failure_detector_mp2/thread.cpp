#include "thread.h"

Thread::Thread() : m_tid(0), m_running(0), m_detached(0) {}

Thread::~Thread()
{
    if (m_running == 1 && m_detached == 0) {
        pthread_detach(m_tid);
    }
    if (m_running == 1) {
        pthread_cancel(m_tid);
    }
}

static void* runThread(void* arg);
 
int Thread::start()
{
    int result = pthread_create(&m_tid, NULL, runThread, (void*)(this));
    if (result == 0) {
        m_running = 1;
    }
    return result;
}

static void* runThread(void* arg)
{
    return ((Thread*)arg)->run();
}

int Thread::join()
{
    int result = -1;
    if (m_running == 1) {
        result = pthread_join(m_tid, NULL);
        if (result == 0) {
            m_detached = 1;
        }
    }
    return result;
}

int Thread::detach()
{
    int result = -1;
    if (m_running == 1 && m_detached == 0) {
        result = pthread_detach(m_tid);
        if (result == 0) {
            m_detached = 1;
        }
    }
    return result;
}

pthread_t Thread::self() {
    return m_tid;
}


flag_t::flag_t():_flag(false){

}
void flag_t::set_true(){
    std::lock_guard<std::mutex> guard(m);
    _flag=true;
}
void flag_t::set_false(){
    std::lock_guard<std::mutex> guard(m);
    _flag=false;
}
bool flag_t::is_true(){
    std::lock_guard<std::mutex> guard(m);
    return _flag;
}


flag_t stop_flag;

flag_t pause_flag;