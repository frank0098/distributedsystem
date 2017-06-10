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
    pthread_mutex_init(&m, NULL);
    pthread_cond_init(&cv, NULL);
}
flag_t::~flag_t(){
    pthread_mutex_destroy(&m);
    pthread_cond_destroy(&cv);
}
void flag_t::lock(){
    pthread_mutex_lock(&m); 
}
void flag_t::unlock(){
    pthread_mutex_unlock(&m);
}
void flag_t::set_true(){
    _flag=true;
}
void flag_t::set_false(){
    _flag=false;
}
bool flag_t::is_true(){
    return _flag;
}
void flag_t::cond_signal(){
    pthread_cond_signal(&cv); 
}
void flag_t::cond_wait(){
    pthread_cond_wait(&cv,&m); 
}


flag_t stop_flag;

flag_t pause_flag;

flag_t detector_sender_stop_flag;

flag_t detector_stop_flag;