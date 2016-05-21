//  Copyright (c) 2015-2015 The geprge Authors. All rights reserved.
//  Created on: 2016年5月18日 Author: kerry

#ifndef GEORGE_VIP_DB_H_
#define GEORGE_VIP_DB_H_

#include <string>
#include <list>
#include <map>
#include "logic/base_values.h"
#include "basic/basictypes.h"
#include "logic/base_values.h"
#include "basic/scoped_ptr.h"
#include "storage/data_engine.h"

namespace vip_logic {

class VIPDB {
 public:
	VIPDB(config::FileConfig* config);
	virtual ~VIPDB();
 public:
	bool FectchVIPUserInfo();
 public:
	static void CallFectchVIPUserInfo(void* param,
            base_logic::Value* value);
 private:
	base_logic::DataEngine*            mysql_engine_;
};
}
#endif
