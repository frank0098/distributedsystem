#ifndef WORKQUEUE_H
#define WORKQUEUE_H

#include <pthread.h>
#include <list>
#include <string>
#include "thread.h"

using namespace std;
 
struct WorkItem
{
    string m_message;
    int    m_number;
    WorkItem(const char* message, int number) 
          : m_message(message), m_number(number) {}
    ~WorkItem() {}
 
    const char* getMessage() { return m_message.c_str(); }
    int getNumber() { return m_number; }
};

template <typename T> class wqueue
{ 
    list<T>   m_queue;
    pthread_mutex_t m_mutex;
    pthread_cond_t  m_condv;

public:
    wqueue() {
        pthread_mutex_init(&m_mutex, NULL);
        pthread_cond_init(&m_condv, NULL);
    }
    ~wqueue() {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_condv);
    }
    void add(T item) {
        pthread_mutex_lock(&m_mutex);
        m_queue.push_back(item);
        pthread_cond_signal(&m_condv);
        pthread_mutex_unlock(&m_mutex);
    }
    T remove() {
        pthread_mutex_lock(&m_mutex);
        while (m_queue.size() == 0 && !stop_flag.is_true()) {
            pthread_cond_wait(&m_condv, &m_mutex);
        }
        T item;
        if(stop_flag.is_true() && m_queue.size() == 0){
            item= new WorkItem("wqueue stop waiting...quiting...",0);
        }
        else
        {
            item = m_queue.front();
            m_queue.pop_front();
        }
        pthread_mutex_unlock(&m_mutex);
        return item;
    }
    void end() {
        pthread_mutex_lock(&m_mutex);
        pthread_cond_signal(&m_condv);
        pthread_mutex_unlock(&m_mutex);
    }
    int size() {
        pthread_mutex_lock(&m_mutex);
        int size = m_queue.size();
        pthread_mutex_unlock(&m_mutex);
        return size;
    }
};

#endif