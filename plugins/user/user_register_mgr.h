//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月21日 Author: kerry
#ifndef USER_REGISTER_MGR_H_
#define USER_REGISTER_MGR_H_

#include "user_db.h"
#include "storage/data_engine.h"
#include "basic/basictypes.h"
#include "thread/base_thread_lock.h"
#include "user_info.h"
#include <list>


namespace user_logic {

typedef std::map<std::string, user_logic::RegiserVercode> VERCODE_MAP;

class RegisterCache {
 public:
  VERCODE_MAP vercode_map_;
};

class RegisterManager {
 public:
  RegisterManager();
  virtual ~RegisterManager();
 private:
  void Init();
 public:
  void Init(user_logic::UserDB* user_db);

  bool CreateVerCode(const std::string& phone,int32& vercode);

  bool RegisterUser(const std::string& phone,const std::string& password,
                     const int32 vercode);

 private:
  int32 CreateVercode();
 private:
  user_logic::UserDB*        user_db_;
  struct threadrw_t*         lock_;
  RegisterCache*             register_cache_;
};

class RegisterEngine {
 private:
  static RegisterManager*  register_mgr_;
  static RegisterEngine* register_engine_;

  RegisterEngine() {}
  virtual ~RegisterEngine() {}
 public:
  static RegisterManager* GetRegisterManager() {
    if (register_mgr_ == NULL)
      register_mgr_ = new RegisterManager();
    return register_mgr_;
  }

  static RegisterEngine* GetRegisterEngine() {
    if (register_engine_ == NULL)
      register_engine_ = new RegisterEngine();
    return register_engine_;
  }

  static void FreeRgisterEngine(){
    delete register_engine_;
    register_engine_ = NULL;
  }

  static void FreeRegisterManager() {
    delete register_mgr_;
    register_mgr_ = NULL;
  }
};
}



#endif /* USER_REGISTER_MGR_H_ */
