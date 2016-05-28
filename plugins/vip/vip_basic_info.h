//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月15日 Author: kerry


#ifndef GEORGE_VIP_BASIC_INFO_H_
#define GEORGE_VIP_BASIC_INFO_H_


#include "basic/basictypes.h"
#include "logic/base_values.h"
#include <list>
#include <string>

namespace vip_logic {

class StockInfo {
 public:
	StockInfo();
	StockInfo(const StockInfo& stock_info);
	StockInfo& operator = (const StockInfo& stock_info);

	~StockInfo() {
		if (data_ != NULL) {
			data_->Release();
		}
	}

	const void set_stock_name(const std::string& stock_name) {data_->stock_name_
		= stock_name;}

	const void set_stock_code(const std::string& stock_code) {data_->stock_code_
		= stock_code;}

	const std::string& stock_name() const {return data_->stock_name_;}
	const std::string& stock_code() const {return data_->stock_code_;}

	base_logic::DictionaryValue* get() {
		base_logic::DictionaryValue* value = new base_logic::DictionaryValue();
		value->SetString(L"code",data_->stock_code_);
		value->SetString(L"name",data_->stock_name_);
		return value;
	}


 private:
	class Data {
	public:
		Data()
		:refcount_(1) {}
	public:
		std::string  stock_code_;
		std::string  stock_name_;
		void AddRef() {__sync_fetch_and_add(&refcount_, 1);}
		void Release() {__sync_fetch_and_sub(&refcount_, 1);
        	if (!refcount_)delete this;
		}
	private:
		int  refcount_;
	};


	Data*  data_;
};


class ArticleInfo {
 public:
	ArticleInfo();
	ArticleInfo(const ArticleInfo& article_info);
	ArticleInfo& operator = (const ArticleInfo& article_info);

	~ArticleInfo() {
		if (data_ != NULL) {
			data_->Release();
		}
	}

	void set_id(const int64 id) {
		data_->id_ = id;
	}

	void set_own_id(const int64 own_id) {
		data_->own_id_ = own_id;
	}
	void set_forward_count(const int64 forward_count) {
		data_->forward_count_ = forward_count;
	}

	void set_comment_count(const int64 comment_count) {
		data_->comment_count_ = comment_count;
	}

	void set_recommend_count(const int64 recommend_count) {
		data_->recommend_count_ = recommend_count;
	}

	void set_cheer_count(const int64 cheer_count) {
		data_->cheer_count_ = cheer_count;
	}

	void set_read_count(const int64 read_count) {
		data_->read_count_ = read_count;
	}

	void set_like_count(const int64 like_count) {
		data_->like_count_ = like_count;
	}

	void set_collection_count(const int64 collection_count) {
		data_->collection_count_ = collection_count;
	}

	void set_is_buy(const bool is_buy) {
		data_->is_buy_ = is_buy;
	}

	void set_is_top(const bool is_top) {
		data_->is_top_ = is_top;
	}

	void set_title(const std::string& title){
		data_->title_ = title;
	}

	void set_full_text(const std::string& full_text) {
		data_->full_text_ = full_text;
	}

	void set_article_time(const std::string& article_time) {
		data_->article_time_ = article_time;
	}

	void set_digest(const std::string& digest) {
		data_->digest_ = digest;
	}

	void set_url(const std::string& url) {
		data_->url_ = url;
	}

	void set_source(const int32 source) {
		data_->source_ = source;
	}

	void set_indus(const std::string& indus) {
		data_->indus_ = indus;
	}

	void set_sect(const std::string& sect) {
		data_->sect_ = sect;
	}

	void set_stock(const std::string& stock) {
		data_->stock_ = stock;
	}

	void set_stock(const vip_logic::StockInfo& stock){
		data_->stock_list_.push_back(stock);
	}

	void analyzer_stock(const std::string& str);


	void set_platname(const int32 plt_id);

	const int64 own_id() const {return data_->own_id_;}
	const int64 id() const {return data_->id_;}
	const int64 forward_count() const {return data_->forward_count_;}
	const int64 comment_count() const {return data_->comment_count_;}
	const int64 recommend_count() const {return data_->recommend_count_;}
	const int64 cheer_count() const {return data_->cheer_count_;}
	const int64 read_count() const {return data_->read_count_;}
	const int64 like_count() const {return data_->like_count_;}
	const int64 collection_count() const {return data_->collection_count_;}
	const int64 article_unix_time() const {return data_->article_unix_time_;}
	const bool is_buy() const {return data_->is_buy_;}
	const bool is_top() const {return data_->is_top_;}
	const int32 source() const {return data_->source_;}
	const std::string& source_name() {return data_->source_name_;}
	const std::string& title() const {return data_->title_;}
	const std::string& full_text() const {return data_->full_text_;}
	const std::string& article_time() const {return data_->article_time_;}
	const std::string& digest() const {return data_->digest_;}
	const std::string& url() const {return data_->url_;}
	const std::string& indus() const {return data_->indus_;}
	const std::string& sect() const {return data_->sect_;}
	const std::string& stock() const {return data_->stock_;}

	void stock_list(std::list<vip_logic::StockInfo>& list) {
		list = data_->stock_list_;
	}



	static bool cmp(const vip_logic::ArticleInfo& t_article,
			const vip_logic::ArticleInfo& r_article);

	void ValueSerialization(base_logic::DictionaryValue* dict);

	class Data {
	 public:
		Data()
	 	 :refcount_(1)
	 	 ,id_(0)
	 	 ,own_id_(0)
	 	 ,forward_count_(-1)
	 	 ,comment_count_(-1)
	 	 ,recommend_count_(-1)
	 	 ,cheer_count_(-1)
	 	 ,read_count_(-1)
	 	 ,like_count_(-1)
	 	 ,source_(-1)
	 	 ,collection_count_(-1)
	 	 ,type_(0)
	 	 ,article_unix_time_(0)
	 	 ,is_buy_(false)
	 	 ,is_top_(false){}

	 public:
		int64       id_;
		int64       own_id_;
		int64       forward_count_;
		int64       comment_count_;
		int64       recommend_count_;
		int64       cheer_count_;//淘股吧 加油券
		int64       read_count_;
		int64       like_count_;
		int64       collection_count_;
		int64       article_unix_time_;
		int8        type_;
		int32       source_;
		bool        is_buy_;
		bool        is_top_;
		std::string digest_;
		std::string title_;
		std::string full_text_;
		std::string article_time_;
		std::string url_;
		std::string indus_;
		std::string          sect_;
		std::string          stock_;
		std::string          source_name_;
		std::list<vip_logic::StockInfo> stock_list_;

		void AddRef() {__sync_fetch_and_add(&refcount_, 1);}
		void Release() {__sync_fetch_and_sub(&refcount_, 1);
		        	if (!refcount_)delete this;
		}
	  private:
		int            refcount_;
	};

	Data*             data_;
};

class VIPUserInfo {
 public:
	VIPUserInfo();
	VIPUserInfo(const VIPUserInfo& vip_info);
	VIPUserInfo& operator = (const VIPUserInfo& vip_info);

	~VIPUserInfo() {
		if (data_ != NULL) {
			data_->Release();
		}
	}

	static bool cmp(const vip_logic::VIPUserInfo& t_vip,
				const vip_logic::VIPUserInfo& r_vip);

	void set_id(const int64 id) {data_->id_ = id;}

	void set_followers_count(const int64 followers_count) {
		data_->followers_count_ = followers_count;
	}

	void set_subscribe_count(const int64 subscribe_count) {
		data_->subscribe_count_ = subscribe_count;
	}

	void set_is_vip(const int8 vip) {data_->vip_ = vip;}

	void set_name(const std::string& name) {data_->name_ = name;}

	void set_home_page(const std::string& home_page) {data_->home_page_ = home_page;}


	void set_introduction(const std::string& introduction) {
		data_->introduction_ = introduction;
	}
    
    void set_portrait(const std::string& portrait) {
        data_->portrait_ = portrait;
    }
    
    const int64 id() const {return data_->id_;}
    const int64 followers_count() const {return data_->followers_count_;}
    const int64 subscribe_count() const {return data_->subscribe_count_;}
    const int8 vip() const {return data_->vip_;}
    const std::string& name() const {return data_->name_;}
    const std::string& introduction() const {return  data_->introduction_;}
    const std::string& portrait() const {return data_->portrait_;}
    const std::string& home_page() const {return data_->home_page_;}


    void ValueSerialization(base_logic::DictionaryValue* dict);
 private:
	class Data {
	public:
		Data()
		:refcount_(1)
		,id_(0)
		,followers_count_(0)
		,subscribe_count_(0)
		,vip_(0){}

	 public:
		int64            id_;
		int64            followers_count_;
		int64            subscribe_count_;
		int8             vip_;
		std::string      name_;
		std::string      home_page_;
		std::string      introduction_;
		std::string      portrait_;


		void AddRef() {__sync_fetch_and_add(&refcount_, 1);}
		void Release() {__sync_fetch_and_sub(&refcount_, 1);
        	if (!refcount_)delete this;
		}
	  private:
		int            refcount_;
	};

	Data*             data_;
};



}

#endif
