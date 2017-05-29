#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <mutex>
#include <condition_variable>

class flag_t{
public:
    flag_t();
    ~flag_t();
    void set_true();
    void set_false();
    bool is_true();
    void lock();
    void unlock();
    void cond_wait();
    void cond_signal();
private:
    pthread_mutex_t m; 
    pthread_cond_t cv;
    bool _flag;
};
extern flag_t stop_flag;
extern flag_t pause_flag;


class Thread
{
  public:
    Thread();
    virtual ~Thread();
 
    int start();
    int join();
    int detach();
    pthread_t self();
 
    virtual void* run() = 0;
 
  private:
    pthread_t  m_tid;
    int        m_running;
    int        m_detached;
};


#endif