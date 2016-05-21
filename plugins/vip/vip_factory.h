//  Copyright (c) 2016-2016 The george Authors. All rights reserved.
//  Created on: 2016年5月21日 Author: kerry
#ifndef GEORGE_VIP_FACTORY_H__
#define GEORGE_VIP_FACTORY_H__

#include "vip_user_mgr.h"
#include "vip_db.h"
#include "vip_basic_info.h"
#include "basic/basictypes.h"
#include "basic/scoped_ptr.h"

namespace vip_logic {

class VIPFactory {
 public:
	VIPFactory();
	virtual ~VIPFactory();
 public:
	void Init();
	void Dest();

	void InitParam(config::FileConfig* config);

	void Test();
 private:
	VIPUserManager*    vip_usr_mgr_;
	vip_logic::VIPDB*  vip_db_;
};
}
#endif
