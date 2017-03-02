// 用于派生的单例模板类

#pragma once

#include <iostream>
#include <memory>
#include <pthread.h>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/scoped_ptr.hpp>
#include <boost/thread.hpp>

template <typename T>
class Singleton {
 public:
  static T& GetSingleton() {
    if (!ms_Singleton) {
      lock_.lock();
      if (!ms_Singleton) {
        new T();
      }
      lock_.unlock();
    }
    return (*ms_Singleton);
  }
  static T* GetSingletonPtr() {
    if (!ms_Singleton) {
      lock_.lock();
      if (!ms_Singleton) {
        new T();
      }
      lock_.unlock();
    }
    return ms_Singleton;
  }
 protected:
  static T* ms_Singleton;
  Singleton() {
    // CHECK(!ms_Singleton);
#if defined( _MSC_VER ) && _MSC_VER < 1200
    int offset = (int)(T*)1 - (int)(Singleton<T>*)(T*)1;
    ms_Singleton = (T*)((int)this + offset);
#else
    ms_Singleton = static_cast<T*>(this);
#endif
  };
  ~Singleton() {
    // CHECK(ms_Singleton);
    ms_Singleton = NULL;
  }
  friend class std::auto_ptr<Singleton<T> >;
  static boost::scoped_ptr<Singleton<T> > instance_;
 private:
  static boost::mutex lock_;
  Singleton(const Singleton<T>&);
  Singleton& operator=(const Singleton<T>&);
};

template <typename T>T* Singleton<T>::ms_Singleton = NULL;
template <typename T> boost::scoped_ptr<Singleton<T> > Singleton<T>::instance_;
template <typename T> boost::mutex Singleton<T>::lock_;
