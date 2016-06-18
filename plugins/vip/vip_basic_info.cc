//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月17日 Author: kerry
#include "vip_basic_info.h"
#include "basic/basic_util.h"

namespace vip_logic {

VIPUserInfo::VIPUserInfo() {
	data_ = new Data();
}

VIPUserInfo::VIPUserInfo(const VIPUserInfo& vip_info)
:data_(vip_info.data_){
	if (data_ != NULL) {
		data_->AddRef();
	}
}

VIPUserInfo& VIPUserInfo::operator =(
		const VIPUserInfo& vip_info) {
	if(vip_info.data_ != NULL){
		vip_info.data_->AddRef();
	}

	if (data_ != NULL) {
		data_->Release();
	}

	data_ = vip_info.data_;
	return (*this);
}

void VIPUserInfo::ValueSerialization(base_logic::DictionaryValue* dict){
	dict->GetBigInteger(L"id", &data_->id_);
	dict->GetBigInteger(L"followers_count", &data_->followers_count_);
	dict->GetBigInteger(L"subscribed", &data_->subscribe_count_);
    int8 vip = 1;
    if (dict->GetCharInteger(L"official_vip", &vip))
        data_->vip_ = vip;

    dict->GetString(L"name", &data_->name_);
    dict->GetString(L"home_page", &data_->home_page_);
    dict->GetString(L"introduction", &data_->introduction_);
    dict->GetString(L"portrait", &data_->portrait_);

}

bool VIPUserInfo::cmp(const vip_logic::VIPUserInfo& t_vip,
				const vip_logic::VIPUserInfo& r_vip) {
	return t_vip.subscribe_count() > r_vip.subscribe_count();
}


ArticleInfo::ArticleInfo() {
	data_ = new Data();
}

ArticleInfo::ArticleInfo(const ArticleInfo& article_info)
:data_(article_info.data_) {
	if (data_ != NULL) {
		data_->AddRef();
	}
}

ArticleInfo& ArticleInfo::operator =(
		const ArticleInfo& article_info){
	if (article_info.data_ != NULL){
		article_info.data_->AddRef();
	}

	if (data_ != NULL){
		data_->Release();
	}

	data_ = article_info.data_;
	return (*this);
}

void ArticleInfo::set_platname(const int32 plt_id) {

	switch (plt_id) {
	  case 40001: {
		  data_->source_name_ = "雪球";
		  break;
	  }
	  case 40003: {
		  data_->source_name_ = "摩尔金融";
		  break;
	  }
	  case 40002 : {
		  data_->source_name_ = "中金博客";
		  break;
	  }
	  case 40004 : {
		  data_->source_name_ = "淘股吧";
		  break;
	  }
	  case 40005 : {
		  data_->source_name_ = "微博";
		  break;
	  }

	  default:{
		  data_->source_name_ = "未知";
		  break;
	  }
	}
}

void ArticleInfo::ValueSerialization(base_logic::DictionaryValue* dict){
	dict->GetBigInteger(L"id", &data_->id_);
	dict->GetBigInteger(L"user_id", &data_->own_id_);

    dict->GetString(L"title", &data_->title_);
    dict->GetBigInteger(L"unix_time", &data_->article_unix_time_);
    dict->GetString(L"url", &data_->url_);
    dict->GetString(L"stock", &data_->stock_);
    dict->GetInteger(L"source", &data_->source_);
    int8 type = 0;
    dict->GetCharInteger(L"type", &type);
    data_->type_ = type;
    bool r = dict->GetInteger(L"source", &data_->source_);
    if (r)
    	set_platname(data_->source_);
    std::string stock_info;
    if (dict->GetString(L"stock",&stock_info)) {
    	analyzer_stock(stock_info);
    }

    dict->GetString(L"digest",&data_->digest_);
    dict->GetString(L"summary",&data_->full_text_);
}



bool ArticleInfo::cmp(const vip_logic::ArticleInfo& t_article,
		const vip_logic::ArticleInfo& r_article) {
	return t_article.id() > r_article.id();
}

void ArticleInfo::analyzer_stock(const std::string& str) {
	std::string stock_str = str;
	while (stock_str.length()!=0){
		size_t start_pos = stock_str.find("=");
		size_t end_pos = stock_str.find("&")==std::string::npos?str.length():stock_str.find("&");
		std::string code = stock_str.substr(0,start_pos);
		std::string name = stock_str.substr(code.length()+1,end_pos - code.length()-1);
		StockInfo stock;
		stock.set_stock_code(code);
		stock.set_stock_name(name);
		data_->stock_list_.push_back(stock);
		if (stock_str.find("&") != std::string::npos)
			stock_str = stock_str.substr(end_pos+1,stock_str.length());
		else
			stock_str.clear();
	}
}




StockInfo::StockInfo() {
	data_ = new Data();
}

StockInfo::StockInfo(const StockInfo& stock_info)
:data_(stock_info.data_) {
	if (data_ != NULL) {
		data_->AddRef();
	}
}

StockInfo& StockInfo::operator =(
		const StockInfo& stock_info){
	if (stock_info.data_ != NULL){
		stock_info.data_->AddRef();
	}

	if (data_ != NULL){
		data_->Release();
	}

	data_ = stock_info.data_;
	return (*this);
}

SubcribeInfo::SubcribeInfo() {
	data_ = new Data();
}

SubcribeInfo::SubcribeInfo(const SubcribeInfo& subcribe_info)
:data_(subcribe_info.data_) {
	if (data_ != NULL) {
		data_->AddRef();
	}
}

SubcribeInfo& SubcribeInfo::operator =(
		const SubcribeInfo& subcribe_info) {

	if (subcribe_info.data_ != NULL){
		subcribe_info.data_->AddRef();
	}

	if (data_ != NULL){
		data_->Release();
	}

	data_ = subcribe_info.data_;
	return (*this);
}

void SubcribeInfo::ValueSerialization(base_logic::DictionaryValue* dict) {
	dict->GetString(L"uid", &data_->uid_);
	std::string subcribe_str;

	dict->GetString(L"subcribe",&subcribe_str);

	while (subcribe_str.length()>0){
		size_t end_pos = subcribe_str.find(",");
		std::string subcribe_id = subcribe_str.substr(0,end_pos);
		set_subcribe_id(atoll(subcribe_id.c_str()));
		if (subcribe_str.find(",") != std::string::npos)
			subcribe_str = subcribe_str.substr(end_pos+1,subcribe_str.length());
		else
			subcribe_str.clear();
	}
}

bool SubcribeInfo::get_subcirbe_string(std::string& str){
	base_logic::RLockGd lk(data_->lock_);
	std::map<int64,int64>::iterator it =
			data_->subcribe_map_.begin();
	int32 map_size = data_->subcribe_map_.size();
	int32 index =0;
	for (; it != data_->subcribe_map_.end();it++,index++) {
		str.append(base::BasicUtil::StringUtil::Int64ToString(it->first));
		if(index < (map_size - 1))
			str.append(",");
	}
	return true;
}


}
