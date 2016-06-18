//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月21日 Author: kerry

#include "vip_factory.h"
#include "vip_proto_buf.h"
#include "operator_code.h"
#include "basic/template.h"
#include "logic/logic_comm.h"
#include "check/token.h"
//#include "tools/tools.h"
#include <list>

namespace vip_logic {

VIPFactory*
VIPFactory::instance_ = NULL;

VIPFactory*
VIPFactory::GetInstance() {
	if (instance_ == NULL)
		instance_ =new VIPFactory();
	return instance_;
}

void VIPFactory::FreeInstance() {
	delete instance_;
	instance_ = NULL;
}


VIPFactory::VIPFactory() {
	Init();
}

VIPFactory::~VIPFactory() {
	if(packet_json_) {delete packet_json_; packet_json_ = NULL;}
	if(packet_jsonp_) {delete packet_jsonp_; packet_jsonp_ = NULL;}
}

void VIPFactory::Init() {
	vip_usr_mgr_ = vip_logic::VIPUserEngine::GetVIPUserManager();
	article_mgr_ = vip_logic::ArticleEngine::GetArticleManager();
	subcribe_mgr_ = vip_logic::SubcribeEngine::GetSubcribeManager();
	packet_json_ = new george_logic::json_packet::PacketProcess();
	packet_jsonp_ = new george_logic::jsonp_packet::PacketProcess();

}

void VIPFactory::InitParam(config::FileConfig* config) {
	vip_db_ = new vip_logic::VIPDB(config);
	vip_usr_mgr_->Init(vip_db_);
	article_mgr_->Init(vip_db_);
	subcribe_mgr_->Init(vip_db_);

}
void VIPFactory::Dest() {
	vip_logic::VIPUserEngine::FreeVIPUserEngine();
	vip_logic::VIPUserEngine::FreeVIPUserManager();
	vip_logic::ArticleEngine::FreeArticleEngine();
	vip_logic::ArticleEngine::FreeArticleManager();
}

void VIPFactory::Test() {

	std::string key = "pingan";
	std::string uid = "10081";
	std::string stoken;
	base_logic::StringValue* key_value = new base_logic::StringValue(key);
	base_logic::StringValue* uid_value = new base_logic::StringValue(uid);
	base_logic::TokenMgr* token = new base_logic::TokenMgr;
	stoken = token->CreateToken((base_logic::Value*)uid_value,(base_logic::Value*)key_value);

	token->CheckToken(uid,key,stoken);
	/*std::string key = "1888888888888888888";
	std::string t = tools::TeaEncode(key);
	std::string r = tools::TeaDecode(t);
	LOG_MSG2("%s",t.c_str());*/


	/*OnVIPNewsEvent(1);
	r = dict->GetString(L"response_type",&response_type);
	vip_db_->FectchVIPUserInfo();
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
	LOG_MSG2("%s",json.c_str());*/
}

void VIPFactory::TimeEvent(int opcode, int time) {
	switch (opcode) {
	  case TIME_UPDATE_ACRTICLE : {
		  article_mgr_->UpdateArticle();
		  break;
	  }
	  default:
		  break;
	}
}

void VIPFactory::OnHotVIPUser(const int socket,
		base_logic::DictionaryValue* dict,george_logic::PacketHead* packet) {
	vip_logic::net_request::HotVIP* hot_vip = new vip_logic::net_request::HotVIP;
	hot_vip->set_http_packet(dict);
	std::list<vip_logic::VIPUserInfo> list;
	vip_usr_mgr_->GetHotVIPUser(list, hot_vip->pos(),
			hot_vip->count());

	int count = list.size();
	vip_logic::net_reply::VIPUserList* vip_list = new vip_logic::net_reply::VIPUserList();

	int32 index = 0;
	std::list<vip_logic::VIPUserInfo>::iterator it = list.begin();
	for(it = list.begin();index < count,it != list.end();it++,index++) {
		vip_logic::VIPUserInfo vip_user = (*it);
		vip_logic::net_reply::VIPUser* user = new  vip_logic::net_reply::VIPUser();
		user->set_home_page(vip_user.home_page());
		user->set_introduction(vip_user.introduction());
		user->set_name(vip_user.name());
		user->set_protrait(vip_user.portrait());
		user->set_subscribe_count(vip_user.subscribe_count());
		user->set_vid(vip_user.id());
		user->set_vip(vip_user.vip());
		vip_list->set_vip_news(user->get());
	}

	/*vip_list->set_type(george_logic::VIP_TYPE);
	vip_list->set_operator_code(HOT_USER_RLY);

	packet_json_->PackPacket(socket, vip_list->packet());*/
	SendPacket(socket,vip_list,packet->attach_field(),
			HOT_USER_RLY,george_logic::VIP_TYPE);

	if (hot_vip) { delete hot_vip; hot_vip = NULL;}
	if (vip_list) { delete vip_list; vip_list = NULL;}
}


void VIPFactory::OnVIPArticle(const int socket,
			base_logic::DictionaryValue* dict,george_logic::PacketHead* packet) {
	vip_logic::net_request::VIPArticle* vip_article =
			new vip_logic::net_request::VIPArticle;
	vip_article->set_http_packet(dict);
	vip_logic::VIPUserInfo vip;

	//获取大V文章
	std::list<vip_logic::ArticleInfo> list;
	int32 count = list.size();
	article_mgr_->GetVIPArticle(vip_article->vid(),
			list,vip_article->pos(),vip_article->count(),
			vip_article->flag());

	//获取大V信息
	vip_usr_mgr_->GetVIPUserInfo(vip_article->vid(),vip);

	vip_logic::net_reply::VIPArticleList* vip_article_list =
			new vip_logic::net_reply::VIPArticleList();

	int32 index = 0;
	std::list<vip_logic::ArticleInfo>::iterator it;
	for(it = list.begin();index < count,it != list.end();it++,index++) {
		vip_logic::ArticleInfo article = (*it);
		vip_logic::net_reply::VIPNews* news = new  vip_logic::net_reply::VIPNews();
		news->set_aid(article.id());
		news->set_article_source(article.source_name());
		news->set_article_time(article.article_unix_time());
		news->set_title(article.title());
		news->set_vid(vip.id());
		news->set_name(vip.name());
		news->set_article_url(article.url());
		news->set_flag(article.type());
		if (article.type() == 1)
			vip_article_list->set_vip_article(news->get());
		else
			vip_article_list->set_vip_live(news->get());
	}

	//
	vip_logic::net_reply::VIPUser* user = new  vip_logic::net_reply::VIPUser();
	user->set_home_page(vip.home_page());
	user->set_introduction(vip.introduction());
	user->set_name(vip.name());
	user->set_protrait(vip.portrait());
	user->set_subscribe_count(vip.subscribe_count());
	user->set_vid(vip.id());
	user->set_vip(vip.vip());
	vip_article_list->set_vip_info(user->get());

	/*vip_article_list->set_operator_code(VIP_ARTICLE_RLY);
	vip_article_list->set_type(george_logic::VIP_TYPE);

	packet_json_->PackPacket(socket, vip_article_list->packet());*/
	SendPacket(socket,vip_article_list,packet->attach_field(),
			VIP_ARTICLE_RLY,george_logic::VIP_TYPE);
	if (vip_article) { delete vip_article; vip_article = NULL;}
	if (vip_article_list) {delete vip_article_list; vip_article_list = NULL;}
}

void VIPFactory::OnVIPNewsEvent(const int socket,
		base_logic::DictionaryValue* dict,george_logic::PacketHead* packet) {
	vip_logic::net_request::VIPNews* vip_news = new vip_logic::net_request::VIPNews;
	vip_news->set_http_packet(dict);
	//获取最新文章
	std::map<int64, vip_logic::VIPUserInfo>  map;
	std::list<vip_logic::ArticleInfo> list;
	article_mgr_->GetNewArticle(list,vip_news->pos(),vip_news->count());
	int count = list.size();
	int64* uid = new int64[count];
	int32 index = 0;
	std::list<vip_logic::ArticleInfo>::iterator it = list.begin();
	for(; it != list.end(),index < count; it++,index++) {
		uid[index] = (*it).own_id();
	}

	//获取大V信息
	vip_usr_mgr_->GetVIPUserInfo(uid,count,map);

	//组装最新消息
	vip_logic::net_reply::VIPNewsList* vip_list = new vip_logic::net_reply::VIPNewsList();
	index = 0;
	for(it = list.begin();index < count,it != list.end();it++,index++) {
		vip_logic::ArticleInfo article = (*it);
		int64 vid = article.own_id();
		vip_logic::VIPUserInfo vip;
		bool r = base::MapGet<VIPUSERINFO_MAP,VIPUSERINFO_MAP::iterator,
					int64,vip_logic::VIPUserInfo>(map,
							vid,vip);
		if (!r)
			continue;
		vip_logic::net_reply::VIPNews* news = new  vip_logic::net_reply::VIPNews();
		std::list<vip_logic::StockInfo> stock_list;
		article.stock_list(stock_list);
		news->set_aid(article.id());
		news->set_article_source(article.source_name());
		news->set_article_time(article.article_unix_time());
		news->set_title(article.title());
		news->set_vid(vip.id());
		news->set_name(vip.name());
		news->set_article_url(article.url());
		news->set_flag(article.type());
		news->set_code_name(stock_list);
		vip_list->set_vip_news(news->get());
	}



	/*vip_list->set_type(2);
	vip_list->set_operator_code(VIP_NEWS_RLY);
	vip_list->set_jsonpcallback(packet->attach_field()->callback());

	packet_jsonp_->PackPacket(socket, vip_list->packet());*/
	SendPacket(socket,vip_list,packet->attach_field(),
			VIP_NEWS_RLY,george_logic::VIP_TYPE);
	if (vip_list) {delete vip_list; vip_list = NULL;}
	if (uid) {delete [] uid; uid = NULL;}
	if (vip_news) { delete vip_news; vip_news = NULL;}

}

void VIPFactory::OnSetVIPSubcribe(const int socket,
			base_logic::DictionaryValue* dict,george_logic::PacketHead* packet) {
	vip_logic::net_request::SetSubcribeVIP* set_subcribe_vip =
			new vip_logic::net_request::SetSubcribeVIP;

	set_subcribe_vip->set_http_packet(dict);

	subcribe_mgr_->SetSubcribeInfo(set_subcribe_vip->uid(),
			set_subcribe_vip->vid());

	SendHeader(socket, packet->attach_field(),VIP_SETSUB_RLY,
			george_logic::VIP_TYPE);
	/*george_logic::PacketHead * head = new george_logic::PacketHead();
	SendPacket(socket,head,packet->attach_field(),
			VIP_SETSUB_RLY,george_logic::VIP_TYPE);
	/*head->set_type(george_logic::VIP_TYPE);
	head->set_operator_code(VIP_SETSUB_RLY);
	packet_json_->PackPacket(socket, head->packet());
	if(head) {delete head; head = NULL;}*/
}

void VIPFactory::OnNewsDigest(const int socket,
			base_logic::DictionaryValue* dict,george_logic::PacketHead* packet) {
	vip_logic::net_request::VIPNewsDigest* digest =
			new vip_logic::net_request::VIPNewsDigest;
	digest->set_http_packet(dict);
	vip_logic::ArticleInfo article;

	//查询文章
	bool r = article_mgr_->GetArticleInfo(digest->article_id(), article);
	vip_logic::net_reply::VIPNewsDigest* news_digest =
			new vip_logic::net_reply::VIPNewsDigest;
	if (r){
		news_digest->set_digest(article.digest());
		news_digest->set_summary(article.full_text());
		news_digest->set_url(article.url());
	}

	SendPacket(socket,news_digest,packet->attach_field(),
			VIP_NEWDIGEST_RLY,george_logic::VIP_TYPE);
	if (digest) { delete digest; digest = NULL;}
	if (news_digest) { delete news_digest; news_digest = NULL;}
}

void VIPFactory::OnUserSubcribe(const int socket,
	    	base_logic::DictionaryValue* dict,george_logic::PacketHead* packet) {
	vip_logic::net_request::SubcribeVIP* subcribe_vip =
			new vip_logic::net_request::SubcribeVIP;

	subcribe_vip->set_http_packet(dict);
	int64* vid = NULL;

	int count = subcribe_vip->count();

	int32 now_count = subcribe_mgr_->GetSubcribeInfo(subcribe_vip->uid(),
			subcribe_vip->pos(),subcribe_vip->count(),
			&vid);


	std::map<int64, vip_logic::VIPUserInfo>  map;
	std::map<int64,vip_logic::ArticleInfo>   amap;
	//获取大V信息
	vip_usr_mgr_->GetVIPUserInfo(vid,now_count,map);
	//获取大V对应的最新文章
	article_mgr_->GetVIPNewArticle(vid,now_count,amap);


	vip_logic::net_reply::VIPUserList* vip_list = new vip_logic::net_reply::VIPUserList();

	int32 index = 0;
	std::map<int64, vip_logic::VIPUserInfo>::iterator it = map.begin();

	index = 0;
	for(;index < count,it != map.end();it++,index++) {
		vip_logic::VIPUserInfo vip_user = it->second;
		vip_logic::ArticleInfo article;
		bool r = base::MapGet<std::map<int64,vip_logic::ArticleInfo> ,
				std::map<int64,vip_logic::ArticleInfo> ::iterator,
						int64,vip_logic::ArticleInfo>(amap,
								vip_user.id(),article);
		//if (!r)
			//continue;
		vip_logic::net_reply::VIPNews* news = new  vip_logic::net_reply::VIPNews();
		if (r) {
			news->set_aid(article.id());
			news->set_article_source(article.source_name());
			news->set_article_time(article.article_unix_time());
			news->set_title(article.title());
			news->set_article_url(article.url());
			news->set_flag(article.type());
		}else{
			news->set_article_url(article.url());
			news->set_flag(article.type());
			news->set_title("暂无最新文章");
		}

		news->set_vid(vip_user.id());
		news->set_name(vip_user.name());
		news->set_subcribe_count(vip_user.subscribe_count());
		news->set_introduction(vip_user.introduction());
		news->set_protrait(vip_user.portrait());
		vip_list->set_vip_news(news->get());
	}


	/*for(it = map.begin();index < now_count,it != map.end();it++,index++) {
		vip_logic::VIPUserInfo vip_user = it->second;
		vip_logic::net_reply::VIPUser* user = new  vip_logic::net_reply::VIPUser();
		user->set_home_page(vip_user.home_page());
		user->set_introduction(vip_user.introduction());
		user->set_name(vip_user.name());
		user->set_protrait(vip_user.portrait());
		user->set_subscribe_count(vip_user.subscribe_count());
		user->set_vid(vip_user.id());
		user->set_vip(vip_user.vip());

		vip_list->set_vip_news(user->get());
	}*/
	/*vip_list->set_type(george_logic::VIP_TYPE);
	vip_list->set_operator_code(VIP_SUBCRIBE_RLY);
	packet_json_->PackPacket(socket, vip_list->packet());*/
	SendPacket(socket,vip_list,packet->attach_field(),
			VIP_SUBCRIBE_RLY,george_logic::VIP_TYPE);
	if (vid) {delete [] vid; vid = NULL;}
	if (subcribe_vip) { delete subcribe_vip; subcribe_vip = NULL;}
	if (vip_list) { delete vip_list; vip_list = NULL;}
}


void VIPFactory::SendPacket(const int socket, george_logic::PacketHead* packet,
		george_logic::AttachField* attach,
		const int16 operator_code, const int8 type) {
	packet->set_operator_code(operator_code);
	packet->set_type(type);
	if (attach != NULL && attach->format()=="jsonp") {
		packet->attach_field()->set_callback(attach->callback());
				packet_jsonp_->PackPacket(socket,packet->packet());
	} else
		packet_json_->PackPacket(socket,packet->packet());
}

void VIPFactory::SendError(const int socket,george_logic::AttachField* attach,
			const int16 operator_code) {
	SendHeader(socket, attach, operator_code, george_logic::ERROR_TYPE);
}


void VIPFactory::SendHeader(const int socket,george_logic::AttachField* attach,
			const int16 operator_code, const int8 type) {
	george_logic::PacketHead *header  = new george_logic::PacketHead();
	SendPacket(socket, header, attach, operator_code, type);
	if (header) {delete header;header = NULL;}
}



}

