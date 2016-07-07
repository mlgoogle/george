//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月21日 Author: kerry

#include "basic/template.h"
#include "logic/logic_comm.h"
#include "net/packet_process.h"
#include "basic/basic_util.h"
#include <list>

#include "stock_factory.h"
#include "stock_proto_buf.h"


namespace stock_logic {

StockFactory*
StockFactory::instance_ = NULL;

StockFactory*
StockFactory::GetInstance() {
	if (instance_ == NULL)
		instance_ =new StockFactory();
	return instance_;
}

void StockFactory::FreeInstance() {
	delete instance_;
	instance_ = NULL;
}


StockFactory::StockFactory() {
	Init();
}

StockFactory::~StockFactory() {
}

void StockFactory::Init() {
	stock_usr_mgr_ = stock_logic::StockUserEngine::GetVIPUserManager();
}

void StockFactory::InitParam(config::FileConfig* config) {
	stock_db_ = new stock_logic::StockDB(config);
	stock_usr_mgr_->Init(stock_db_);
	OnUpdateStockHistData();
	OnUpdateYieldDataFromDB();
	std::map<int, YieldInfoUnit>& hs300_yields_info = stock_usr_mgr_->stock_user_cache_->get_hs300_yield_data();
	std::map<int, YieldInfoUnit>::iterator iter = hs300_yields_info.begin();
	for (; iter != hs300_yields_info.end(); iter++) {
		int trade_time = iter->first;
		stock_usr_mgr_->UpdateIndustryYieldInfo(trade_time);
	}
	TimeUpdateWeekMonthData();
	int current_trade_time = 0;
	stock_usr_mgr_->UpdateIndustryPriceInfo(current_trade_time);
	stock_usr_mgr_->UpdateIndustryJson();
}
void StockFactory::Dest() {
	stock_logic::StockUserEngine::FreeVIPUserEngine();
	stock_logic::StockUserEngine::FreeVIPUserManager();
}

void StockFactory::OnVIPGetLimitData(const int socket,
		base_logic::DictionaryValue* dict, george_logic::PacketHead* packet) {
	std::string limit_data_json;
	std::string format = "json";
	dict->GetString(L"format", &format);
	stock_usr_mgr_->GetLimitData(limit_data_json, format);
	LOG_MSG2("limit_data_json=%s", limit_data_json.c_str());
	if ("jsonp" == format) {
		std::string limit_data_jsonp = "";
		stock_logic::net_reply::VIPNewsList* vip_list = new stock_logic::net_reply::VIPNewsList();
		StockUtil::Instance()->filter_character(limit_data_json, '\r');
		StockUtil::Instance()->filter_character(limit_data_json, '\n');
		vip_list->set_string_value(L"jsonp_result", limit_data_json);
		StockUtil::Instance()->wrap_jsonp_serialize(vip_list, packet->attach_field(), limit_data_json, limit_data_jsonp);
        limit_data_json = limit_data_jsonp;
	StockUtil::Instance()->filter_character(limit_data_json, '\\');
	StockUtil::Instance()->filter_redundant_quotes(limit_data_json);
	}
	base_logic::LogicComm::SendFull(socket, limit_data_json.c_str(),limit_data_json.length());
}

void StockFactory::OnVIPGetHotDiagramData(const int socket,
		base_logic::DictionaryValue* dict, george_logic::PacketHead* packet) {

	std::string type = "";
	std::string stock_code = "";
	bool r = false;
	LOG_MSG2("dict.size=%d", dict->size());
	if (dict->GetString(L"stock_code", &stock_code)) {
		LOG_MSG2("stock_code=%s", stock_code.c_str());
	}
	else
		LOG_MSG("get stock_code failed");
	if(dict->GetString(L"type1", &type))
		LOG_MSG2("type=%s", type.c_str());
	else
		LOG_MSG("get type failed");
	std::string industry_name = "";
	r = dict->GetString(L"event", &industry_name);
	std::string encode_industry_name = "";
    if (true == r) {
	    base::BasicUtil::UrlDecode(industry_name, encode_industry_name);
	    LOG_MSG2("get industry=%s", encode_industry_name.c_str());
	    industry_name = encode_industry_name;
    }

    std::string cycle_type = "day";
    r = dict->GetString(L"cycle_type", &cycle_type);
    std::string format = "json";
    dict->GetString(L"format", &format);

    std::string classify = "industry";
	r = dict->GetString(L"classify", &classify);
	if (true == r) {
		LOG_MSG2("get classify=%s", classify.c_str());
	}

    std::string start_date = ",2016-06-01";
	r = dict->GetString(L"start_date", &start_date);
	if (true == r) {
		LOG_MSG2("get start_date=%s", start_date.c_str());
	}
	if (start_date.size() > 1)
				start_date.erase(start_date.begin());
    LOG_MSG2("cycle_type=%s,type=%s,format=%s", cycle_type.c_str(), type.c_str(), format.c_str());
	if ("industry" == type)
		//TODO 行业热力图
		ProcessHotDiagramIndustryData(socket, cycle_type, format, packet, dict);
	else if ("event" == type) {
		//事件排行榜
		ProcessHotDiagramEventData(socket, packet, dict);
	}
	else if ("" != industry_name) {
		//返回事件，行业和概念日收益率
		industry_name += classify;
        ProcessEventYieldByName(socket, start_date, industry_name, packet, cycle_type);
		//ProcessHotDiagramByIndustry(socket, cycle_type, industry_name);
	}
	else if ("" != stock_code) {
		if (stock_code.size() > 1)
			stock_code.erase(stock_code.begin());
        ProcessStockKLine(socket, stock_code, format, packet, dict, cycle_type, start_date);
	}
	LOG_MSG("OnVIPGetHotDiagramData end");
}

void StockFactory::ProcessEventYieldByName(int socket, std::string& start_date, std::string& industry_name,
		george_logic::PacketHead* packet, std::string& cycle_type) {
	LOG_MSG2("ProcessEventYieldByName start_date=%s,industry_name=%s", start_date.c_str(), industry_name.c_str());
    std::string yield_json = "";
    if (stock_usr_mgr_->GetYieldJsonByName(cycle_type, start_date, industry_name, yield_json)) {
    	base_logic::LogicComm::SendFull(socket, yield_json.c_str(), yield_json.length());
    }
    return;
}

void StockFactory::ProcessHotDiagramEventData(int socket,
		george_logic::PacketHead* packet, base_logic::DictionaryValue* dict) {
	std::string event_json = stock_usr_mgr_->GetEventHotDiagram();
	LOG_MSG2("event_json=%s", event_json.c_str());
	base_logic::LogicComm::SendFull(socket, event_json.c_str(),event_json.length());
}

//YGTODO 更改json字符串取值位置
void StockFactory::ProcessHotDiagramIndustryData(int socket, std::string type, std::string format,
		george_logic::PacketHead* packet, base_logic::DictionaryValue* dict) {
	std::string industry_json = stock_usr_mgr_->GetIndustryHotDiagram(type, format);
	LOG_MSG2("industry_json=%s",industry_json.c_str());
	if ("jsonp" == format) {
		std::string industry_jsonp = "";
		stock_logic::net_reply::VIPNewsList* vip_list = new stock_logic::net_reply::VIPNewsList();
		StockUtil::Instance()->filter_character(industry_json, '\r');
		StockUtil::Instance()->filter_character(industry_json, '\n');
		vip_list->set_string_value(L"jsonp_result", industry_json);
		StockUtil::Instance()->wrap_jsonp_serialize(vip_list, packet->attach_field(), industry_json, industry_jsonp);
		industry_json = industry_jsonp;
	LOG_MSG2("industry_json=%s",industry_json.c_str());
	StockUtil::Instance()->filter_character(industry_json, '\\');
	StockUtil::Instance()->filter_redundant_quotes(industry_json);
	}
	base_logic::LogicComm::SendFull(socket, industry_json.c_str(),industry_json.length());
}

void StockFactory::ProcessHotDiagramByIndustry(int socket, std::string type, std::string industry_name) {
    std::string stocks_json = stock_usr_mgr_->GetStocksHotDiagram(type, industry_name);
    LOG_MSG2("stocks_json=%s", stocks_json.c_str());
    base_logic::LogicComm::SendFull(socket, stocks_json.c_str(),stocks_json.length());
}

void StockFactory::ProcessStockKLine(int socket, std::string stock_code, std::string format,
		george_logic::PacketHead* packet, base_logic::DictionaryValue* dict,
		std::string& cycle_type, std::string& start_date) {
    std::string stock_kline = stock_usr_mgr_->GetStockKLineByCode(stock_code, format, cycle_type, start_date);
    LOG_MSG2("stock_kline=%s,stock_code=%s", stock_kline.c_str(),stock_code.c_str());
    if ("jsonp" == format) {
		std::string stock_kline_jsonp = "";
		stock_logic::net_reply::VIPNewsList* vip_list = new stock_logic::net_reply::VIPNewsList();
		StockUtil::Instance()->filter_character(stock_kline, '\r');
		StockUtil::Instance()->filter_character(stock_kline, '\n');
		vip_list->set_string_value(L"jsonp_result", stock_kline);
		StockUtil::Instance()->wrap_jsonp_serialize(vip_list, packet->attach_field(), stock_kline, stock_kline_jsonp);
		stock_kline = stock_kline_jsonp;
    StockUtil::Instance()->filter_character(stock_kline, '\\');
    StockUtil::Instance()->filter_redundant_quotes(stock_kline);
    }
    base_logic::LogicComm::SendFull(socket, stock_kline.c_str(),stock_kline.length());
}

void StockFactory::OnUpdateRealtimeStockInfo() {
	stock_usr_mgr_->UpdateRealtimeStockInfo();
	LOG_MSG("UpdateIndustryPriceInfo in OnUpdateRealtimeStockInfo");
	int current_trade_time = 0;
	stock_usr_mgr_->UpdateIndustryPriceInfo(current_trade_time);
	stock_usr_mgr_->UpdateIndustryVolume();
	stock_usr_mgr_->UpdateIndustryJson();
}

void StockFactory::OnUpdateStockHistData() {
	LOG_MSG("OnUpdateStockHistData");
	stock_usr_mgr_->UpdateStockHistData();
	stock_usr_mgr_->UpdateStockKLine();
	stock_usr_mgr_->UpdateIndustryHistData();
	//stock_usr_mgr_->UpdateIndustryKLine();TODO 缓存数据
}

void StockFactory::OnUpdateStockKLineData() {
	//如果有新的K线，则更新个股json
	LOG_MSG("OnUpdateStockKLineData");
	if (stock_usr_mgr_->UpdateStockDayKLineData())
		stock_usr_mgr_->UpdateStockKLine();
}

void StockFactory::OnUpdateEventsData() {
	stock_usr_mgr_->UpdateEventsData();
}

void StockFactory::OnUpdateYieldDataFromDB() {
	stock_usr_mgr_->UpdateYieldDataFromDB();
}

void StockFactory::OnUpdateYieldDataToDB() {
    stock_usr_mgr_->UpdateYieldDataToDB();
}

void StockFactory::OnDeleteOldYieldData() {
	stock_usr_mgr_->DeleteOldYieldData();
}

void StockFactory::TimeUpdateWeekMonthData() {
	LOG_MSG("TimeUpdateWeekMonthData");
	int current_trade_time = 0;
	stock_usr_mgr_->UpdateWeekMonthData();
	stock_usr_mgr_->UpdateIndustryPriceInfo(current_trade_time);
}

void StockFactory::TimeWriteLimitData(int current_trade_time){
	LOG_MSG("TimeWriteLimitData");
	stock_usr_mgr_->WriteLimitData(current_trade_time);
	stock_usr_mgr_->UpdateIndustryPriceInfo(current_trade_time);
	//stock_usr_mgr_->UpdateEventsYield(current_trade_time);
}

void StockFactory::TimeDeleteOldLimitData() {
	LOG_MSG("TimeDeleteOldLimitData");
	stock_usr_mgr_->DeleteOldEventsData();
}

void StockFactory::OnUpdateLimitData() {
	LOG_MSG("OnUpdateLimitData");
	stock_usr_mgr_->UpdateLimitData();
}

}

