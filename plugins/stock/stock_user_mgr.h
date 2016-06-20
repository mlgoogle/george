//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月18日 Author: kerry

#ifndef GEORGE_VIP_USER_MGR_H__
#define GEORGE_VIP_USER_MGR_H__

#include "storage/data_engine.h"
#include "basic/basictypes.h"
#include "thread/base_thread_lock.h"
#include <map>
#include <set>

#include "stock_basic_info.h"
#include "stock_db.h"

typedef std::map<std::string,stock_logic::StockTotalInfo> STOCKINFO_MAP;
typedef stock_logic::IndustryInfo INDUSTRYINFO_MAP;
typedef std::map< int, std::vector<int> > MARKET_LIMIT;
typedef std::map<std::string, std::string> STOCKS_PER_INDUSTRY_JSON;

namespace stock_logic {

class StockExchange {
public:
	double shanghai_exchange() const{return shanghai_exchange_;};

	void set_shanghai_exchange(double shanghai_exchange) {
		shanghai_exchange_ = shanghai_exchange;
	}

	double shenzhen_exchange() const{return shenzhen_exchange_;}

	void set_shenzhen_exchange(double shenzhen_exchange) {
		shenzhen_exchange_ = shenzhen_exchange;
	}

	double growth_exchange() const {return growth_exchange_;}

	void set_growth_exchange(double growth_exchange) {
		growth_exchange_ = growth_exchange;
	}

	double shanghai_exchange_;
	double shenzhen_exchange_;
	double growth_exchange_;
};

class CachedJsonInfo {
public:

	CachedJsonInfo() {
		market_limit_list_ = new base_logic::ListValue();
	}

	std::string industry_json(std::string type) {return industry_json_[type];}

	void set_industry_json(std::string type, std::string industry_json) {
		industry_json_[type] = industry_json;
	}

	std::string market_limit_json() const{return market_limit_json_;}

	void set_market_limit_json(std::string market_limit_json) {
		market_limit_json_ = market_limit_json;
	}

	std::string market_limit_jsonp() const{return market_limit_jsonp_;}

	void set_market_limit_jsonp(std::string market_limit_jsonp) {
		market_limit_jsonp_ = market_limit_jsonp;
	}

	std::string get_stocks_json(std::string type, std::string industry_name) {
		return stocks_per_industry_json_[type][industry_name];
	}

	void update_stocks_json(std::string type, std::string industry_name, std::string stocks_json) {
		stocks_per_industry_json_[type][industry_name] = stocks_json;
	}

	void clear() {
		industry_json_.clear();
		stocks_per_industry_json_.clear();
	}

	void clear_list() {
		if (NULL != market_limit_list_)
		    market_limit_list_->Clear();
	}

	void add_value_to_list(base_logic::Value* value) {
		market_limit_list_->Append(value);
	}

	std::map<std::string, std::string> industry_json_;
	std::string market_limit_json_;
	std::string market_limit_jsonp_;
	base_logic::ListValue* market_limit_list_;
	std::map<std::string, STOCKS_PER_INDUSTRY_JSON> stocks_per_industry_json_;
};

class StockUserCache {
public:
	std::string max_hist_data_time() {return max_hist_data_time_;}

	void set_max_hist_data_time(std::string max_hist_data_time) {
		max_hist_data_time_ = max_hist_data_time;
	}

	bool stock_data_inited() {return stock_data_inited_;}

	void set_stock_data_inited(bool stock_data_inited) {
		stock_data_inited_ = stock_data_inited;
	}

	STOCKINFO_MAP stock_total_info_;
	INDUSTRYINFO_MAP industry_info_;
	MARKET_LIMIT market_limit_info_;
	std::string week_first_day_;
	std::string month_first_day_;
	std::string max_hist_data_time_;
	bool stock_data_inited_;
	StockExchange stock_exchange_info_;
	CachedJsonInfo cached_json_info_;
};

class StockUserManager {
 public:
	StockUserManager();
	virtual ~StockUserManager();
 private:
	void Init();
 public:
	void Init(stock_logic::StockDB* vip_db);
 public:
	void UpdateRealtimeStockInfo();

	void UpdateStockHistData();

	bool UpdateStockDayKLineData();

	void UpdateIndustryPriceInfo();

	void UpdateIndustryVolume();

	void UpdateIndustryJson();

	void UpdateStockKLine();

	std::string GetStockKLineByCode(std::string stock_code, std::string format);

	void UpdateIndustryMarketValue();

	void GetTenIncreaseIndustry(std::map<std::string, double>& industry_map);

	void GetStockOfIndustry(std::string industry_name, std::map<std::string, double>& stock_info);

	void UpdateStockLimitInfo();

	void GetLimitData(std::string& market_limit_info_str, std::string format);

	void GetHotDiagramIndustryData(std::list<HotDiagramPerIndustry>& hot_diagram_industry_data);

	void GetHotDiagramDataByIndustry(std::list<HotDiagramPerStock>& hot_diagram_data_by_industry);

	void UpdateLimitData();

	void UpdateWeekMonthData();

	void WriteLimitData(int trade_time);

	void UpdateHotDiagram();

	void UpdateIndustryHotDiagram();

	void UpdateStockHotDiagram();

    std::string GetIndustryHotDiagram(std::string type, std::string format);

    std::string GetStocksHotDiagram(std::string type, std::string industry_name);

	struct threadrw_t*                 lock_;
	StockUserCache*                      stock_user_cache_;
	stock_logic::StockDB*                  stock_db_;
};

class StockUserEngine {
 private:
	static StockUserManager*    stock_user_mgr_;
	static StockUserEngine*     stock_user_engine_;
	StockUserEngine() {}
	virtual ~StockUserEngine() {}
 public:
	static StockUserManager* GetVIPUserManager() {
		if (stock_user_mgr_ == NULL)
			stock_user_mgr_ = new StockUserManager();
		return stock_user_mgr_;
	}

	static StockUserEngine* GetVIPUserEngine() {
		if (stock_user_engine_ == NULL)
			stock_user_engine_ = new StockUserEngine();
		return stock_user_engine_;
	}

	static void FreeVIPUserEngine() {
		delete stock_user_engine_;
	}

	static void FreeVIPUserManager() {
		delete stock_user_mgr_;
	}

};

}

#endif
