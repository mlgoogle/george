//  Copyright (c) 2015-2015 The geprge Authors. All rights reserved.
//  Created on: 2016年5月18日 Author: kerry

#ifndef GEORGE_USER_DB_H_
#define GEORGE_USER_DB_H_

#include <string>
#include <list>
#include <map>
#include "user_info.h"
#include "logic/base_values.h"
#include "basic/basictypes.h"
#include "logic/base_values.h"
#include "basic/scoped_ptr.h"
#include "storage/data_engine.h"

namespace user_logic {

class UserDB {
 public:
  UserDB(config::FileConfig* config);
	virtual ~UserDB();
 public:
	bool RegisterUser(const std::string& phone,const std::string& password);
	bool LoginUser(const std::string& phone,const std::string& password,
	               user_logic::UserInfo& info);

	bool LoginSMS(const std::string& phone, user_logic::UserInfo& info);
 private:
	static void CallLoginUserInfo(void* param, base_logic::Value* value);
 private:
	base_logic::DataEngine*            mysql_engine_;
};
}
#endif
