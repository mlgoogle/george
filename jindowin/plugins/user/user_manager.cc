//  Copyright (c) 2015-2015 The mem_manager.cc Authors. All rights reserved.
//  Created on: 2016骞�1鏈�9鏃� Author: paco

#include "user/user_manager.h"
#include "base/logic/logic_comm.h"

UserManager* UserManager::instance_;
pthread_mutex_t UserManager::lock_ = PTHREAD_MUTEX_INITIALIZER;

UserManager::UserManager() {
  max_regist_cache_ = 10000;
}

UserManager::~UserManager() {
}

UserManager* UserManager::GetInstance() {
  if (NULL == instance_) {
    pthread_mutex_lock(&lock_);
    if (NULL == instance_)
      instance_ = new UserManager();
    pthread_mutex_unlock(&lock_);
  }
  return instance_;
}

int UserManager::AddRegist(UserInfo* user) {
  assert(NULL != user);
  pthread_mutex_lock(&lock_);
  RegisterUserMap::iterator it = regist_user_map_.find(user->email);
  if (it != regist_user_map_.end()) {
      pthread_mutex_unlock(&lock_);
    return -1;
  }
  if (regist_user_map_.size() > max_regist_cache_) {
      RegisterUserMap::iterator it_begin = regist_user_map_.begin();
      for(int i = 0; i < max_regist_cache_/2 && it_begin != regist_user_map_.end(); i++) {
	  UserInfo* u = (UserInfo*) it_begin->second;
	  delete u;
	  regist_user_map_.erase(it_begin ++);
      }
  }
  regist_user_map_[user->email] = user;
  pthread_mutex_unlock(&lock_);
  return 0;
}

int UserManager::DelRegist(std::string email) {
  pthread_mutex_lock(&lock_);
  RegisterUserMap::iterator it = regist_user_map_.find(email);
    if (it != regist_user_map_.end()) {
	 UserInfo* u = (UserInfo*) it->second;
	 delete u;
	 regist_user_map_.erase(it);
	 pthread_mutex_unlock(&lock_);
	 return 0;
    }
    pthread_mutex_unlock(&lock_);
    return -1;

}

UserInfo* UserManager::FindRegist(std::string email) {
  pthread_mutex_lock(&lock_);
   RegisterUserMap::iterator it = regist_user_map_.find(email);
   if (it != regist_user_map_.end()) {
     pthread_mutex_unlock(&lock_);
     return it->second;
   }
   else {
       pthread_mutex_unlock(&lock_);
     return NULL;
   }
}
