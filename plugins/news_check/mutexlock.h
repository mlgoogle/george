// Copyright (c) 2016 The Jindowin Authors. All rights reserved.
// mutexlock.h
// Created on: 2016年5月12日.
// Author: Paco.
#ifndef PLUGINS_NEWS_CHECK_MUTEXLOCK_H_
#define PLUGINS_NEWS_CHECK_MUTEXLOCK_H_

#include <pthread.h>
// Helper class that locks a mutex on construction and unlocks the mutex when
// the destructor of the MutexLock object is invoked.
//
// Typical usage:
//
//   void MyClass::MyMethod() {
//     MutexLock l(&mu_);       // mu_ is an instance variable
//     ... some complex code, possibly with multiple return paths ...
//   }

class Mutex {
 public:
  Mutex() { pthread_mutex_init(&mu_, NULL); }
  ~Mutex() { pthread_mutex_destroy(&mu_); }

  void Lock() { pthread_mutex_lock(&mu_); }
  void Unlock() { pthread_mutex_unlock(&mu_); }

 private:
  pthread_mutex_t mu_;
};

class MutexLock {
 public:
  explicit MutexLock(Mutex* mu) : mu_(mu) {
    mu_->Lock();
  }

  ~MutexLock() { mu_->Unlock(); }

 private:
  Mutex* const mu_;
};

#endif  // PLUGINS_NEWS_CHECK_MUTEXLOCK_H_
