// 用于派生的单例模板类

#pragma once

#include <iostream>
#include <memory>
#include <pthread.h>
#include <memory>
#include <mutex>

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
  static std::unique_ptr<Singleton<T> > instance_;
 private:
  static std::mutex lock_;
  Singleton(const Singleton<T>&);
  Singleton& operator=(const Singleton<T>&);
};

template <typename T>T* Singleton<T>::ms_Singleton = NULL;
template <typename T> std::unique_ptr<Singleton<T> > Singleton<T>::instance_;
template <typename T> std::mutex Singleton<T>::lock_;
