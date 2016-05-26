//  Copyright (c) 2016-2016 The george Authors. All rights reserved.
//  Created on: 2016年5月21日 Author: kerry
#ifndef GEORGE_VIP_FACTORY_H__
#define GEORGE_VIP_FACTORY_H__

#include "vip_user_mgr.h"
#include "vip_article_mgr.h"
#include "vip_db.h"
#include "vip_proto_buf.h"
#include "vip_basic_info.h"
#include "basic/basictypes.h"
#include "basic/scoped_ptr.h"
#include "net/packet_process.h"

namespace vip_logic {

class VIPFactory {
 public:
	VIPFactory();
	virtual ~VIPFactory();
 public:
	void Init();
	void Dest();
 private:
	static VIPFactory*  instance_;
 public:
	static VIPFactory* GetInstance();
	static void FreeInstance();

	void InitParam(config::FileConfig* config);

	void Test();
 public:
	void OnVIPNewsEvent(const int socket,
			base_logic::DictionaryValue* dict);
 private:
	VIPUserManager*                                     vip_usr_mgr_;
	ArticleManager*                                     article_mgr_;
	vip_logic::VIPDB*                                   vip_db_;
	george_logic::json_packet::PacketProcess*           packet_;
};
}
#endif
