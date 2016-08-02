//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月17日 Author: yangge
#include "logic/logic_comm.h"
#include "stock_basic_info.h"
#include "stock_factory.h"

namespace stock_logic {

StockUtil* StockUtil::instance_ = NULL;

StockTotalInfo::StockTotalInfo() {
  score_ = 0;
}

StockTotalInfo::~StockTotalInfo() {

}

void StockTotalInfo::countScore() {

}

void StockBasicInfo::ValueSerialization(base_logic::DictionaryValue* dict) {
  dict->GetString(L"code", &code_);
  dict->GetString(L"name", &name_);
  dict->GetReal(L"totalAssets", &outstanding_);
  dict->GetReal(L"bvps", &bvps_);
  dict->GetReal(L"pb", &pb_);
  market_value_ = outstanding_ * bvps_ * pb_;
}

std::string BasicIndustryInfo::get_stocks_json(std::string type) {
  stock_logic::net_reply::VIPNewsList* vip_list =
      new stock_logic::net_reply::VIPNewsList();
  std::list<HotDiagramPerStock>::iterator iter = hot_diagram_stocks_.begin();
  int count = 0;
  for (; iter != hot_diagram_stocks_.end(); iter++) {
    stock_logic::net_reply::STOIndustryData* industry_data =
        new stock_logic::net_reply::STOIndustryData();
    industry_data->set_code(iter->code());
    industry_data->set_name(iter->name());
    if (type == "day") {
      industry_data->set_change_percent(iter->change_percent());
      industry_data->set_volume(iter->volume());
    } else if (type == "week") {
      industry_data->set_change_percent(iter->week_change_percent());
      industry_data->set_volume(iter->week_volume());
    } else if (type == "month") {
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
  std::map<std::string, double>::iterator stocks_iter =
      stock_price_info_.begin();
  for (; stocks_iter != stock_price_info_.end(); stocks_iter++) {
    double stock_price = stocks_iter->second;
    if (stock_price > 0.001)
      increase_stock_num++;
    else if (stock_price < -0.001)
      decrease_stock_num++;
  }
  std::string stocks_hot_diagram_json;
  StockUtil::Instance()->serialize(vip_list->packet(), stocks_hot_diagram_json);
  return stocks_hot_diagram_json;
}

void BasicIndustryInfo::update_hist_data() {
  //TODO 需要改进效率，只更新最新的hist data
  clear_yield_info();
  std::map<std::string, double>::iterator iter = stock_price_info_.begin();
  for (; iter != stock_price_info_.end(); iter++) {
    std::string stock_name = iter->first;
    StockTotalInfo& stock_total_info = StockFactory::GetInstance()
        ->stock_usr_mgr_->stock_user_cache_->get_stock_total_info(stock_name);
    StockHistDataInfo& stock_hist_info = stock_total_info.hist_data_info_;
    std::map<std::string, HistDataPerDay>& stock_hist_data = stock_hist_info
        .stock_hist_data_;
    std::map<std::string, HistDataPerDay>::iterator hist_data_iter =
        stock_hist_data.begin();
    for (; hist_data_iter != stock_hist_data.end(); hist_data_iter++) {
      std::string date = hist_data_iter->first;
      add_yield_info_by_stock(date, stock_total_info.basic_info_.market_value(),
                              hist_data_iter->second);
      add_volatility_info_by_stock(date, stock_total_info.basic_info_.market_value(),
                                    hist_data_iter->second);
    }
  }
}

void BasicIndustryInfo::add_yield_info_by_stock(
    std::string date, double stock_market_value,
    HistDataPerDay& hist_data_unit) {
  HistDataPerDay& hist_data = industry_hist_data_info_.stock_hist_data_[date];
  double stock_yield = hist_data_unit.day_yield_;
  hist_data.day_yield_ += stock_yield * stock_market_value
      / industry_market_value_;
}

void BasicIndustryInfo::add_volatility_info_by_stock(
    std::string date, double stock_market_value,
    HistDataPerDay& hist_data_unit) {
  HistDataPerDay& hist_data = industry_hist_data_info_.stock_hist_data_[date];
  double stock_volatility = hist_data_unit.day_volatility_;
  hist_data.day_volatility_ += stock_volatility * stock_market_value
      / industry_market_value_;
}

void BasicIndustryInfo::clear_yield_info() {
  industry_hist_data_info_.stock_hist_data_.clear();
}

bool BasicIndustryInfo::get_chart_json(
    std::string& yield_json,
    std::map<int, YieldInfoUnit>& hs300_yield_data,
    std::string& name) {
  return StockUtil::Instance()->get_json_of_yield_data(hs300_yield_data,
                                                       industry_yield_infos_,
                                                       yield_json,
                                                       name);
}

bool BasicIndustryInfo::get_hist_data_json(
    std::map<std::string, HistDataPerDay>& hs300_hist_data,
    std::string& start_date, std::string& end_date,
    std::string& json_str) {
  return StockUtil::Instance()->get_json_of_hist_data(
      hs300_hist_data,
      start_date,
      end_date,
      industry_hist_data_info_.stock_hist_data_,
      json_str);
}

void BasicIndustryInfo::update_hot_diagram_stock_json() {
  std::map<std::string, stock_logic::StockTotalInfo>& stocks_map =
      StockFactory::GetInstance()->stock_usr_mgr_->stock_user_cache_
          ->stock_total_info_;
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
    stock_hot_diagram.set_month_change_percent(
        basic_info.month_change_percent());
    stock_hot_diagram.set_volume(basic_info.volume());
    stock_hot_diagram.set_week_volume(basic_info.week_volume());
    stock_hot_diagram.set_month_volume(basic_info.month_volume());
    hot_diagram_stocks_.push_back(stock_hot_diagram);
  }
  update_hot_diagram_stock_json_by_day();
  update_hot_diagram_stock_json_by_week();
  update_hot_diagram_stock_json_by_month();
}

std::string IndustryInfo::set_industry_json(
    std::string type, std::list<BasicIndustryInfo>& industry_list) {
  stock_logic::net_reply::VIPNewsList* vip_list =
      new stock_logic::net_reply::VIPNewsList();
  std::list<BasicIndustryInfo>::iterator iter = industry_list.begin();
  int count = 0;
  for (; iter != industry_list.end(); iter++) {
    //过滤掉事件类型
    if (0 != iter->type())
      continue;
    stock_logic::net_reply::STOIndustryData* industry_data =
        new stock_logic::net_reply::STOIndustryData();
    std::string industry_name = iter->industry_name();
    int industry_len = industry_name.length();
    int con_industry_len = strlen("industry");
    if (industry_len > con_industry_len)
      industry_name = industry_name.substr(0, industry_len - con_industry_len);
    industry_data->set_name(industry_name);
    if ("day" == type) {
      industry_data->set_change_percent(iter->industry_changepoint());
      industry_data->set_volume(iter->industry_volume());
    } else if ("week" == type) {
      industry_data->set_change_percent(iter->industry_week_changepoint());
      //industry_data->set_volume(iter->week_industry_volume());TODO数据不精确用volume代替
      industry_data->set_volume(iter->industry_volume());
    } else if ("month" == type) {
      industry_data->set_change_percent(iter->industry_month_changepoint());
      //industry_data->set_volume(iter->month_industry_volume());TODO数据不精确用volume代替
      industry_data->set_volume(iter->industry_volume());
    }
    base_logic::Value* industry_data_value = industry_data->get();
    vip_list->set_vip_news(industry_data_value);
    delete industry_data;
    industry_data = NULL;
    //industry_info_map_[iter->industry_name()].update_hot_diagram_stock_json();
    count++;
    if (count > 9)
      break;
  }
  set_dapan_info(vip_list);
  std::string stocks_hot_diagram_json = "";
  base_logic::DictionaryValue* dict = vip_list->packet();
  StockUtil::Instance()->serialize(dict, stocks_hot_diagram_json);
  //StockUtil::Instance()->jsonp_serialize(dict, industry_hot_diagram_jsonp_[type]);
  delete vip_list;
  vip_list = NULL;
  delete dict;
  dict = NULL;
  return stocks_hot_diagram_json;
}

void IndustryInfo::set_dapan_info(
    stock_logic::net_reply::VIPNewsList* vip_list) {
  std::map<std::string, stock_logic::StockTotalInfo>& stocks_map =
      StockFactory::GetInstance()->stock_usr_mgr_->stock_user_cache_
          ->stock_total_info_;
  stock_logic::StockTotalInfo& shangzhen = stocks_map["sh000001"];
  double shangzhen_index = shangzhen.basic_info_.current_trade();
  double shangzhen_changepercent = shangzhen.basic_info_.change_percent();
  stock_logic::net_reply::STOIndustryData* shangzhen_index_data =
      new stock_logic::net_reply::STOIndustryData();
  shangzhen_index_data->set_code("上证指数");
  shangzhen_index_data->set_name("sh000001");
  shangzhen_index_data->set_current_trade(shangzhen_index);
  shangzhen_index_data->set_change_percent(shangzhen_changepercent);
  vip_list->set_vip_news(shangzhen_index_data->get());

  stock_logic::StockTotalInfo& shenzhen = stocks_map["sh399001"];
  double shenzhen_index = shenzhen.basic_info_.current_trade();
  double shenzhen_changepercent = shenzhen.basic_info_.change_percent();
  stock_logic::net_reply::STOIndustryData* shenzhen_index_data =
      new stock_logic::net_reply::STOIndustryData();
  shenzhen_index_data->set_code("深证指数");
  shenzhen_index_data->set_name("sh399001");
  shenzhen_index_data->set_current_trade(shenzhen_index);
  shenzhen_index_data->set_change_percent(shenzhen_changepercent);
  vip_list->set_vip_news(shenzhen_index_data->get());

  stock_logic::StockTotalInfo& chuanye = stocks_map["sh399006"];
  double chuanye_index = chuanye.basic_info_.current_trade();
  double chuanye_changepercent = chuanye.basic_info_.change_percent();
  stock_logic::net_reply::STOIndustryData* chuanye_index_data =
      new stock_logic::net_reply::STOIndustryData();
  chuanye_index_data->set_code("创业板指");
  chuanye_index_data->set_name("sh399006");
  chuanye_index_data->set_current_trade(chuanye_index);
  chuanye_index_data->set_change_percent(chuanye_changepercent);
  vip_list->set_vip_news(chuanye_index_data->get());

}

void StockBasicInfo::RealtimeValueSerialization(
    base_logic::DictionaryValue* dict) {
  int trade_time;
  dict->GetInteger(L"time", &trade_time);
  current_trade_time_ = (int) time(NULL);
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
}

void StockBasicInfo::AllRealtimeValueSerialization(
    base_logic::DictionaryValue* dict) {
  int trade_time = 0;
  double change_percent, trade, open, high, low, settlement, volume = 0;
  dict->GetInteger(L"time", &trade_time);
  trade_time = (trade_time / 60) * 60;
  dict->GetReal(L"changepercent", &change_percent);
  dict->GetReal(L"trade", &trade);
  dict->GetReal(L"open", &open);
  dict->GetReal(L"high", &high);
  dict->GetReal(L"low", &low);
  dict->GetReal(L"settlement", &settlement);
  dict->GetReal(L"volume", &volume);

  yield_infos_[trade_time].set_all_data(trade_time, change_percent, trade, open,
                                        high, low, settlement, volume,
                                        change_percent);
}

void StockBasicInfo::YieldValueSerialization(
    base_logic::DictionaryValue* dict) {
  std::string code;
  double yield;
  int trade_time;
  //dict->GetString(L"code", &code);
  dict->GetInteger(L"time", &trade_time);
  dict->GetReal(L"yield", &yield);
  add_yield_info(trade_time, yield, yield);
}

void StockBasicInfo::VisitDataSerialization(
    base_logic::DictionaryValue* dict) {
  std::string code = "";
  int count = 0;
  int trade_time = 0;
  dict->GetInteger(L"timeTamp", &trade_time);
  trade_time = (int)(trade_time / 60) * 60;
  dict->GetInteger(L"count", &count);
  StockFactory::GetInstance()->stock_usr_mgr_->stock_user_cache_->update_min_visit_time(trade_time);
  add_visit_info(trade_time, count);
  if (code_ == "601288" || code_ == "000002")
    LOG_MSG2("code=%s,trade_time=%d,visit_count=%d",
             code_.c_str(),
             trade_time,
             count);
}

void StockBasicInfo::UpdateTodayKLine() {
  std::map<std::string, stock_logic::StockTotalInfo>& stocks_map =
      StockFactory::GetInstance()->stock_usr_mgr_->stock_user_cache_
          ->stock_total_info_;
  stock_logic::StockTotalInfo& total_info = stocks_map[code_];
  stock_logic::StockHistDataInfo& hist_data_info = total_info.hist_data_info_;
  std::string current_day = StockUtil::Instance()->get_current_day_str();
  hist_data_info.add_hist_data(current_day, current_open_, current_high_,
                               current_trade_, current_low_, current_trade_);
  hist_data_info.CountSupportPressurePos();
  hist_data_info.CountQFQYield();
}

void StockBasicInfo::MonthWeekDataValueSerialization(
    base_logic::DictionaryValue* dict) {
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
  } else if ("M" == type) {
    if (date < month_trade_day_)
      return;
    dict->GetReal(L"p_change", &month_change_percent_);
    dict->GetString(L"date", &month_trade_day_);
    dict->GetReal(L"volume", &month_volume_);
  }
}

void StockHistDataInfo::add_hist_data(std::string date, double open,
                                      double high, double close, double low,
                                      double qfq_price) {
  /*std::map<std::string, HistDataPerDay>::iterator iter = stock_hist_data_.begin();
   if (stock_hist_data_.end() != iter)
   continue;*/	//TODO 交易时间会出现数据错误问题
  HistDataPerDay hist_data_per_day;
  hist_data_per_day.open_ = open;
  hist_data_per_day.high_ = high;
  hist_data_per_day.close_ = close;
  hist_data_per_day.low_ = low;
  if (qfq_price > 1)
    hist_data_per_day.qfq_close_ = qfq_price;
  else
    hist_data_per_day.qfq_close_ = close;
  hist_data_per_day.mid_price_ = (high + low + close) / 3;
  hist_data_per_day.date_ = date;
  stock_hist_data_[date] = hist_data_per_day;
  StockUserCache* cache = StockFactory::GetInstance()->stock_usr_mgr_
      ->stock_user_cache_;
  if (date > cache->max_hist_data_time()) {
    cache->set_max_hist_data_time(date);
  }
}

void StockHistDataInfo::ValueSerialization(base_logic::DictionaryValue* dict) {
  std::string date;
  double open, high, close, low, qfq_close = 0.0;
  dict->GetString(L"date", &date);
  dict->GetReal(L"open", &open);
  dict->GetReal(L"high", &high);
  dict->GetReal(L"close", &close);
  dict->GetReal(L"low", &low);
  dict->GetReal(L"qfq_close", &qfq_close);
  add_hist_data(date, open, high, close, low, qfq_close);
}

void StockHistDataInfo::CountSupportPressurePos() {
  std::map<std::string, HistDataPerDay>::iterator iter =
      stock_hist_data_.begin();
  std::vector<HistDataPerDay> hist_data_vec;
  for (; iter != stock_hist_data_.end(); iter++) {
    hist_data_vec.push_back(iter->second);
  }
  int kline_num = hist_data_vec.size();
  if (code_ == "000002" || code_ == "601012") {
    LOG_MSG2("CountSupportPressurePos:kline_num=%d",kline_num);
  }
  if (kline_num < 12)
    return;
  int kline_start_pos = 0;
  double pre_qfq_close = -100;
  int total_count = hist_data_vec.size();
  if (total_count < 30) {
    kline_start_pos = 0;
  } else
    kline_start_pos = total_count - 30;
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
    double support_pos = hist_data_vec[i].mid_price_
        - (high_in_12_days - low_in_12_days);
    double pressure_pos = hist_data_vec[i].mid_price_
        + (high_in_12_days - low_in_12_days);
    std::string date = hist_data_vec[i].date_;
    if (pre_qfq_close < -1)
      pre_qfq_close = stock_hist_data_[date].qfq_close_;
    stock_hist_data_[date].support_pos_ = support_pos;
    stock_hist_data_[date].pressure_pos_ = pressure_pos;
    stock_hist_data_[date].month_init_price_ = hist_data_vec[kline_start_pos]
        .qfq_close_;
    stock_hist_data_[date].month_init_date_ = hist_data_vec[kline_start_pos]
        .date_;
    double price_diff = stock_hist_data_[date].qfq_close_ - pre_qfq_close;
    stock_hist_data_[date].day_yield_ = price_diff / pre_qfq_close;
    pre_qfq_close = stock_hist_data_[date].qfq_close_;
  }

}

void StockHistDataInfo::CountQFQYield() {
  std::map<std::string, HistDataPerDay>::iterator iter =
      stock_hist_data_.begin();
  std::vector<HistDataPerDay> hist_data_vec;
  for (; iter != stock_hist_data_.end(); iter++) {
    hist_data_vec.push_back(iter->second);
  }
  double pre_qfq_close = -100;
  double pre_close = -100;
  for (int i = 0; i < hist_data_vec.size(); i++) {
    std::string date = hist_data_vec[i].date_;
    if (pre_qfq_close < -1)
      pre_qfq_close = stock_hist_data_[date].qfq_close_;
    double price_diff = stock_hist_data_[date].qfq_close_ - pre_qfq_close;
    stock_hist_data_[date].day_yield_ = price_diff / pre_qfq_close;
    pre_qfq_close = stock_hist_data_[date].qfq_close_;

    if (pre_close < -1)
      pre_close = stock_hist_data_[date].close_;
    double volatility_price_diff = stock_hist_data_[date].high_ - stock_hist_data_[date].low_;
    stock_hist_data_[date].day_volatility_ = volatility_price_diff / pre_close;
    pre_close = stock_hist_data_[date].close_;
  }
}

void IndustryInfo::ValueSerialization(base_logic::DictionaryValue* dict) {
  std::string code, industry_name, board_code;
  dict->GetString(L"symbol", &code);
  dict->GetString(L"keyname", &industry_name);
  dict->GetString(L"boardcode", &board_code);
  if ("1105" == board_code) {
    industry_name += "concept";
    industry_info_map_[industry_name].set_type(2);
  } else if ("1109" == board_code) {
    industry_name += "industry";
    industry_info_map_[industry_name].set_type(0);
  }
  industry_info_map_[industry_name].add_price_info(code, 0);
  industry_info_map_[industry_name].set_industry_name(industry_name);
}

void IndustryInfo::EventsValueSerialization(base_logic::DictionaryValue* dict) {
  std::string stocks, event_name;
  dict->GetString(L"name", &event_name);
  std::map<std::string, BasicIndustryInfo>::iterator iter = industry_info_map_
      .find(event_name);
  //if (iter != industry_info_map_.end())
  //	event_name = event_name + "2";
  event_name += "event";
  dict->GetString(L"stock", &stocks);
  std::string split_chars = ",";
  std::vector<std::string> stock_vec;
  StockUtil::Instance()->stock_split(stocks, split_chars, stock_vec);
  for (int i = 0; i < stock_vec.size(); i++) {
    industry_info_map_[event_name].add_price_info(stock_vec[i], 0);
  }
  industry_info_map_[event_name].set_industry_name(event_name);
  industry_info_map_[event_name].set_type(1);
}

void IndustryInfo::CustomEventsValueSerialization(
    base_logic::DictionaryValue* dict) {
  std::string stocks, custom_event;
  dict->GetString(L"event_name", &custom_event);
  custom_event += "custom";
  dict->GetString(L"stocks", &stocks);
  std::string split_chars = ",";
  std::vector<std::string> stock_vec;
  StockUtil::Instance()->stock_split(stocks, split_chars, stock_vec);
  for (int i = 0; i < stock_vec.size(); i++) {
    industry_info_map_[custom_event].add_price_info(stock_vec[i], 0);
  }
  industry_info_map_[custom_event].set_industry_name(custom_event);
  industry_info_map_[custom_event].set_type(4);
}

void StockTotalInfo::update_kline_json() {
  KLine_json_ = "";
  setKLineJsonp("");
  std::map<std::string, HistDataPerDay>& stock_hist_data = hist_data_info_
      .stock_hist_data_;
  stock_logic::net_reply::VIPNewsList* vip_list =
      new stock_logic::net_reply::VIPNewsList();
  int count = 0;
  std::map<std::string, HistDataPerDay>::reverse_iterator iter = stock_hist_data
      .rbegin();
  for (; iter != stock_hist_data.rend(); iter++) {
    stock_logic::net_reply::STOIndustryData* kline_data =
        new stock_logic::net_reply::STOIndustryData();
    HistDataPerDay& hist_data = iter->second;
    kline_data->setDate(iter->first);
    kline_data->setOpen(hist_data.open_);
    kline_data->setHigh(hist_data.high_);
    kline_data->setLow(hist_data.low_);
    kline_data->setClose(hist_data.close_);
    kline_data->setSupport(hist_data.support_pos_);
    kline_data->setPressure(hist_data.pressure_pos_);
    kline_data->setDayYield(hist_data.day_yield_);
    kline_data->setMonthInitPrice(hist_data.month_init_price_);
    kline_data->setMonthInitDate(hist_data.month_init_date_);
    kline_data->setQfqClose(hist_data.qfq_close_);
    vip_list->set_vip_news(kline_data->get());
    delete kline_data;
    kline_data = NULL;
    count++;
    if (count > 29)
      break;
  }
  base_logic::DictionaryValue* kline_value = vip_list->packet();
  StockUtil::Instance()->serialize(kline_value, KLine_json_);
  StockUtil::Instance()->jsonp_serialize(kline_value, KLine_jsonp_);
  delete kline_value;
  kline_value = NULL;
  delete vip_list;
  vip_list = NULL;
}

}
