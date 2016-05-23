//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月21日 Author: kerry

#include "vip_factory.h"
#include "vip_proto_buf.h"
#include "logic/logic_comm.h"

namespace vip_logic {

VIPFactory::VIPFactory() {
	Init();
}

VIPFactory::~VIPFactory() {

}

void VIPFactory::Init() {
	vip_usr_mgr_ = vip_logic::VIPUserEngine::GetVIPUserManager();
	article_mgr_ = vip_logic::ArticleEngine::GetArticleManager();

}

void VIPFactory::InitParam(config::FileConfig* config) {
	vip_db_ = new vip_logic::VIPDB(config);
	vip_usr_mgr_->Init(vip_db_);
	article_mgr_->Init(vip_db_);
}
void VIPFactory::Dest() {
	vip_logic::VIPUserEngine::FreeVIPUserEngine();
	vip_logic::VIPUserEngine::FreeVIPUserManager();
	vip_logic::ArticleEngine::FreeArticleEngine();
	vip_logic::ArticleEngine::FreeArticleManager();
}

void VIPFactory::Test() {

	//vip_db_->FectchVIPUserInfo();
	int64 vid = 231008;
	vip_logic::VIPUserInfo vip;
	vip_usr_mgr_->GetVIPUserInfo(vid,vip);
	LOG_MSG2("id %lld name %s introduction %s portrait %s",vip.id(),vip.name().c_str(),
			vip.introduction().c_str(),vip.portrait().c_str());


	vip_logic::net_reply::VIPNewsList* list = new vip_logic::net_reply::VIPNewsList();
	int64 uid[] = {231008,231009,231010,231011,231012,231013,231014,231015};
	int32 n = sizeof(uid) / sizeof(int64);
	for(int32 i = 0; i < n; i++) {
		vip_logic::VIPUserInfo user_info;
		bool r = vip_usr_mgr_->GetVIPUserInfo(uid[i],user_info);
		if (!r)
			continue;
		vip_logic::net_reply::VIPNews* news = new  vip_logic::net_reply::VIPNews();
		news->set_vid(user_info.id());
		news->set_name(user_info.name());
		list->set_vip_news(news->get());
	}

	list->set_is_zip_encrypt(0);
	list->set_packet_length(16);
	list->set_operator_code(1001);
	list->set_data_length(0);
	list->set_reserved(0);
	list->set_sessionid(0);
	list->set_signature(0);
	list->set_type(0);
	list->set_timestamp(0);

	std::string json;
	base_logic::ValueSerializer* engine = base_logic::ValueSerializer::Create(base_logic::IMPL_JSON, &json);
	engine->Serialize(*list->packet());
	LOG_MSG2("%s",json.c_str());
}

void VIPFactory::OnVIPNewsEvent(const int socket) {

}


}

