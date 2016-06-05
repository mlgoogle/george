// Copyright (c) 2016 The george Authors. All rights reserved.
// news_proto_buf.h
// Created on: 2016年5月30日.
// Author: Paco.
#ifndef PLUGINS_NEWS_CHECK_NEWS_PROTO_BUF_H_
#define PLUGINS_NEWS_CHECK_NEWS_PROTO_BUF_H_

#include "base/logic/base_values.h"

#include "pub/net/proto_buf.h"
#include "pub/net/typedef.h"

namespace news {

namespace recv {

class RelateNews : public george_logic::PacketHead {
 public:
  RelateNews() {
    uid_ = -1;
    count_ = -1;
    relate_ = -1;
    last_id_ = -1;
  }
  virtual ~RelateNews() {}

 public:
  virtual void set_http_packet(DicValue* value);

  void set_uid(const int64 uid) { uid_ = uid; }

  void set_token(const std::string& token) { token_ = token; }

  void set_count(const int32 count) { count_ = count; }

  void set_relate(const int32 relate) { relate_ = relate; }

  void set_last_id(const int64 last_id) { last_id_ = last_id; }

  void set_key_name(const std::string& name) { key_name_ = name; }

  int32 count() { return count_; }
  std::string& token() { return token_; }
  int64 uid() { return uid_; }
  int32 relate() { return relate_; }
  int64 last_id() { return last_id_; }
  std::string& key_name() { return key_name_; }

 private:
  int64 uid_;
  std::string token_;
  int32 count_;
  int32 relate_;
  int64 last_id_;
  std::string key_name_;
};

class StNews : public george_logic::PacketHead {
 public:
  StNews() {
    uid_ = -1;
  }
  virtual ~StNews() {}

  virtual void set_http_packet(DicValue* value);
  void set_uid(const int64 uid) { uid_ = uid; }
  void set_token(const std::string& token) { token_ = token; }
  void set_stock(const std::string& stock) { stock_ = stock; }

  std::string& token() { return token_; }
  int64 uid() { return uid_; }
  std::string& stock() { return stock_; }
 private:
  int64 uid_;
  std::string token_;
  std::string stock_;
};

}  // namespace recv

namespace send {

class SendNews {
 public:
  SendNews() {
    nid_ = NULL;
    type_ = NULL;
    time_ = NULL;
    sentiment_ = NULL;
    title_ = NULL;
    url_ = NULL;
    summary_ = NULL;
    stock_ = NULL;
    industry_ = NULL;
    section_ = NULL;
    from_ = NULL;
    value_ = NULL;
  }
  ~SendNews() {
    if (nid_) { delete nid_; nid_ = NULL; }
    if (type_) { delete type_; type_ = NULL; }
    if (time_) { delete time_; time_ = NULL; }
    if (sentiment_) { delete sentiment_; sentiment_ = NULL; }
    if (title_) { delete title_; title_ = NULL; }
    if (url_) { delete url_; url_ = NULL; }
    if (summary_) { delete summary_; summary_ = NULL; }
    if (stock_) { delete stock_; stock_ = NULL; }
    if (industry_) { delete industry_; industry_ = NULL; }
    if (section_) { delete section_; section_ = NULL; }
    if (from_) { delete from_; from_ = NULL; }
  }

  void set_nid(const int64 id) { nid_ = new base_logic::FundamentalValue(id); }
  void set_type(const int32 type) { type_ = new base_logic::FundamentalValue(type); }
  void set_time(const int64 time) { time_ = new base_logic::FundamentalValue(time); }
  void set_sentiment(const int32 sent) { sentiment_ = new base_logic::FundamentalValue(sent); }
  void set_title(const std::string& title) { title_ = new base_logic::StringValue(title); }
  void set_url(const std::string& url) { url_ = new base_logic::StringValue(url); }
  void set_summary(const std::string& summary) { summary_ = new base_logic::StringValue(summary); }
  void set_stock(const std::string& stock) { stock_ = new base_logic::StringValue(stock); }
  void set_industry(const std::string& ind) { industry_ = new base_logic::StringValue(ind); }
  void set_section(const std::string& sec) { section_ = new base_logic::StringValue(sec); }
  void set_from (const std::string& from) { from_ =  new base_logic::StringValue(from); }

  DicValue* get() {
    value_ = new DicValue();
    if (nid_) value_->Set(L"nid", nid_);
    if (type_) value_->Set(L"type", type_);
    if (time_) value_->Set(L"time", time_);
    if (sentiment_) value_->Set(L"sentiment", sentiment_);
    if (title_) value_->Set(L"title", title_);
    if (url_) value_->Set(L"url", url_);
    if (summary_) value_->Set(L"summary", summary_);
    if (stock_) value_->Set(L"stock", stock_);
    if (industry_) value_->Set(L"industry", industry_);
    if (section_) value_->Set(L"section", section_);
    if (from_) value_->Set(L"from", from_);
    return value_;
  }
 private:
  base_logic::FundamentalValue* nid_;
  base_logic::FundamentalValue* type_;
  base_logic::FundamentalValue* time_;
  base_logic::FundamentalValue* sentiment_;
  base_logic::StringValue* title_;
  base_logic::StringValue* url_;
  base_logic::StringValue* summary_;
  base_logic::StringValue* stock_;
  base_logic::StringValue* industry_;
  base_logic::StringValue* section_;
  base_logic::StringValue* from_;
  DicValue* value_;

};

class SendSt {
 public:
  SendSt() {
    count_ = NULL;
    nagative_ = NULL;
    non_nagative_ = NULL;
    value_ = NULL;
  }
  ~SendSt() {
    if (count_) { delete count_; count_ = NULL; }
    if (non_nagative_) { delete non_nagative_; non_nagative_ = NULL; }
    if (nagative_) { delete nagative_; nagative_ = NULL; }
  }

  void set_count(const int32 count) { count_ = new base_logic::FundamentalValue(count); }
  void set_nagative(const int32 nagative) { nagative_ = new base_logic::FundamentalValue(nagative); }
  void set_non_nagative(const int32 non_nagative) { non_nagative_ = new base_logic::FundamentalValue(non_nagative); }

  DicValue* get() {
    value_ = new DicValue();
    if (count_) value_->Set(L"news_count", count_);
    if (nagative_) value_->Set(L"nagative", nagative_);
    if (non_nagative_) value_->Set(L"non_nagative", non_nagative_);
    return value_;
  }
 private:
  base_logic::FundamentalValue* count_;
  base_logic::FundamentalValue* nagative_;
  base_logic::FundamentalValue* non_nagative_;

  DicValue* value_;

};

class NewsList : public george_logic::PacketHead {
 public:
  NewsList() {
    list_ = new base_logic::ListValue();
  }

  virtual ~NewsList() {
    if (list_ != NULL) {
        delete list_;
        list_ = NULL;
    }
    if (body_value_ != NULL) {
        delete body_value_;
        body_value_ = NULL;
    }
  }

  void set_news(base_logic::Value* value) {
      list_->Append(value);
  }

  base_logic::DictionaryValue* body() {
    body_value_ = new base_logic::DictionaryValue();
    body_value_->SetWithoutPathExpansion(L"list",list_);
    body_value_->SetInteger(L"count",list_->GetSize());
    return body_value_;
  }

  int size() {
    return list_ == NULL ? 0 : list_->GetSize();
  }

  void set_http_packet(base_logic::DictionaryValue* value) {}

 private:
  base_logic::ListValue* list_;
};

}  // namespace send

}  // namespace news



#endif  // PLUGINS_NEWS_CHECK_NEWS_PROTO_BUF_H_
