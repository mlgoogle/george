//  Copyright (c) 2016-2016 The george Authors. All rights reserved.
//  Created on: 2016年5月21日 Author: kerry
#ifndef GEORGE_VIP_FACTORY_H__
#define GEORGE_VIP_FACTORY_H__

#include "basic/basictypes.h"
#include "basic/scoped_ptr.h"
#include "config/config.h"
#include "net/packet_process.h"
#include "stock_basic_info.h"
#include "stock_db.h"
#include "stock_proto_buf.h"
#include "stock_user_mgr.h"
#include "stock_user_mgr.h"

namespace stock_logic {

class StockDB;

class StockFactory {
 public:
	StockFactory();
	virtual ~StockFactory();
 public:
	void Init();
	void Dest();
 private:
	static StockFactory*  instance_;
 public:
	static StockFactory* GetInstance();

	static void FreeInstance();

	void InitParam(config::FileConfig* config);
 public:

	void OnVIPGetLimitData(const int socket,
			base_logic::DictionaryValue* dict);

	void OnVIPGetHotDiagramData(const int socket,
			base_logic::DictionaryValue* dict);

	void OnUpdateRealtimeStockInfo();

	void OnUpdateStockHistData();

	void OnUpdateStockKLineData();

	void OnUpdateLimitData();

	void ProcessHotDiagramIndustryData(int socket, std::string type);

	void ProcessHotDiagramByIndustry(int socket, std::string type, std::string industry_name);

	void ProcessStockKLine(int socket, std::string stock_code);

	void TimeWriteLimitData(int current_trade_time);

	void TimeDeleteOldLimitData();

	void TimeUpdateWeekMonthData();

	StockUserManager*                                     stock_usr_mgr_;
	stock_logic::StockDB*                                 stock_db_;
};
}
#endif
