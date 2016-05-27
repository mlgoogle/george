// Copyright (c) 2016 The george Authors. All rights reserved.
// news.h
// Created on: 2016年5月18日.
// Author: Paco.
#ifndef PLUGINS_NEWS_CHECK_NEWS_H_
#define PLUGINS_NEWS_CHECK_NEWS_H_

#include <assert.h>
#include <string>

#include "news_check/mutexlock.h"
#include "pub/net/typedef.h"

namespace news {

class News {
 public:
  News();
  ~News();
  News(const News& news);
  News& operator = (const News& news);

  void Serialization(DicValue* value);

  int64 id() const { return data_->id_; }
  int64 type() const { return data_->type_; }
  std::string title() const { return data_->title_; }
  std::string url() const { return data_->url_; }
  int8 sentiment() const { return data_->sentiment_; }
  std::string summary() const { return data_->summary_; }
  int64 time() const { return data_->time_; }
  int64 updated_time() const { return data_->updated_time_; }
  std::string rf_stock() const { return data_->rf_stock_; }
  std::string rf_industry() const { return data_->rf_industry_; }
  std::string rf_section() const { return data_->rf_section_; }
  std::string from() const { return data_->from_; }

  void set_id(int64 id) { data_->id_ = id; }
  void set_type(int64 type) { data_->type_ = type; }
  void set_title(std::string title) { data_->title_ = title; }
  void set_url(std::string url) { data_->url_ = url; }
  void set_sentiment(int8 sentiment) { data_->sentiment_ = sentiment; }
  void set_summary(std::string summary) { data_->summary_ = summary; }
  void set_time(int64 time) { data_->time_ = time; }
  void set_updated_time(int64 time) { data_->updated_time_ = time; }
  void set_rf_stock(std::string stock) { data_->rf_stock_ = stock; }
  void set_rf_industry(std::string industry) { data_->rf_industry_ = industry; }
  void set_rf_section(std::string section) { data_->rf_section_ = section; }
  void set_from(std::string from) { data_->from_ = from; }

  class Data {
   public:
    Data() : id_(-1), type_(-1), time_(0), updated_time_(0), sentiment_(0),
        refs_(1) {};

   public:
    int64 id_;
    int64 type_;
    int64 time_;
    int64 updated_time_;
    int8 sentiment_;
    std::string title_;
    std::string url_;
    std::string summary_;
    std::string rf_stock_;
    std::string rf_industry_;
    std::string rf_section_;
    std::string from_;

    void AddRef() {
      MutexLock lock(&refs_mutex_);
      ++refs_;
    }

    void Release() {
      bool do_delete = false;
      MutexLock lock(&refs_mutex_);
      --refs_;
      assert(refs_ >= 0);
      if (refs_ <= 0) {
        do_delete = true;
      }
      if (do_delete) {
        delete this;
      }
    }

   private:
    int refs_;
    Mutex refs_mutex_;
  };

 private:
  Data* data_;
};

}  // namespace news

#endif  // PLUGINS_NEWS_CHECK_NEWS_H_
