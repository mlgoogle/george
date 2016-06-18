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
}
void StockFactory::Dest() {
	stock_logic::StockUserEngine::FreeVIPUserEngine();
	stock_logic::StockUserEngine::FreeVIPUserManager();
}

void StockFactory::OnVIPGetLimitData(const int socket,
		base_logic::DictionaryValue* dict) {
	std::string limit_data_json;
	stock_usr_mgr_->GetLimitData(limit_data_json);
	LOG_MSG2("limit_data_json=%s", limit_data_json.c_str());
	base_logic::LogicComm::SendFull(socket, limit_data_json.c_str(),limit_data_json.length());
}

void StockFactory::OnVIPGetHotDiagramData(const int socket,
		base_logic::DictionaryValue* dict) {

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
	/*std::string industry_name = "";
	bool r = dict->GetString(L"industry", &industry_name);
	std::string out_hy;
    if (true == r) {
	    base::BasicUtil::UrlDecode(industry_name, out_hy);
	    LOG_MSG2("get industry=%s", out_hy.c_str());
	    industry_name = out_hy;
    }*/

    std::string cycle_type = "day";
    r = dict->GetString(L"cycle_type", &cycle_type);
    LOG_MSG2("cycle_type=%s,type=%s", cycle_type.c_str(),type.c_str());
	if ("industry" == type)
		ProcessHotDiagramIndustryData(socket, cycle_type);
	/*else if ("" != industry_name) {
		ProcessHotDiagramByIndustry(socket, cycle_type, industry_name);
	} */
	else if ("" != stock_code) {
		if (stock_code.size() > 1)
			stock_code.erase(stock_code.begin());
        ProcessStockKLine(socket, stock_code);
	}
}

//YGTODO 更改json字符串取值位置
void StockFactory::ProcessHotDiagramIndustryData(int socket, std::string type) {
	std::string industry_json = stock_usr_mgr_->GetIndustryHotDiagram(type);
	LOG_MSG2("industry_json=%s",industry_json.c_str());
	base_logic::LogicComm::SendFull(socket, industry_json.c_str(),industry_json.length());
}

void StockFactory::ProcessHotDiagramByIndustry(int socket, std::string type, std::string industry_name) {
    std::string stocks_json = stock_usr_mgr_->GetStocksHotDiagram(type, industry_name);
    LOG_MSG2("stocks_json=%s", stocks_json.c_str());
    base_logic::LogicComm::SendFull(socket, stocks_json.c_str(),stocks_json.length());
}

void StockFactory::ProcessStockKLine(int socket, std::string stock_code) {
    std::string stock_kline = stock_usr_mgr_->GetStockKLineByCode(stock_code);
    LOG_MSG2("stock_kline=%s,stock_code=%s", stock_kline.c_str(),stock_code.c_str());
    base_logic::LogicComm::SendFull(socket, stock_kline.c_str(),stock_kline.length());
}

void StockFactory::OnUpdateRealtimeStockInfo() {
	stock_usr_mgr_->UpdateRealtimeStockInfo();
	LOG_MSG("UpdateIndustryPriceInfo in OnUpdateRealtimeStockInfo");
	stock_usr_mgr_->UpdateIndustryPriceInfo();
	stock_usr_mgr_->UpdateIndustryVolume();
	stock_usr_mgr_->UpdateIndustryJson();
}

void StockFactory::OnUpdateStockHistData() {
	stock_usr_mgr_->UpdateStockHistData();
	stock_usr_mgr_->UpdateStockKLine();
}

void StockFactory::OnUpdateStockKLineData() {
	//如果有新的K线，则更新个股json
	if (stock_usr_mgr_->UpdateStockDayKLineData())
		stock_usr_mgr_->UpdateStockKLine();
}

void StockFactory::TimeUpdateWeekMonthData() {
	stock_usr_mgr_->UpdateWeekMonthData();
	stock_usr_mgr_->UpdateIndustryPriceInfo();
}

void StockFactory::TimeWriteLimitData(int current_trade_time){
	stock_usr_mgr_->WriteLimitData(current_trade_time);
}

void StockFactory::TimeDeleteOldLimitData() {
}

void StockFactory::OnUpdateLimitData() {
	stock_usr_mgr_->UpdateLimitData();
}

}

