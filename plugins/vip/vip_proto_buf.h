//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月21日 Author: kerry

#ifndef GEORGE_VIP_PROTO_BUF_H__
#define GEORGE_VIP_PROTO_BUF_H__

#include "basic/basictypes.h"
#include "logic/base_values.h"
#include "net/proto_buf.h"
#include <list>
#include <string>

namespace vip_logic {

namespace net_request {
class VIPNews : public george_logic::PacketHead{
 public:
	VIPNews() {
		uid_ = pos_ = count_ = NULL;
		token_ = NULL;
	}

	~VIPNews() {
		if (uid_) {delete uid_; uid_ = NULL;}
		if (token_) {delete token_; token_ = NULL;}
		if (pos_) {delete pos_; pos_ = NULL;}
		if (count_) {delete count_; count_ = NULL;}
	}

	virtual void set_http_packet(base_logic::DictionaryValue* value);

 public:
	base_logic::FundamentalValue*     uid_;
	base_logic::StringValue*          token_;
	base_logic::FundamentalValue*     pos_;
	base_logic::FundamentalValue*     count_;
};
}

namespace net_reply {
class VIPNews{
 public:
	VIPNews() {
		article_type_ =  NULL;
		vid_ = aid_ =  article_time_ = NULL;
		name_ = title_ = article_source_  =  NULL;
		value_ = NULL;

	}
	void set_vid(const int64 vid) {vid_ = new base_logic::FundamentalValue(vid);}
	void set_aid(const int64 aid) {aid_ = new base_logic::FundamentalValue(aid);}
	void set_name(const std::string& name) {name_ = new base_logic::StringValue(name);}
	void set_title(const std::string& title) {title_ = new base_logic::StringValue(title);}
	void set_article_source(const std::string& article_source) {article_source_
		= new base_logic::StringValue(article_source);}
	void set_article_time(const int64 article_time) {article_time_ =
			new base_logic::FundamentalValue(article_time);}

	~VIPNews() {
		if(vid_) {delete vid_; vid_ = NULL;}
		if(aid_) {delete aid_; aid_ = NULL;}
		if(name_) {delete name_; name_ = NULL;}
		if(title_) {delete title_; title_ = NULL;}
		if(article_source_) {delete article_source_; article_source_ = NULL;}
		if(article_type_) {delete article_type_; article_type_ = NULL;}
		if(article_time_) {delete article_time_; article_time_ = NULL;}
	}

	base_logic::DictionaryValue* get() {
		value_ = new base_logic::DictionaryValue();
		if(vid_ != NULL)
			value_->Set(L"vid",vid_);
		if(aid_ != NULL)
			value_->Set(L"aid",aid_);
		if(name_ != NULL)
			value_->Set(L"name",name_);
		if(title_ != NULL)
			value_->Set(L"title",title_);
		if(article_source_ != NULL)
			value_->Set(L"article_source",article_source_);
		if(article_type_ != NULL)
			value_->Set(L"article_type",article_type_);
		if(article_time_ != NULL)
			value_->Set(L"article_time",article_time_);
		return value_;
	}
 private:
	base_logic::FundamentalValue*       vid_;
	base_logic::FundamentalValue*       aid_;
	base_logic::StringValue*            name_;
	base_logic::StringValue*            title_;
	base_logic::StringValue*            article_source_;
	base_logic::ListValue*              article_type_;
	base_logic::FundamentalValue*       article_time_;
	base_logic::DictionaryValue*        value_;
};

class VIPNewsList: public george_logic::PacketHead{
 public:
	VIPNewsList(){
		list_ = new base_logic::ListValue;
	}
	~VIPNewsList() {
		if (list_) {delete list_; list_ = NULL;}
		if (body_value_) {delete body_value_; body_value_ = NULL;}
	}

	void set_vip_news(base_logic::Value* value) {
		list_->Append(value);
	}

	base_logic::DictionaryValue* body() {
		body_value_ = new base_logic::DictionaryValue();
		body_value_->SetWithoutPathExpansion(L"list",list_);
		body_value_->SetInteger(L"count",list_->GetSize());
		return body_value_;
	}

	void set_http_packet(base_logic::DictionaryValue* value) {}
 private:
	base_logic::ListValue*         list_;
};

}
}
#endif
