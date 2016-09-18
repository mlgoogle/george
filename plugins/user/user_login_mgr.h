//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月21日 Author: kerry

#ifndef USER_LOGIN_MGR_H_
#define USER_LOGIN_MGR_H_

#include "user_db.h"
#include "storage/data_engine.h"
#include "basic/basictypes.h"
#include "thread/base_thread_lock.h"
#include "user_info.h"
#include <list>


namespace user_logic {

typedef std::map<std::string, user_logic::LoginVercode> VERCODE_MAP;

class LoginCache {
 public:
  VERCODE_MAP vercode_map_;
};


class LoginManager {
 public:
  LoginManager();
  virtual ~LoginManager();
 private:
  void Init();
 public:
  void Init(user_logic::UserDB* user_db);

  bool CreateVerCode(const std::string& phone,int32& vercode);

  bool LoginUser(const std::string& phone,const std::string& password,
                 user_logic::UserInfo& info);

  bool LoginSMSUser(const std::string& phone,const int32 vercode,
                    user_logic::UserInfo& info);

 private:
  int32 CreateVercode();
 private:
  user_logic::UserDB*        user_db_;
  struct threadrw_t*         lock_;
  LoginCache*                login_cache_;
};

class LoginEngine {
 private:
  static LoginManager*  login_mgr_;
  static LoginEngine* login_engine_;

  LoginEngine() {}
  virtual ~LoginEngine() {}
 public:
  static LoginManager* GetLoginManager() {
    if (login_mgr_ == NULL)
      login_mgr_ = new LoginManager();
    return login_mgr_;
  }

  static LoginEngine* GetLoginEngine() {
    if (login_engine_ == NULL)
      login_engine_ = new LoginEngine();
    return login_engine_;
  }

  static void FreeLoginEngine(){
    delete login_engine_;
    login_engine_ = NULL;
  }

  static void FreeLoginManager() {
    delete login_mgr_;
    login_mgr_ = NULL;
  }
};
}


#endif /* USER_LOGIN_MGR_H_ */
