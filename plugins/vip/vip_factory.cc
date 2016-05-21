//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月21日 Author: kerry

#include "vip_factory.h"
#include "logic/logic_comm.h"

namespace vip_logic {

VIPFactory::VIPFactory() {
	Init();
}

VIPFactory::~VIPFactory() {

}

void VIPFactory::Init() {
	vip_usr_mgr_ = vip_logic::VIPUserEngine::GetVIPUserManager();

}

void VIPFactory::InitParam(config::FileConfig* config) {
	vip_db_ = new vip_logic::VIPDB(config);
	vip_usr_mgr_->Init(vip_db_);
}
void VIPFactory::Dest() {
	vip_logic::VIPUserEngine::FreeVIPUserEngine();
	vip_logic::VIPUserEngine::FreeVIPUserManager();
}

void VIPFactory::Test() {

	//vip_db_->FectchVIPUserInfo();
	int64 vid = 378;
	vip_logic::VIPUserInfo vip;
	vip_usr_mgr_->GetVIPUserInfo(vid,vip);
	LOG_MSG2("id %lld name %s introduction %s portrait %s",vip.id(),vip.name().c_str(),
			vip.introduction().c_str(),vip.portrait().c_str());
}
}

