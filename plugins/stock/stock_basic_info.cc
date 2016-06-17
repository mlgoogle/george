//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月17日 Author: kerry
#include "logic/logic_comm.h"
#include "stock_basic_info.h"
#include "stock_factory.h"

namespace stock_logic {

StockUtil* StockUtil::instance_ = NULL;

StockTotalInfo::StockTotalInfo() {

}

StockTotalInfo::~StockTotalInfo() {

}

void StockBasicInfo::ValueSerialization(base_logic::DictionaryValue* dict){
	dict->GetString(L"code", &code_);
	dict->GetString(L"name", &name_);
	dict->GetReal(L"totalAssets", &outstanding_);
	dict->GetReal(L"bvps", &bvps_);
	dict->GetReal(L"pb", &pb_);
	market_value_ = outstanding_ * bvps_ * pb_;
	if(code_ == "000001") {
		LOG_MSG2("code=%s,name=%s,outstanding=%f,bvps=%f,pb=%f,market_value=%f",
				code_.c_str(),name_.c_str(),outstanding_,bvps_,pb_,market_value_);
	}
}

std::string BasicIndustryInfo::get_stocks_json(std::string type) {
	stock_logic::net_reply::VIPNewsList* vip_list = new stock_logic::net_reply::VIPNewsList();
	std::list<HotDiagramPerStock>::iterator iter = hot_diagram_stocks_.begin();
	int count = 0;
	for (; iter != hot_diagram_stocks_.end(); iter++) {
		stock_logic::net_reply::STOIndustryData* industry_data = new  stock_logic::net_reply::STOIndustryData();
		industry_data->set_code(iter->code());
		industry_data->set_name(iter->name());
		if (type == "day") {
			industry_data->set_change_percent(iter->change_percent());
			industry_data->set_volume(iter->volume());
		}
		else if (type == "week") {
			industry_data->set_change_percent(iter->week_change_percent());
			industry_data->set_volume(iter->week_volume());
		}
		else if (type == "month") {
			industry_data->set_change_percent(iter->month_change_percent());
			industry_data->set_volume(iter->month_volume());
		}
		industry_data->set_volume(iter->volume());
		vip_list->set_vip_news(industry_data->get());
		count++;
		if (count > 9)
			break;
	}
	int32 increase_stock_num = 0;
	int32 decrease_stock_num = 0;
	std::map<std::string, double>::iterator stocks_iter = stock_price_info_.begin();
	for (; stocks_iter != stock_price_info_.end(); stocks_iter++) {
		double stock_price = stocks_iter->second;
		if (stock_price > 0.001)
			increase_stock_num++;
		else if (stock_price < -0.001)
			decrease_stock_num++;
	}
	//vip_list->set_double_value(L"increase", increase_stock_num);
	//vip_list->set_double_value(L"decrease", decrease_stock_num);
	std::string stocks_hot_diagram_json;
	StockUtil::Instance()->serialize(vip_list->packet(), stocks_hot_diagram_json);
	return stocks_hot_diagram_json;
}

void BasicIndustryInfo::update_hot_diagram_stock_json() {
	std::map<std::string,stock_logic::StockTotalInfo>& stocks_map =
			StockFactory::GetInstance()->stock_usr_mgr_->stock_user_cache_->stock_total_info_;
	std::map<std::string, double>::iterator iter = stock_price_info_.begin();
	hot_diagram_stocks_.clear();
	for (; iter != stock_price_info_.end(); iter++) {
		HotDiagramPerStock stock_hot_diagram;
		std::string code = iter->first;
		stock_logic::StockTotalInfo& stock_total_info = stocks_map[code];
		stock_logic::StockBasicInfo& basic_info = stock_total_info.basic_info_;
		std::string name = basic_info.name();
		stock_hot_diagram.set_code(code);
		stock_hot_diagram.set_name(name);
		stock_hot_diagram.set_change_percent(basic_info.change_percent());
		stock_hot_diagram.set_week_change_percent(basic_info.week_change_percent());
		stock_hot_diagram.set_month_change_percent(basic_info.month_change_percent());
		stock_hot_diagram.set_volume(basic_info.volume());
		stock_hot_diagram.set_week_volume(basic_info.week_volume());
		stock_hot_diagram.set_month_volume(basic_info.month_volume());
		hot_diagram_stocks_.push_back(stock_hot_diagram);
		if ("002045" == code || "002285" == code) {
			LOG_MSG2("code=%s,name=%s,change_percent=%f,volume=%f,week_volume=%f,month_volume=%f",
					code.c_str(),name.c_str(),stock_hot_diagram.change_percent(),stock_hot_diagram.volume(),
					stock_hot_diagram.week_volume(),stock_hot_diagram.month_volume());
		}
	}
	update_hot_diagram_stock_json_by_day();
	update_hot_diagram_stock_json_by_week();
	update_hot_diagram_stock_json_by_month();
}

std::string IndustryInfo::get_industry_json(std::string type, std::list<BasicIndustryInfo>& industry_list) {
	stock_logic::net_reply::VIPNewsList* vip_list = new stock_logic::net_reply::VIPNewsList();
	std::list<BasicIndustryInfo>::iterator iter = industry_list.begin();
	int count = 0;
	for (; iter != industry_list.end(); iter++) {
		stock_logic::net_reply::STOIndustryData* industry_data = new  stock_logic::net_reply::STOIndustryData();
		industry_data->set_name(iter->industry_name());
		if ("day" == type) {
			industry_data->set_change_percent(iter->industry_changepoint());
			industry_data->set_volume(iter->industry_volume());
		} else if ("week" == type) {
			industry_data->set_change_percent(iter->industry_week_changepoint());
			industry_data->set_volume(iter->week_industry_volume());
		} else if ("month" == type) {
			industry_data->set_change_percent(iter->industry_month_changepoint());
			industry_data->set_volume(iter->month_industry_volume());
		}
		vip_list->set_vip_news(industry_data->get());
		//industry_info_map_[iter->industry_name()].update_hot_diagram_stock_json();
		count++;
		if (count > 9)
			break;
	}
	set_dapan_info(vip_list);
	std::string stocks_hot_diagram_json;
	base_logic::DictionaryValue* dict = vip_list->packet();
	StockUtil::Instance()->serialize(dict, stocks_hot_diagram_json);
	delete vip_list;
	vip_list = NULL;
	delete dict;
	dict = NULL;
	return stocks_hot_diagram_json;
}

void IndustryInfo::set_dapan_info(stock_logic::net_reply::VIPNewsList* vip_list) {
	std::map<std::string,stock_logic::StockTotalInfo>& stocks_map =
					StockFactory::GetInstance()->stock_usr_mgr_->stock_user_cache_->stock_total_info_;
	stock_logic::StockTotalInfo& shangzhen = stocks_map["sh000001"];
	double shangzhen_index = shangzhen.basic_info_.current_trade();
	double shangzhen_changepercent = shangzhen.basic_info_.change_percent();
	stock_logic::net_reply::STOIndustryData* shangzhen_index_data = new  stock_logic::net_reply::STOIndustryData();
	shangzhen_index_data->set_code("上证指数");
	shangzhen_index_data->set_name("sh000001");
	shangzhen_index_data->set_current_trade(shangzhen_index);
	shangzhen_index_data->set_change_percent(shangzhen_changepercent);
	vip_list->set_vip_news(shangzhen_index_data->get());

	stock_logic::StockTotalInfo& shenzhen = stocks_map["sh399001"];
	double shenzhen_index = shenzhen.basic_info_.current_trade();
	double shenzhen_changepercent = shenzhen.basic_info_.change_percent();
	stock_logic::net_reply::STOIndustryData* shenzhen_index_data = new  stock_logic::net_reply::STOIndustryData();
	shenzhen_index_data->set_code("深证指数");
	shenzhen_index_data->set_name("sh399001");
	shenzhen_index_data->set_current_trade(shenzhen_index);
	shenzhen_index_data->set_change_percent(shenzhen_changepercent);
	vip_list->set_vip_news(shenzhen_index_data->get());

	stock_logic::StockTotalInfo& chuanye = stocks_map["sh399006"];
	double chuanye_index = chuanye.basic_info_.current_trade();
	double chuanye_changepercent = chuanye.basic_info_.change_percent();
	stock_logic::net_reply::STOIndustryData* chuanye_index_data = new  stock_logic::net_reply::STOIndustryData();
	chuanye_index_data->set_code("创业板指");
	chuanye_index_data->set_name("sh399006");
	chuanye_index_data->set_current_trade(chuanye_index);
	chuanye_index_data->set_change_percent(chuanye_changepercent);
	vip_list->set_vip_news(chuanye_index_data->get());

}

void StockBasicInfo::RealtimeValueSerialization(base_logic::DictionaryValue* dict){
	int trade_time;
	dict->GetInteger(L"time", &trade_time);
	current_trade_time_ = (int)time(NULL);
	/*if (current_trade_time_ > trade_time)
		return;
	else
		current_trade_time_ = trade_time;*/
	dict->GetReal(L"changepercent", &change_percent_);
	dict->GetReal(L"trade", &current_trade_);
	dict->GetReal(L"open", &current_open_);
	dict->GetReal(L"high", &current_high_);
	dict->GetReal(L"low", &current_low_);
	dict->GetReal(L"settlement", &current_settlement_);
	dict->GetReal(L"volume", &volume_);

	if(code_ == "000001" || code_ == "601002") {
		LOG_MSG2("code=%s,changepercent=%f,current_trade_=%f,"
				"current_open_=%f,current_high_=%f,current_low_=%f,current_settlement_=%f,time=%d,volume=%f",
				code_.c_str(),change_percent_,current_trade_,
				current_open_,current_high_,current_low_,current_settlement_,current_trade_time_,volume_);
	}
}

void StockBasicInfo::UpdateTodayKLine() {
	std::map<std::string,stock_logic::StockTotalInfo>& stocks_map =
						StockFactory::GetInstance()->stock_usr_mgr_->stock_user_cache_->stock_total_info_;
	stock_logic::StockTotalInfo& total_info = stocks_map[code_];
	stock_logic::StockHistDataInfo& hist_data_info = total_info.hist_data_info_;
	std::string current_day = StockUtil::Instance()->get_current_day_str();
	hist_data_info.add_hist_data(current_day,current_open_,current_high_,current_trade_,current_low_);
	if (code_ == "000001")
		LOG_MSG2("UpdateTodayKLine,current_day=%s,current_open=%f,current_high_=%f,current_trade_=%f,current_low_=%f",
				current_day.c_str(),current_open_,current_high_,current_trade_,current_low_);
}

void StockBasicInfo::MonthWeekDataValueSerialization(base_logic::DictionaryValue* dict) {
	std::string type = "";
	dict->GetString(L"type", &type);
	std::string date = "";
	dict->GetString(L"date", &date);
	if ("W" == type) {
		if (date < week_trade_day_)
			return;
		dict->GetReal(L"p_change", &week_change_percent_);
		dict->GetString(L"date", &week_trade_day_);
		dict->GetReal(L"volume", &week_volume_);
		if (code_ == "000001" || code_ == "601002" || code_ == "002285") {
			LOG_MSG2("code=%s,type=%s,week_change_percent=%f,week_trade_day=%s,week_volume=%f",
					code_.c_str(),type.c_str(),week_change_percent_,week_trade_day_.c_str(),week_volume_);
		}
	} else if ("M" == type) {
		if (date < month_trade_day_)
			return;
		dict->GetReal(L"p_change", &month_change_percent_);
		dict->GetString(L"date", &month_trade_day_);
		dict->GetReal(L"volume", &month_volume_);
		if (code_ == "000001" || code_ == "601002" || code_ == "002285") {
			LOG_MSG2("code=%s,type=%s,month_change_percent=%f,month_trade_day=%s,month_volume=%f",
					code_.c_str(),type.c_str(),month_change_percent_,month_trade_day_.c_str(),month_volume_);
		}
	}
}

void StockHistDataInfo::add_hist_data(std::string date, double open, double high, double close, double low) {
	HistDataPerDay hist_data_per_day;
	hist_data_per_day.open_ = open;
	hist_data_per_day.high_ = high;
	hist_data_per_day.close_ = close;
	hist_data_per_day.low_ = low;
	hist_data_per_day.mid_price_ = (high + low + close)/3;
	hist_data_per_day.date_ = date;
	stock_hist_data_[date] = hist_data_per_day;
	StockUserCache* cache = StockFactory::GetInstance()->stock_usr_mgr_->stock_user_cache_;
	if (date > cache->max_hist_data_time()) {
		cache->set_max_hist_data_time(date);
	}
}

void StockHistDataInfo::ValueSerialization(base_logic::DictionaryValue* dict){
	std::string date;
	double open, high, close, low;
	dict->GetString(L"date", &date);
	dict->GetReal(L"open", &open);
	dict->GetReal(L"high", &high);
	dict->GetReal(L"close", &close);
	dict->GetReal(L"low", &low);
	add_hist_data(date,open,high,close,low);
}

void StockHistDataInfo::CountSupportPressurePos() {
	std::map<std::string, HistDataPerDay>::iterator iter = stock_hist_data_.begin();
	std::vector<HistDataPerDay> hist_data_vec;
	for (; iter != stock_hist_data_.end(); iter++) {
		hist_data_vec.push_back(iter->second);
	}
	int kline_num = hist_data_vec.size();
	if (kline_num < 12)
		return;
	for (int i = 11; i < hist_data_vec.size(); i++) {
		double high_in_12_days = 0.0;
		double low_in_12_days = 100000.0;
		for (int j = i; j > -1; j--) {
			double high = hist_data_vec[j].high_;
			double low = hist_data_vec[j].low_;
			if (high > high_in_12_days)
				high_in_12_days = high;
			if (low < low_in_12_days)
				low_in_12_days = low;
		}
		double support_pos = hist_data_vec[i].mid_price_ - (high_in_12_days - low_in_12_days);
		double pressure_pos = hist_data_vec[i].mid_price_ + (high_in_12_days - low_in_12_days);
		std::string date = hist_data_vec[i].date_;
		stock_hist_data_[date].support_pos_ = support_pos;
		stock_hist_data_[date].pressure_pos_ = pressure_pos;
		//if ("")
		//LOG_MSG2("")
	}

}

void IndustryInfo::ValueSerialization(base_logic::DictionaryValue* dict){
	std::string code,industry_name;
	dict->GetString(L"symbol", &code);
	dict->GetString(L"keyname", &industry_name);
    industry_info_map_[industry_name].add_price_info(code,0);
    industry_info_map_[industry_name].set_industry_name(industry_name);
}

void StockTotalInfo::update_kline_json() {
	KLine_json_ = "";
	std::map<std::string, HistDataPerDay>& hist_data = hist_data_info_.stock_hist_data_;
	stock_logic::net_reply::VIPNewsList* vip_list = new stock_logic::net_reply::VIPNewsList();
	int count = 0;
	std::map<std::string, HistDataPerDay>::reverse_iterator iter = hist_data.rbegin();
	for (; iter != hist_data.rend(); iter++) {
		stock_logic::net_reply::STOIndustryData* kline_data = new  stock_logic::net_reply::STOIndustryData();
		HistDataPerDay& hist_data = iter->second;
		kline_data->setDate(iter->first);
		kline_data->setOpen(hist_data.open_);
		kline_data->setHigh(hist_data.high_);
		kline_data->setLow(hist_data.low_);
		kline_data->setClose(hist_data.close_);
		kline_data->setSupport(hist_data.support_pos_);
		kline_data->setPressure(hist_data.pressure_pos_);
		vip_list->set_vip_news(kline_data->get());
		delete kline_data;
		kline_data = NULL;
		if ("000001" == basic_info_.code()) {
			LOG_MSG2("code=%s,date=%s,open=%f,high=%f,low=%f,close=%f,support_pos=%f,pressure_pos=%f",
					basic_info_.code().c_str(),iter->first.c_str(),hist_data.open_,hist_data.high_,hist_data.low_,
					hist_data.close_,hist_data.support_pos_,hist_data.pressure_pos_);
		}
		count++;
		if (count > 29)
			break;
	}
	base_logic::DictionaryValue* kline_value = vip_list->packet();
	StockUtil::Instance()->serialize(kline_value, KLine_json_);
	delete kline_value;
	kline_value = NULL;
	delete vip_list;
	vip_list = NULL;
	if ("000001" == basic_info_.code()) {
		LOG_MSG2("code=%s,KLine_json=%s", basic_info_.code().c_str(), KLine_json_.c_str());
	}
}

}
