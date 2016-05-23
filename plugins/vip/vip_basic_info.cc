//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月17日 Author: kerry
#include "vip_basic_info.h"

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
    int8 vip = 1;
    if (dict->GetCharInteger(L"vip", &vip))
        data_->vip_ = vip;

    dict->GetString(L"name", &data_->name_);
    dict->GetString(L"introduction", &data_->introduction_);
    dict->GetString(L"portrait", &data_->portrait_);

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

void ArticleInfo::ValueSerialization(base_logic::DictionaryValue* dict){
	dict->GetBigInteger(L"id", &data_->id_);
	dict->GetBigInteger(L"user_id", &data_->own_id_);

    dict->GetString(L"title", &data_->title_);
    dict->GetString(L"time", &data_->title_);
    dict->GetString(L"url", &data_->url_);
    dict->GetString(L"stock", &data_->stock_);
}

bool ArticleInfo::cmp(const vip_logic::ArticleInfo& t_article,
		const vip_logic::ArticleInfo& r_article) {
	return t_article.article_unix_time() > r_article.article_unix_time();
}


}
