// Copyright (c) 2016 The george Authors. All rights reserved.
// news_memery.h
// Created on: 2016年5月18日.
// Author: Paco.
#ifndef PLUGINS_NEWS_CHECK_NEWS_MEMERY_H_
#define PLUGINS_NEWS_CHECK_NEWS_MEMERY_H_

#include <stdio.h>
#include <string.h>

#include "news/news_base.h"
#include "news/news_mysql.h"
#include "news/news_proto_buf.h"
#include "thread/base_thread_lock.h"

const int MAX_NEWS_COUNT = 10000;
const int EVERY_NEWS_COUNT = 4000;
namespace news {

class NewsMemery {
 public:
  static NewsMemery* GetInstance();

  int UpdateCacheNews();
  void InitNewMysql(NewsMysql* ptr);
  void NewsAnalyze(NewsMap* news_map);
  void NewsStAnalyze(SimpleMap* news_map);

  void GetRfNews(std::string key, int type);

  void QueryNewsDetails(int64 nid, send::NewsList* out_list);
  //relate 0-股票 1-行业 2-概念
  //news_id 上一页获取的最小新闻id
  //news_count 这次要请求的新闻条数
  //out_list 结果集
  void QueryMemeryNews(int32 relate, std::string& key, int64 news_id,
                       int32 news_count, send::NewsList* out_list);

  void QueryCountNews(std::string stock, send::NewsList* out_list);
 private:
  void QueryMapNews(RfNewsMap map, int32 relate, int64 id, std::string& key,
                    int32 count, send::NewsList* out_list);
  void SetSendNews(send::SendNews* send_news, News news);
  void DelNews();

  //统计新闻数量，情感
  void CountStNews(SimpleNews news);
  //更新新闻分类
  void UpdateNews(News newer, News older);

  //describe : 比较新闻关联信息
  //increase : 新增关联数据
  //decrease ： 减少的关联数据
  void CompareString(std::string newer, std::string older,
                     std::vector<std::string>* increase,
                     std::vector<std::string>* decrease,
                     std::vector<std::string>* same = NULL);
  //
  void AddAndDelCom(RfNewsMap* map, std::vector<std::string>* increase,
                    std::vector<std::string>* decrease, int64 id);

  //向分类cache中添加新闻
  void SaveNewsCache(News news);

  //向分类cache中删除新闻
  void DelNewsCache(News news);

  bool RfMapAdds(RfNewsMap* map, std::string strs, int64 id);
  bool RfMapDel(RfNewsMap* map, std::string strs, int64 id);

  void Init7Vector(std::deque<NewsCount*>* deq);
  int32 GetDaysToCur(int64 millitime);
  void UpAllSimpleMap();
  void UpdateNewsCount(SimpleNews new_sim, SimpleNews old_sim);

  NewsMemery();
  ~NewsMemery();

 private:
  NewsMap news_map_;
  SimpleMap simple_map_;
  RfNewsMap st_map_;
  RfNewsMap in_map_;
  RfNewsMap se_map_;
  CountMap co_map_;

  //lasttime : 当前内存中新闻的更新时间（上次更新新闻的时间）
  int64 lasttime_;

  //count_time_ : 上次统计新闻时间
  int64 count_time_;
  int64 seven_days_time_;

  //由news_interface传入，该类不负责管理，只使用
  NewsMysql* news_mysql_;

  threadrw_t*  lock_;

 public:
  int64 GetDaysMilliSecond(int days);

 private:
  static NewsMemery* instance_;

};

}  // namespace news
#endif  // PLUGINS_NEWS_CHECK_NEWS_MEMERY_H_
