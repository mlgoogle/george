//  Copyright (c) 2015-2015 The mem_manager.h Authors. All rights reserved.
//  Created on: 2016骞�1鏈�9鏃� Author: paco

#ifndef _HOME_TIANYIHENG_TEST_JINDOWIN_RUNNER_JINDOWIN_PLUGINS_NEWS_MEM_MANAGER_H_
#define _HOME_TIANYIHENG_TEST_JINDOWIN_RUNNER_JINDOWIN_PLUGINS_NEWS_MEM_MANAGER_H_

#include <pthread.h>
#include <string>
#include <map>

#include "net/comm_struct.h"
#include "net/typedef.h"

class UserManager {
 private:
  UserManager();
  ~UserManager();

 public:
  static UserManager* GetInstance();

  int AddRegist(UserInfo* user);
  int DelRegist(std::string email);

  UserInfo* FindRegist(std::string email);
 private:
  RegisterUserMap regist_user_map_;
  int max_regist_cache_;
 private:
  static pthread_mutex_t lock_;
  static UserManager* instance_;
};

#endif  // _HOME_TIANYIHENG_TEST_JINDOWIN_RUNNER_JINDOWIN_PLUGINS_NEWS_MEM_MANAGER_H_
