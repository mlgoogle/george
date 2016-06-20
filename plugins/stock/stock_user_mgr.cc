#include "logic/logic_comm.h"
#include "basic/template.h"
#include "stock_user_mgr.h"

#include "stock_proto_buf.h"
#include "thread/base_thread_lock.h"

namespace stock_logic {


StockUserManager* StockUserEngine::stock_user_mgr_ = NULL;
StockUserEngine* StockUserEngine::stock_user_engine_ = NULL;


StockUserManager::StockUserManager()
:stock_db_(NULL){
	stock_user_cache_ = new StockUserCache();
	Init();
}

StockUserManager::~StockUserManager() {
	DeinitThreadrw(lock_);
}

void StockUserManager::Init() {
	InitThreadrw(&lock_);
}

void StockUserManager::Init(stock_logic::StockDB* vip_db) {
	stock_db_ = vip_db;
	stock_db_->FectchStockBasicInfo(stock_user_cache_->stock_total_info_);
	for(STOCKINFO_MAP::iterator it = stock_user_cache_->stock_total_info_.begin();
			it != stock_user_cache_->stock_total_info_.end();it++) {
		stock_db_->FectchStockHistData(it->first, stock_user_cache_->stock_total_info_);
	}
	stock_db_->FectchIndustryInfo(stock_user_cache_->industry_info_);
	for(std::map<std::string, BasicIndustryInfo>::iterator it = stock_user_cache_->industry_info_.industry_info_map_.begin();
			it != stock_user_cache_->industry_info_.industry_info_map_.begin(); it++) {
		std::string industry_name = it->first;
		BasicIndustryInfo& basic_industry_info = it->second;
        for (std::map<std::string, double>::iterator iter = basic_industry_info.stock_price_info_.begin();
        		iter != basic_industry_info.stock_price_info_.end(); iter++) {
        	stock_user_cache_->stock_total_info_[iter->first].add_stock_industry(it->first);
        }
	}

	UpdateIndustryMarketValue();
	UpdateStockKLine();
}

void StockUserManager::UpdateStockHistData() {
	base_logic::WLockGd lk(lock_);
	for(STOCKINFO_MAP::iterator it = stock_user_cache_->stock_total_info_.begin();
				it != stock_user_cache_->stock_total_info_.end();it++) {
		stock_db_->FectchStockHistData(it->first, stock_user_cache_->stock_total_info_);
		it->second.hist_data_info_.CountSupportPressurePos();
	}
	stock_user_cache_->set_stock_data_inited(true);
}

bool StockUserManager::UpdateStockDayKLineData() {
	base_logic::WLockGd lk(lock_);
	std::string latest_time = stock_user_cache_->max_hist_data_time();
	return stock_db_->FectchStockDayKLineData(latest_time, stock_user_cache_->stock_total_info_);
}

void StockUserManager::UpdateRealtimeStockInfo() {
	base_logic::WLockGd lk(lock_);
	LOG_MSG("UpdateRealtimeStockInfo");
	//if (StockUtil::Instance()->is_trading_time())
    stock_db_->UpdateRealtimeStockInfo(stock_user_cache_->stock_total_info_);
}

void StockUserManager::UpdateWeekMonthData() {
	base_logic::WLockGd lk(lock_);
	stock_db_->UpdateWeekMonthData(stock_user_cache_->stock_total_info_);
}

void StockUserManager::GetLimitData(std::string& market_limit_info_str, std::string format) {
	base_logic::WLockGd lk(lock_);
	if ("jsonp" == format)
		market_limit_info_str = stock_user_cache_->cached_json_info_.market_limit_jsonp();
	else
		market_limit_info_str = stock_user_cache_->cached_json_info_.market_limit_json();
}

void StockUserManager::GetHotDiagramIndustryData(std::list<HotDiagramPerIndustry>& hot_diagram_industry_data) {
	base_logic::WLockGd lk(lock_);

}

void StockUserManager::GetHotDiagramDataByIndustry(std::list<HotDiagramPerStock>& hot_diagram_data_by_industry) {
	base_logic::WLockGd lk(lock_);
}

void StockUserManager::UpdateLimitData() {
	base_logic::WLockGd lk(lock_);
	stock_user_cache_->market_limit_info_.clear();
	//stock_user_cache_->cached_json_info_.clear_list();
	stock_db_->GetLimitData(stock_user_cache_->market_limit_info_);
	MARKET_LIMIT& market_limit_info = stock_user_cache_->market_limit_info_;
	stock_logic::net_reply::VIPNewsList* vip_list = new stock_logic::net_reply::VIPNewsList();
	MARKET_LIMIT::iterator iter = market_limit_info.begin();
	for (; iter != market_limit_info.end(); iter++) {
		stock_logic::net_reply::VIPLimitData* limit_data = new  stock_logic::net_reply::VIPLimitData();
		limit_data->set_time(iter->first);
		limit_data->set_surged_stock_num(iter->second[0]);
		limit_data->set_decline_stock_num(iter->second[1]);
		base_logic::Value* limit_data_value = limit_data->get();
		vip_list->set_vip_news(limit_data_value);
		//stock_user_cache_->cached_json_info_.add_value_to_list(limit_data_value);
		delete limit_data;
		limit_data = NULL;
	}
    base_logic::DictionaryValue* vip_list_packet = vip_list->packet();
    stock_user_cache_->cached_json_info_.market_limit_json_ = "";
    StockUtil::Instance()->serialize(vip_list_packet, stock_user_cache_->cached_json_info_.market_limit_json_);
    stock_user_cache_->cached_json_info_.set_market_limit_jsonp("");
    StockUtil::Instance()->jsonp_serialize(vip_list_packet, stock_user_cache_->cached_json_info_.market_limit_jsonp_);
    delete vip_list_packet;
    vip_list_packet = NULL;
    delete vip_list;
    vip_list = NULL;
	LOG_MSG2("packet finished market_limit_info.size=%d", market_limit_info.size());
}

void StockUserManager::WriteLimitData(int trade_time) {
	if (!StockUtil::Instance()->is_trading_time())
		return;
	int current_trade_time;
	LOG_MSG2("time write limit data trade_time=%d", trade_time);
	base_logic::WLockGd lk(lock_);
	STOCKINFO_MAP& stock_total_info = stock_user_cache_->stock_total_info_;
	STOCKINFO_MAP::iterator iter = stock_total_info.begin();
	int surged_count = 0;
	int decline_count = 0;
	for (; iter != stock_total_info.end(); iter++) {
		std::string code = iter->first;
		StockBasicInfo& basic_info = iter->second.basic_info_;
		current_trade_time = basic_info.current_trade_time();
		basic_info.check_limit();
		if (basic_info.is_decline_limit()) {
			//LOG_MSG2("decline code=%s,current_trade=%f,surged_limit_price=%f,decline_limit_price=%f",
				//	basic_info.code_.c_str(),basic_info.current_trade_,basic_info.surged_limit_price_,basic_info.decline_limit_price_);
            decline_count += 1;
		}else if (basic_info.is_surged_limit()) {
			//LOG_MSG2("surged code=%s,current_trade=%f,surged_limit_price=%f,decline_limit_price=%f",
				//	basic_info.code_.c_str(),basic_info.current_trade_,basic_info.surged_limit_price_,basic_info.decline_limit_price_);
			surged_count += 1;
		}
	}
	//LOG_MSG2("current_trade_time=%dsurged_count=%d,decline_count=%d", current_trade_time, surged_count, decline_count);
	if (current_trade_time == 0)
		return;
	stock_db_->WriteLimitData(current_trade_time, surged_count, decline_count);
}

void StockUserManager::UpdateStockHotDiagram() {
	base_logic::WLockGd lk(lock_);
	/*CachedJsonInfo& cached_json = vip_user_cache_->cached_json_info_;
	cached_json.clear();
	std::map<std::string, STOCKS_PER_INDUSTRY_JSON>& stocks_map = cached_json.stocks_per_industry_json_;
	std::map<std::string, std::string>& industry_map = cached_json.industry_json_;
	vip_user_cache_->industry_info_.get_day_hottest_industry(industry_map);
	std::list<BasicIndustryInfo> industry_list;
	for ()

	vip_logic::net_reply::VIPNewsList* vip_list = new vip_logic::net_reply::VIPNewsList();
		MARKET_LIMIT::iterator iter = vip_user_cache_->cached_json_info_.stocks_per_industry_json_.begin();
		for (; iter != vip_user_cache_->cached_json_info_.stocks_per_industry_json_.end(); iter++) {
			STOCKS_PER_INDUSTRY_JSON& stocks_json_map = iter->second;
			STOCKS_PER_INDUSTRY_JSON::iterator it = stocks_json_map.begin();
			for ()
			vip_logic::net_reply::STOIndustryData* stock_data = new  vip_logic::net_reply::STOIndustryData();
			stock_data->set_code(iter->)
			limit_data->set_time(iter->first);
			limit_data->set_surged_stock_num(iter->second[0]);
			limit_data->set_decline_stock_num(iter->second[1]);
			vip_list->set_vip_news(limit_data->get());
		}

	std::string stocks_hot_diagram_json;
	StockUtil::Instance()->serialize(vip_list->packet(), stocks_hot_diagram_json);*/
}

std::string StockUserManager::GetIndustryHotDiagram(std::string type, std::string format) {
	base_logic::RLockGd lk(lock_);
	//std::string industry_json = vip_user_cache_->cached_json_info_.industry_json(type);
	std::string industry_json = stock_user_cache_->industry_info_.get_industry_hot_diagram_by_type(type, format);
	return industry_json;
}

std::string StockUserManager::GetStocksHotDiagram(std::string type, std::string industry_name) {
	base_logic::RLockGd lk(lock_);
	//std::string stocks_json = vip_user_cache_->cached_json_info_.get_stocks_json(type, industry_name);
	BasicIndustryInfo& industry_info = stock_user_cache_->industry_info_.get_industry_by_name(industry_name);
	std::string stocks_json = industry_info.get_hot_diagram_by_type(type);
	return stocks_json;
}

void StockUserManager::UpdateIndustryPriceInfo() {
	LOG_MSG("UpdateIndustryPriceInfo");
	base_logic::WLockGd lk(lock_);
	std::string first_day_of_week = StockUtil::Instance()->get_first_day_of_week();
	std::string first_day_of_month = StockUtil::Instance()->get_first_day_of_month();
	std::map<std::string, BasicIndustryInfo>& industry_map = stock_user_cache_->industry_info_.industry_info_map_;
	std::map<std::string, BasicIndustryInfo>::iterator iter = industry_map.begin();
	for (; iter != industry_map.end(); iter++) {
		std::string industry_name = iter->first;
		double industry_changepoint = 0;
		double week_industry_changepoint = 0;
		double month_industry_changepoint = 0;
		BasicIndustryInfo& basic_industry_info = iter->second;
		std::map<std::string, double>::iterator it = basic_industry_info.stock_price_info_.begin();
		for (; it != basic_industry_info.stock_price_info_.end(); it++) {
			std::string code = it->first;
			StockTotalInfo& stock_total_info = stock_user_cache_->stock_total_info_[code];
			StockBasicInfo& stock_basic_info = stock_total_info.basic_info_;
            //if(first_day_of_week > stock_basic_info.week_trade_day())
            //	continue;
			double stock_market_value = stock_basic_info.market_value();
			industry_changepoint += stock_basic_info.change_percent() * stock_market_value/basic_industry_info.industry_market_value();
			/*if ("酿酒食品" == industry_name) {
				/*LOG_MSG2("industry_name=%s,code=%s,industry_changepint=%f,changepoint=%f,market_value=%f,total_market_value=%f",
						industry_name.c_str(),code.c_str(),industry_changepoint,stock_basic_info.change_percent()
						,stock_market_value,basic_industry_info.industry_market_value());
			}*/
			week_industry_changepoint += stock_basic_info.week_change_percent() *
					stock_market_value/basic_industry_info.industry_market_value();
			month_industry_changepoint += stock_basic_info.month_change_percent() *
					stock_market_value/basic_industry_info.industry_market_value();
			/*LOG_MSG2("code=%s,market_value=%f,industry_market_value=%f,industry_changepoint=%f,week=%f,month=%f",
					code.c_str(),stock_market_value,basic_industry_info.industry_market_value(),industry_changepoint,
					week_industry_changepoint,month_industry_changepoint);*/
		}
		basic_industry_info.set_industry_changepoint(industry_changepoint);
		basic_industry_info.set_industry_week_changepoint(week_industry_changepoint);
		basic_industry_info.set_industry_month_changepoint(month_industry_changepoint);
		/*LOG_MSG2("total industry_name=%s,industry_changepoint=%f,week=%f,month=%f",
				industry_name.c_str(), industry_changepoint,
				week_industry_changepoint,month_industry_changepoint);*/
	}
}

void StockUserManager::UpdateIndustryVolume() {
	LOG_MSG("UpdateIndustryVolume");
	base_logic::WLockGd lk(lock_);
	std::string first_day_of_week = StockUtil::Instance()->get_first_day_of_week();
	std::string first_day_of_month = StockUtil::Instance()->get_first_day_of_month();
	std::map<std::string, BasicIndustryInfo>& industry_map = stock_user_cache_->industry_info_.industry_info_map_;
	std::map<std::string, BasicIndustryInfo>::iterator iter = industry_map.begin();
	for (; iter != industry_map.end(); iter++) {
		std::string industry_name = iter->first;
		double industry_volume = 0;
		double week_industry_volume = 0;
		double month_industry_volume = 0;
		BasicIndustryInfo& basic_industry_info = iter->second;
		std::map<std::string, double>::iterator it = basic_industry_info.stock_price_info_.begin();
		for (; it != basic_industry_info.stock_price_info_.end(); it++) {
			std::string code = it->first;

			StockTotalInfo& stock_total_info = stock_user_cache_->stock_total_info_[code];
			StockBasicInfo& stock_basic_info = stock_total_info.basic_info_;
			//if(first_day_of_week > stock_basic_info.week_trade_day())
			//	continue;
			double stock_market_value = stock_basic_info.market_value();
			industry_volume += stock_basic_info.volume();
			week_industry_volume += stock_basic_info.week_volume();
			month_industry_volume += stock_basic_info.month_volume();
			/*if (code == "601002") {
				LOG_MSG2("code=%s,volume=%f,week_volume=%f,month_volume=%f",
						code.c_str(),stock_basic_info.volume(),stock_basic_info.week_volume(),stock_basic_info.month_volume());
			}
			/*LOG_MSG2("code=%s,market_value=%f,industry_market_value=%f,industry_changepoint=%f,week=%f,month=%f",
					code.c_str(),stock_market_value,basic_industry_info.industry_market_value(),industry_changepoint,
					week_industry_changepoint,month_industry_changepoint);*/
		}
		basic_industry_info.set_industry_volume(industry_volume);
		basic_industry_info.set_week_industry_volume(week_industry_volume);
		basic_industry_info.set_month_industry_volume(month_industry_volume);
		/*LOG_MSG2("total industry_name=%s,industry_changepoint=%f,week=%f,month=%f",
				industry_name.c_str(), industry_changepoint,
				week_industry_changepoint,month_industry_changepoint);*/
	}
}

void StockUserManager::UpdateIndustryJson() {
	base_logic::WLockGd lk(lock_);
	stock_user_cache_->industry_info_.update_hottest_industry();
}

void StockUserManager::UpdateStockKLine() {
	LOG_MSG("UpdateStockKLine");
	base_logic::WLockGd lk(lock_);
	STOCKINFO_MAP& stocks_map = stock_user_cache_->stock_total_info_;
	STOCKINFO_MAP::iterator stocks_iter = stocks_map.begin();
	for (; stocks_iter != stocks_map.end(); stocks_iter++) {
		if ("000001" == stocks_iter->first) {
			LOG_MSG2("code=%s", stocks_iter->first.c_str());
		}
		stocks_iter->second.update_kline_json();
	}
}

std::string StockUserManager::GetStockKLineByCode(std::string stock_code, std::string format) {
	base_logic::WLockGd lk(lock_);
	STOCKINFO_MAP& stocks_map = stock_user_cache_->stock_total_info_;
	STOCKINFO_MAP::iterator iter = stocks_map.find(stock_code);
	if (stocks_map.end() != iter) {
		if ("jsonp" == format)
			return iter->second.getKLineJsonp();
		else
		    return iter->second.getLineJson();
	} else
		return "null";
}

void StockUserManager::UpdateIndustryMarketValue() {
	base_logic::WLockGd lk(lock_);
	std::map<std::string, BasicIndustryInfo>& industry_map = stock_user_cache_->industry_info_.industry_info_map_;
	std::map<std::string, BasicIndustryInfo>::iterator iter = industry_map.begin();
	for (; iter != industry_map.end(); iter++) {
		BasicIndustryInfo& basic_industry_info = iter->second;
		double industry_market_value = 0.0;
		std::map<std::string, double>::iterator it = basic_industry_info.stock_price_info_.begin();
		for (; it != basic_industry_info.stock_price_info_.end(); it++) {
			std::string code = it->first;
			StockTotalInfo& stock_total_info = stock_user_cache_->stock_total_info_[code];
			StockBasicInfo& stock_basic_info = stock_total_info.basic_info_;
			double stock_market_value = stock_basic_info.market_value();
			/*if ("酿酒食品" == iter->first) {
				LOG_MSG2("industry_name=%s,code=%s,market_value=%f,industry_market_value=%f",
						iter->first.c_str(),code.c_str(),stock_market_value,industry_market_value);
			}*/
			industry_market_value += stock_market_value;
		}
		basic_industry_info.set_industry_market_value(industry_market_value);
	}
}

}
