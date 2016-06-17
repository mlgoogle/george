//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月15日 Author: kerry


#ifndef GEORGE_VIP_BASIC_INFO_H_
#define GEORGE_VIP_BASIC_INFO_H_


#include "basic/basictypes.h"
#include "logic/base_values.h"
#include "logic/logic_comm.h"
#include <math.h>
#include <list>
#include <set>
#include <string>
#include "stock_proto_buf.h"

namespace stock_logic {

class StockUtil {
public:

	StockUtil() {
        holiday_in_2016_.insert("2016-06-09");
        holiday_in_2016_.insert("2016-06-10");
        holiday_in_2016_.insert("2016-09-15");
        holiday_in_2016_.insert("2016-09-16");
        holiday_in_2016_.insert("2016-10-01");
		holiday_in_2016_.insert("2016-10-02");
		holiday_in_2016_.insert("2016-10-03");
		holiday_in_2016_.insert("2016-10-04");
		holiday_in_2016_.insert("2016-10-05");
		holiday_in_2016_.insert("2016-10-06");
		holiday_in_2016_.insert("2016-10-07");

	}

	static StockUtil* Instance() {
		if(NULL == instance_)
			instance_ = new StockUtil();
		return instance_;
	}

	std::string get_first_day_of_month() {
		std::string first_day_of_month;
		time_t current_time = time(NULL);
		struct tm* current_tm = localtime(&current_time);
		char str_time[20];
		sprintf(str_time, "%d-%02d-%02d", current_tm->tm_year + 1900, current_tm->tm_mon + 1, 1);
		first_day_of_month = str_time;
		return first_day_of_month;
	}

	std::string get_first_day_of_week() {
			std::string first_day_of_week;
			time_t current_time = time(NULL);
			struct tm* current_tm = localtime(&current_time);
			int week = current_tm->tm_wday;
			int diff = 0;
			if (0 == week)
				diff = 7 * 24 * 60 * 60;
			else
				diff = (week - 1) * 24 * 60 * 60;
			long seconds_of_first_day_in_week = current_time - diff;
			struct tm*first_day_in_week_tm = localtime(&seconds_of_first_day_in_week);
			char str_time[20];
			sprintf(str_time, "%d-%02d-%02d", first_day_in_week_tm->tm_year + 1900, first_day_in_week_tm->tm_mon + 1, first_day_in_week_tm->tm_mday);
			first_day_of_week = str_time;
			return first_day_of_week;
		}

	std::string get_current_day_str() {
		time_t current_time = time(NULL);
		struct tm* current_tm = localtime(&current_time);
		char str_today[20];
		sprintf(str_today, "%d-%02d-%02d", current_tm->tm_year + 1900, current_tm->tm_mon + 1, current_tm->tm_mday);
		return std::string(str_today);
	}

	bool is_trading_time() {
			time_t current_time = time(NULL);
			struct tm* current_tm = localtime(&current_time);
			//LOG_MSG2("current_tm->tm_wday=%d", current_tm->tm_wday);
			if(0 == current_tm->tm_wday || 6 == current_tm->tm_wday)
				return false;
			char str_hour_time[20];
			sprintf(str_hour_time, "%02d%02d%02d", current_tm->tm_hour, current_tm->tm_min, current_tm->tm_sec);
			//LOG_MSG2("str_hour_time=%s", str_hour_time);
			if (strcmp(str_hour_time, "092500") < 0 || strcmp(str_hour_time, "150000") > 0
					|| (strcmp(str_hour_time, "113000") > 0 && strcmp(str_hour_time, "130000") < 0))
				return false;
			char str_today[20];
			sprintf(str_today, "%d-%02d-%02d", current_tm->tm_year + 1900, current_tm->tm_mon + 1, current_tm->tm_mday);
			//LOG_MSG2("str_today=%s", str_today);
			if(holiday_in_2016_.end() != holiday_in_2016_.find(str_today)) {
				//LOG_MSG2("return false%d",1);
				return false;
			}
			//LOG_MSG2("return true%d",1);
			return true;
		}

	bool is_trading_day() {
		time_t current_time = time(NULL);
		struct tm* current_tm = localtime(&current_time);
		if(0 == current_tm->tm_wday || 6 == current_tm->tm_wday)
			return false;
		char str_today[20];
		sprintf(str_today, "%d-%02d-%02d", current_tm->tm_year + 1900, current_tm->tm_mon + 1, current_tm->tm_mday);
		if(holiday_in_2016_.end() != holiday_in_2016_.find(str_today)) {
			return false;
		}
		return true;
	}

	bool serialize(base_logic::DictionaryValue* value, std::string& json) {
		base_logic::ValueSerializer* serializer = base_logic::ValueSerializer::Create(base_logic::IMPL_JSON);
		bool r = serializer->Serialize(*value, &json);
		delete serializer;
		serializer = NULL;
		return r;
	}

	static StockUtil* instance_;
	std::set<std::string> holiday_in_2016_;
};

class StockBasicInfo {
public:

	StockBasicInfo() {
		code_ = "";
		name_ = "";
		industry_ = "";
		outstanding_ = 0.0;
		bvps_ = 0.0;
		pb_ = 0.0;
		market_value_ = 0.0;
		current_trade_ = 0.0;
		current_trade_time_ = 0;
		current_open_ = 0.0;
		current_high_ = 0.0;
		current_low_ = 0.0;
		current_settlement_ = 0.0;
		surged_limit_price_ = 0.0;
		decline_limit_price_ = 0.0;
		change_percent_ = 0.0;
		week_change_percent_ = 0.0;
		month_change_percent_ = 0.0;
		volume_ = 0.0;
		week_volume_ = 0.0;
		month_volume_ = 0.0;
		week_trade_day_ = "";
		month_trade_day_ = "";
		is_surged_limit_ = false;
		is_decline_limit_ = false;
	}

	std::string code() {return code_;}

	void set_code(std::string code) {code_ = code;}

	std::string week_trade_day() {return week_trade_day_;}

    void set_week_trade_day(std::string week_trade_day) {
    	week_trade_day_ = week_trade_day;
    }

    std::string month_trade_day() {return month_trade_day_;}

    void set_month_trade_day(std::string month_trade_day) {
    	month_trade_day_ = month_trade_day;
    }

	std::string name() {return name_;}

	void set_name(std::string name) {name_ = name;}

	std::string industry() {return industry_;}

	void set_industry(std::string industry) {industry_ = industry;}

	double volume() const {return volume_;}

	void set_volume(double volume) {
		volume_ = volume;
	}

	double week_volume() const {return week_volume_;}

	void set_week_volume(double week_volume) {
		week_volume_ = week_volume;
	}

	double month_volume() const {return month_volume_;}

	void set_month_volume(double month_volume) {
		month_volume_ = month_volume;
	}

	double change_percent() {return change_percent_;}

	void set_change_percent(double change_percent) {
		change_percent_ = change_percent;
	}

	double week_change_percent() {return week_change_percent_;}

	void set_week_change_percent(double week_change_percent) {
		week_change_percent_ = week_change_percent;
	}

	double month_change_percent() {return month_change_percent_;}

	void set_month_change_percent(double month_change_percent) {
		month_change_percent_ = month_change_percent;
	}

	double outstanding() {return outstanding_;}

	void set_outstanding(double outstanding) {outstanding_ = outstanding;}

	double bvps() {return bvps_;}

	void set_bvps(double bvps) {bvps_ = bvps;}

	double pb() {return pb_;}

	void set_pb(double pb) {pb_ = pb;}

	double market_value() {return outstanding_*bvps_*pb_;}

	void set_market_value(double market_value) {market_value_ = market_value;}

	double current_trade() {return current_trade_;}

	void set_current_trade(double current_trade) {
		current_trade_ = current_trade;
	}

	int current_trade_time() {return current_trade_time_;}

	void set_current_trade_time(double current_trade_time) {current_trade_time_ = current_trade_time;}

	bool is_surged_limit() {return is_surged_limit_;}

	bool is_decline_limit() {return is_decline_limit_;}

	double round(double r) {
	    return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
	}

	double get_surged_limit_price() {
		double rounded_range = round(current_settlement_ * 0.1 * 1e3)/1e3;
		surged_limit_price_ = rounded_range + current_settlement_;
		return surged_limit_price_;
	}

	double get_decline_limit_price() {
		double rounded_range = round(current_settlement_ * 0.1 * 1e3)/1e3;
		decline_limit_price_ = current_settlement_ - rounded_range;
		return decline_limit_price_;
	}

	void check_limit() {
		get_surged_limit_price();
		get_decline_limit_price();
		if (current_trade_ > current_settlement_) {
			double diff = surged_limit_price_ - current_trade_;
			if (diff < 0.01 && diff > -0.01)
				is_surged_limit_ = true;
		} else if (current_trade_ < current_settlement_) {
			double diff = decline_limit_price_ - current_trade_;
			if (diff < 0.01 && diff > -0.01) {
				is_decline_limit_ = true;
			}
		}
	}

	void ValueSerialization(base_logic::DictionaryValue* dict);

	void RealtimeValueSerialization(base_logic::DictionaryValue* dict);

	void MonthWeekDataValueSerialization(base_logic::DictionaryValue* dict);

	void UpdateTodayKLine();

	std::string code_;
    std::string name_;
	std::string industry_;
	double volume_;
	double week_volume_;
	double month_volume_;
	double change_percent_;
	double week_change_percent_;
	double month_change_percent_;
    double outstanding_;
    double bvps_;
    double pb_;
    double market_value_;
    double current_trade_;
    double current_open_;
    double current_high_;
    double current_low_;
    double current_settlement_;
    int current_trade_time_;
    double surged_limit_price_;
    double decline_limit_price_;
    std::string week_trade_day_;
    std::string month_trade_day_;
    bool is_surged_limit_;
    bool is_decline_limit_;
};

typedef struct HistDataPerDay {
	std::string date_;
	double open_;
	double high_;
	double close_;
	double low_;
	double support_pos_;
	double pressure_pos_;
	double mid_price_;
}HistDataPerDay;

class StockHistDataInfo {
public:

	void add_hist_data(std::string date, double open, double high, double close, double low);

	void ValueSerialization(base_logic::DictionaryValue* dict);

	void CountSupportPressurePos();

	std::map<std::string, HistDataPerDay> stock_hist_data_;
};

class StockTotalInfo {
public:
	StockTotalInfo();
	~StockTotalInfo();

	void ValueSerialization(base_logic::DictionaryValue* dict);

	void add_stock_industry(std::string industry_name) {
		basic_info_.set_industry(industry_name);
	}

	void update_kline_json();

	const std::string& getLineJson() const {
		return KLine_json_;
	}

	void setLineJson(const std::string& lineJson) {
		KLine_json_ = lineJson;
	}

    StockHistDataInfo hist_data_info_;
	StockBasicInfo basic_info_;
	std::string KLine_json_;
};

class HotDiagramPerStock {
public:
	HotDiagramPerStock() {
		code_ = "";
		name_ = "";
		volume_ = 0.0;
		week_volume_ = 0.0;
		month_volume_ = 0.0;
		change_percent_ = 0.0;
		week_change_percent_ = 0.0;
		month_change_percent_ = 0.0;
	}
	std::string code() const {return code_;}

	void set_code(std::string code) {code_ = code;}

	std::string name() const {return name_;}

	void set_name(std::string name) {name_ = name;}

	double volume() const {return volume_;}

	void set_volume(double volume) {volume_ = volume;}

	double week_volume() const {return week_volume_;}

	void set_week_volume(double week_volume) {
		week_volume_ = week_volume;
	}

	double month_volume() const {return month_volume_;}

	void set_month_volume(double month_volume) {
		month_volume_ = month_volume;
	}

	double change_percent() const {return change_percent_;}

	void set_change_percent(double change_percent) {change_percent_ = change_percent;}

	double week_change_percent() const {return week_change_percent_;}

	void set_week_change_percent(double week_change_percent) {week_change_percent_ = week_change_percent;}

	double month_change_percent() const {return month_change_percent_;}

	void set_month_change_percent(double month_change_percent) {month_change_percent_ = month_change_percent;}

	static bool cmp(const HotDiagramPerStock& t_stock, const HotDiagramPerStock& d_stock) {
		return fabs(t_stock.change_percent()) > fabs(d_stock.change_percent());
	}

	static bool week_cmp(const HotDiagramPerStock& t_stock, const HotDiagramPerStock& d_stock) {
		return fabs(t_stock.week_change_percent()) > fabs(d_stock.week_change_percent());
	}

	static bool month_cmp(const HotDiagramPerStock& t_stock, const HotDiagramPerStock& d_stock) {
		return fabs(t_stock.month_change_percent()) > fabs(d_stock.month_change_percent());
	}

	std::string code_;
	std::string name_;
	double volume_;
	double week_volume_;
	double month_volume_;
	double change_percent_;
	double week_change_percent_;
	double month_change_percent_;
};

class BasicIndustryInfo {
public:

	BasicIndustryInfo() {
		industry_name_ = "";
		industry_market_value_ = 0.0;
		industry_changepoint_ = 0.0;
		industry_month_changepoint_ = 0.0;
		industry_week_changepoint_ = 0.0;
		industry_volume_ = 0.0;
		week_industry_volume_ = 0.0;
		month_industry_volume_ = 0.0;
		fall_stock_num_ = 0;
		rise_stock_num_ = 0;
	}

	std::string industry_name() {return industry_name_;}

	void set_industry_name(std::string industry_name) {industry_name_ = industry_name;}

	double industry_volume() {return industry_volume_;}

	void set_industry_volume(double industry_volume) {
		industry_volume_ = industry_volume;
	}

	double week_industry_volume() {return week_industry_volume_;}

	void set_week_industry_volume(double week_industry_volume) {
		week_industry_volume_ = week_industry_volume;
	}

	double month_industry_volume() {return month_industry_volume_;}

	void set_month_industry_volume(double month_industry_volume) {
		month_industry_volume_ = month_industry_volume;
	}

	double industry_market_value() {return industry_market_value_;}

	void set_industry_market_value(double industry_market_value) {industry_market_value_ = industry_market_value;}

	void add_price_info(std::string stock_name, double price) {
		stock_price_info_[stock_name] = price;
	}

	double industry_changepoint() const {return industry_changepoint_;}

	void set_industry_changepoint(double industry_changepoint) {industry_changepoint_ = industry_changepoint;}

	double industry_week_changepoint() const {return industry_week_changepoint_;}

	void set_industry_week_changepoint(double industry_week_changepoint) {
		industry_week_changepoint_ = industry_week_changepoint;
	}

	double industry_month_changepoint() const {return industry_month_changepoint_;}

	void set_industry_month_changepoint(double industry_month_changepoint) {
		industry_month_changepoint_ = industry_month_changepoint;
	}

	int rise_stock_num() const {return rise_stock_num_;}

	void set_rise_stock_num(int rise_stock_num) {rise_stock_num_ = rise_stock_num;}

	int fall_stock_num() const {return fall_stock_num_;}

	void set_fall_stock_num(int fall_stock_num) {fall_stock_num_ = fall_stock_num;}

	std::string ten_hot_diagram_stock_json() const{return ten_hot_diagram_stock_json_;}

	void set_ten_hot_diagram_stock_json(std::string ten_hot_diagram_stock_json) {
		ten_hot_diagram_stock_json_ = ten_hot_diagram_stock_json;
	}

	static bool cmp(const BasicIndustryInfo& t_info, const BasicIndustryInfo& s_info) {
		return fabs(t_info.industry_changepoint()) > fabs(s_info.industry_changepoint());
	}

	static bool week_cmp(const BasicIndustryInfo& t_info, const BasicIndustryInfo& s_info) {
		return fabs(t_info.industry_week_changepoint()) > fabs(s_info.industry_week_changepoint());
	}

	static bool month_cmp(const BasicIndustryInfo& t_info, const BasicIndustryInfo& s_info) {
		return fabs(t_info.industry_month_changepoint()) > fabs(s_info.industry_month_changepoint());
	}

	void update_hot_diagram_stock_json();

	void update_hot_diagram_stock_json_by_day() {
		hot_diagram_stocks_.sort(HotDiagramPerStock::cmp);
        ten_hot_diagram_stock_json_ = get_stocks_json("day");
	}

	void update_hot_diagram_stock_json_by_week() {
		hot_diagram_stocks_.sort(HotDiagramPerStock::week_cmp);
		week_ten_hot_diagram_stock_json_ = get_stocks_json("week");
	}

	void update_hot_diagram_stock_json_by_month() {
		hot_diagram_stocks_.sort(HotDiagramPerStock::month_cmp);
		month_ten_hot_diagram_stock_json_ = get_stocks_json("month");
	}

	std::string get_stocks_json(std::string type);

	std::map<std::string, double> get_stocks_in_industry() {
		return stock_price_info_;
	}

	std::string get_hot_diagram_by_type(std::string type) {
		if ("week" == type)
			return week_ten_hot_diagram_stock_json_;
		else if ("month" == type)
			return month_ten_hot_diagram_stock_json_;
		return ten_hot_diagram_stock_json_;
	}

	std::string industry_name_;//TODO hello
	double industry_volume_;
	double week_industry_volume_;
	double month_industry_volume_;
	double industry_changepoint_;
	double industry_week_changepoint_;
	double industry_month_changepoint_;
	double industry_market_value_;
    std::map<std::string, double> stock_price_info_;
    int rise_stock_num_;
    int fall_stock_num_;
    std::list<HotDiagramPerStock> hot_diagram_stocks_;
    std::string ten_hot_diagram_stock_json_;
    std::string week_ten_hot_diagram_stock_json_;
    std::string month_ten_hot_diagram_stock_json_;
};

class HotDiagramPerIndustry {
public:
	HotDiagramPerIndustry() {
		industry_name_ = "";
		industry_change_percent_ = 0.0;
		industry_week_change_percent_ = 0.0;
		industry_month_change_percent_ = 0.0;
	}
	std::string industry_name() const{return industry_name_;}

	void set_industry_name(std::string industry_name) {industry_name_ = industry_name;}

	double industry_change_percent() const{return industry_change_percent_;}

	void set_industry_change_percent(double industry_change_percent) {
		industry_change_percent_ = industry_change_percent;
	}

	double industry_week_change_percent() const{return industry_week_change_percent_;}

	void set_industry_week_change_percent(double industry_week_change_percent) {
		industry_week_change_percent_ = industry_week_change_percent;
	}

	double industry_month_change_percent() const{return industry_month_change_percent_;}

	void set_industry_month_change_percent(double industry_month_change_percent) {
		industry_month_change_percent_ = industry_month_change_percent;
	}

	static bool cmp(const HotDiagramPerIndustry& t_info, const HotDiagramPerIndustry& s_info) {
		return fabs(t_info.industry_change_percent()) > fabs(s_info.industry_change_percent());
	}

	static bool week_cmp(const HotDiagramPerIndustry& t_info, const HotDiagramPerIndustry& s_info) {
		return fabs(t_info.industry_week_change_percent()) > fabs(s_info.industry_week_change_percent());
	}

	static bool month_cmp(const HotDiagramPerIndustry& t_info, const HotDiagramPerIndustry& s_info) {
		return fabs(t_info.industry_month_change_percent()) > fabs(s_info.industry_month_change_percent());
	}

	std::string industry_name_;
	double industry_change_percent_;
	double industry_week_change_percent_;
	double industry_month_change_percent_;
};

class IndustryInfo {
public:

	void ValueSerialization(base_logic::DictionaryValue* dict);

	void update_hottest_industry() {
		hot_diagram_industry_.clear();
		std::map<std::string, BasicIndustryInfo>::iterator iter = industry_info_map_.begin();
		for (; iter != industry_info_map_.end(); iter++) {
			hot_diagram_industry_.push_back(iter->second);
		}

        update_hottest_industry_by_day();
        update_hottest_industry_by_week();
        update_hottest_industry_by_month();
        LOG_MSG2("day_industry_hot_diagram_=%s", day_industry_hot_diagram_.c_str());
        LOG_MSG2("week_industry_hot_diagram_=%s", week_industry_hot_diagram_.c_str());
        LOG_MSG2("month_industry_hot_diagram_=%s", month_industry_hot_diagram_.c_str());
	}

	void update_hottest_industry_by_day() {
		hot_diagram_industry_.sort(BasicIndustryInfo::cmp);
		day_industry_hot_diagram_ = get_industry_json("day", hot_diagram_industry_);
	}

	void update_hottest_industry_by_week() {
		hot_diagram_industry_.sort(BasicIndustryInfo::week_cmp);
		week_industry_hot_diagram_ = get_industry_json("week", hot_diagram_industry_);
	}

	void update_hottest_industry_by_month() {
		hot_diagram_industry_.sort(BasicIndustryInfo::month_cmp);
		month_industry_hot_diagram_ = get_industry_json("month", hot_diagram_industry_);
	}

	std::string get_industry_json(std::string type, std::list<BasicIndustryInfo>& industry_list);

	void set_dapan_info(stock_logic::net_reply::VIPNewsList* vip_list);

	BasicIndustryInfo& get_industry_by_name(std::string industry_name) {
		return industry_info_map_[industry_name];
	}

	std::string get_industry_hot_diagram_by_type(std::string type) {
		if ("week" == type)
			return week_industry_hot_diagram_;
		else if ("month" == type)
			return month_industry_hot_diagram_;
		return day_industry_hot_diagram_;
	}

    std::map<std::string, BasicIndustryInfo> industry_info_map_;
    std::string day_industry_hot_diagram_;
    std::string week_industry_hot_diagram_;
    std::string month_industry_hot_diagram_;
    std::list<BasicIndustryInfo> hot_diagram_industry_;
};

}

#endif
