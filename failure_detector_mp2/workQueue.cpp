#include "workQueue.h"


// wqueue::wqueue() {
//     pthread_mutex_init(&m_mutex, NULL);
//     pthread_cond_init(&m_condv, NULL);
// }
// wqueue::~wqueue() {
//     pthread_mutex_destroy(&m_mutex);
//     pthread_cond_destroy(&m_condv);
// }
// wqueue::void add(T item) {
//     pthread_mutex_lock(&m_mutex);
//     m_queue.push_back(item);
//     pthread_cond_signal(&m_condv);
//     pthread_mutex_unlock(&m_mutex);
// }
// wqueue::T remove() {
//     pthread_mutex_lock(&m_mutex);
//     while (m_queue.size() == 0 && !end_flag.is_true()) {
//         pthread_cond_wait(&m_condv, &m_mutex);
//     }
//     T item;
//     if(!end_flag.is_true() && m_queue.size() == 0){
//         item= new WorkItem("wqueue stop waiting...quiting...",0);
//     }
//     else
//     {
//         item = m_queue.front();
//         m_queue.pop_front();
//     }
//     pthread_mutex_unlock(&m_mutex);
//     return item;
// }
// void wqueue::end() {
//     pthread_mutex_lock(&m_mutex);
//     pthread_cond_signal(&m_condv);
//     pthread_mutex_unlock(&m_mutex);
// }
// int wqueue::size() {
//     pthread_mutex_lock(&m_mutex);
//     int size = m_queue.size();
//     pthread_mutex_unlock(&m_mutex);
//     return size;
// }
// void wqueue::clear(){
//     pthread_mutex_lock(&m_mutex);
//     while(m_queue.size()!=0){
//         T item=m_queue.front();
//         delete item;
//         m_queue.pop_front();
//     }
//     pthread_mutex_unlock(&m_mutex);
// }