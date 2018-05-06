#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
template <typename T>
class Queue{
private:
  mutable std::mutex _m;
  std::queue<T> _q;
  std::condition_variable _cv;
  bool _running;
  bool _blocked;
public:
  Queue():_running(true),_blocked(false){}
  Queue(Queue const& other)=delete;
  Queue& operator=(Queue const& other)=delete;
  void stop(){
    std::unique_lock<std::mutex> l(_m);
    _running=false;
  }
  void block(){
    std::unique_lock<std::mutex> l(_m);
    _blocked=true;
  }
  void unblock(){
    std::unique_lock<std::mutex> l(_m);
    _blocked=false;
  }

  void push(const T& item){
    std::lock_guard<std::mutex> l(_m);
    _q.push(item);
    _cv.notify_one();
  }
  void push(T&& item){
    std::lock_guard<std::mutex> l(_m);
    _q.push(std::move(item));
    _cv.notify_one();
  }
  void wait_and_pop(T& value){
    std::unique_lock<std::mutex> l(_m);
    _cv.wait(l,[this]{return !_q.empty()||!_running||!_blocked;});
    value=_q.front();
    _q.pop();
  }
  std::shared_ptr<T> wait_and_pop(){
    std::unique_lock<std::mutex> l(_m);
    _cv.wait(l,[this]{return !_q.empty()||!_running||!_blocked;});
    std::shared_ptr<T> ret=std::make_shared<T>(_q.front());
    _q.pop();
    return ret;
  }
  T wait_and_pop_raw(){
    std::unique_lock<std::mutex> l(_m);
    _cv.wait(l,[this]{return !_q.empty()||!_running||!_blocked;});
    auto ret=std::move(_q.front());
    _q.pop();
    return std::move(ret);
  }
  bool empty() const{
    std::lock_guard<std::mutex> l(_m);
    return _q.empty();
  }
  void clear(){
    std::lock_guard<std::mutex> l(_m);
    _q.clear();
  }
  std::size_t size(){
    std::lock_guard<std::mutex> l(_m);
    return _q.size();
  }

};