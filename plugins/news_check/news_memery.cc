// Copyright (c) 2016 The george Authors. All rights reserved.
// news_memery.cc
// Created on: 2016年5月18日.
// Author: Paco.


#include "../news_check/news_memery.h"

#include <assert.h>

#include "pub/tools/tools.h"

#include "base/logic/logic_comm.h"

//base_logic::RLockGd lk(lock_);
//base_logic::WLockGd lk(lock_);
namespace news {

NewsMemery* NewsMemery::instance_;

NewsMemery::NewsMemery() {
  lasttime_ = 0;
  oldtime_ = 0;
  news_mysql_ = NULL;
  InitThreadrw(&lock_);
}

NewsMemery::~NewsMemery() {
  DeinitThreadrw(lock_);
}

NewsMemery* NewsMemery::GetInstance() {
  if (NULL == instance_)
    instance_ = new NewsMemery();
  return instance_;
}

void NewsMemery::InitNewMysql(NewsMysql* ptr) {
  news_mysql_ = ptr;
}

int NewsMemery::UpdateCacheNews() {
  LOG_MSG("start to update");
  int err = 0;
  do {
    NewsMap map;
    LOG_MSG2("exec sql before map.size(%d)", map.size());
    LOG_MSG2("lasttime_(%ld)", lasttime_);
    news_mysql_->QueryNews(lasttime_, MAX_NEWS_COUNT, &map);
    if (map.size() <= 0) {
      LOG_MSG("no news update");
      break;
    }
    lasttime_ = tools::CurrentTime()*1000;
    LOG_MSG2("exec sql after map.size(%d)", map.size());
    NewsAnalyze(&map);
  } while (0);
  LOG_MSG("end to update");
  return err;
}

//新闻解析
void NewsMemery::NewsAnalyze(NewsMap* map) {
  if (map->size() <= 0)
    return;
  NewsMap::iterator it = map->begin();
  for (; it != map->end(); ++it) {
    News news = it->second;
    NewsMap::iterator it_cache = news_map_.find(news.id());
    //新闻已经在内存中，需要比对更新
    if (it_cache != news_map_.end()) {
      News old_news = it_cache->second;
      news_map_.insert(NewsMap::value_type(news.id(), news));
      UpdateNews(news, old_news);
    }
    else {
      if (news_map_.size() >= MAX_NEWS_COUNT) {
        DelNews();
      }
      SaveNewsCache(news);
    }
  }

}

void NewsMemery::UpdateNews(News newer, News older) {
  assert(newer.id() == older.id());
  std::vector<std::string> increase;
  std::vector<std::string> decrease;
  CompareString(newer.rf_stock(), older.rf_stock(), &increase, &decrease);
  AddAndDelCom(&st_map_, &increase, &decrease, newer.id());
  CompareString(newer.rf_industry(), older.rf_industry(), &increase, &decrease);
  AddAndDelCom(&in_map_, &increase, &decrease, newer.id());
  CompareString(newer.rf_section(), older.rf_section(), &increase, &decrease);
  AddAndDelCom(&se_map_, &increase, &decrease, newer.id());
}

void AddAndDelCom(RfNewsMap* map, std::vector<std::string>* increase,
                  std::vector<std::string>* decrease, int64 id) {
  std::vector<std::string>::iterator de_it = decrease->begin();
  for (; de_it != decrease->end(); ) {
    RfNewsMap::iterator it = map->find(*de_it);
    if (it != map->end()) {
      std::map<int64, int64> tmap = it->second;
      std::map<int64, int64>::iterator tmap_it = tmap.find(id);
      if (tmap_it != tmap.end()) {
        tmap.erase(tmap_it);
      }
    }
    decrease->erase(++de_it);
  }
  std::vector<std::string>::iterator in_it = increase->begin();
  for (; in_it != increase->end(); ) {
    RfNewsMap::iterator it = map->find(*in_it);
    if (it != map->end()) {
      std::map<int64, int64> tmap = it->second;
      std::map<int64, int64>::iterator tmap_it = tmap.find(id);
      if (tmap_it == tmap.end()) {
        tmap[id] = id;
      }
    }
    increase->erase(++in_it);
  }

  assert(increase->size() == 0);
  assert(decrease->size() == 0);
}

void CompareString(std::string newer, std::string older,
                   std::vector<std::string>* increase,
                   std::vector<std::string>* decrease) {
  do {
    *decrease = tools::Split(older, ",");
    *increase = tools::Split(newer, ",");
    std::vector<std::string>::iterator de_it = decrease->begin();
    bool flag;
    for (; de_it != decrease->end(); ) {
      std::vector<std::string>::iterator in_it = increase->begin();
      flag = false;
      for (; in_it != increase->end(); ) {
        if (*in_it == *de_it) {
          increase->erase(++in_it);
          decrease->erase(++de_it);
          flag = true;
          break;
        } else {
          ++in_it;
        }
      }
      if (!flag)
        ++de_it;
    }
  } while (0);

}

void NewsMemery::DelNews() {
  NewsMap::iterator it = news_map_.begin();
  for (int i = 0; i < 500 && it != news_map_.end(); i++) {
      News news = it->second;
      DelNewsCache(news);
      news_map_.erase(++it);
  }
}

void NewsMemery::DelNewsCache(News news) {
  std::string stocks = news.rf_stock();
  std::string industry = news.rf_industry();
  std::string session = news.rf_section();
  RfMapDel(&st_map_, stocks, news.id());
  RfMapDel(&in_map_, industry, news.id());
  RfMapDel(&se_map_, session, news.id());
}

void NewsMemery::SaveNewsCache(News news) {
  std::string stocks = news.rf_stock();
  std::string industry = news.rf_industry();
  std::string session = news.rf_section();
  RfMapAdds(&st_map_, stocks, news.id());
  RfMapAdds(&in_map_, industry, news.id());
  RfMapAdds(&se_map_, session, news.id());
}

bool NewsMemery::RfMapDel(RfNewsMap* map, std::string strs, int64 id) {
  if (strs.length() <= 0)
    return false;
  std::string::size_type pos;
  std::string pattern = ",";
  strs += pattern;
  int size = strs.size();
  std::map<int64, int64> temp_map;
  for (int i = 0; i < size; i++) {
    pos = strs.find(pattern, i);
    if (pos < size) {
      //item 股票-行业-板块
      std::string item = strs.substr(i , pos - i);
      RfNewsMap::iterator it = map->find(item);
      // 关系表中存在，则删除
      if(it != map->end()) {
        map->erase(it);
      }
      i = pos + pattern.size() - 1;
    }
  }
  return true;
}

bool NewsMemery::RfMapAdds(RfNewsMap* map, std::string strs, int64 id) {
  if (strs.length() <= 0)
    return false;
  std::string::size_type pos;
  std::string pattern = ",";
  strs += pattern;
  int size = strs.size();
  std::map<int64, int64> temp_map;
  for (int i = 0; i < size; i++) {
    pos = strs.find(pattern, i);
    if (pos < size) {
      //item 股票-行业-板块
      std::string item = strs.substr(i , pos - i);
      RfNewsMap::iterator it = map->find(item);
      if (it != map->end()) {
        std::map<int64, int64>& n_map = it->second;
        if (n_map.find(id) == n_map.end()) {
          n_map[id] = id;
        }
      } else {
        std::map<int64, int64> n_map;
        n_map[id] = id;
        map->insert(RfNewsMap::value_type(item, n_map));
      }
      i = pos + pattern.size() - 1;
    }
  }
  return true;
}

void NewsMemery::GetRfNews(std::string key, int type) {
  RfNewsMap::iterator it1 = st_map_.begin();
  RfNewsMap::iterator it2 = in_map_.begin();
  RfNewsMap::iterator it3 = se_map_.begin();
  std::string s;
  int64 n = -1;
  for(; it1 != st_map_.end(); ++it1) {
    s = it1->first;
    LOG_MSG2("key[%s]", s.c_str());
    std::map<int64, int64> tmap = it1->second;
    std::map<int64, int64>::iterator it11 = tmap.begin();
    for(; it11 != tmap.end(); ++it11) {
      n = it11->first;
      LOG_MSG2("id[%lld]",  n);
    }
  }
  for(; it2 != in_map_.end(); ++it2) {
    s = it2->first;
    LOG_MSG2("key[%s]", s.c_str());
    std::map<int64, int64> tmap = it2->second;
    std::map<int64, int64>::iterator it11 = tmap.begin();
    for(; it11 != tmap.end(); ++it11) {
      n = it11->first;
      LOG_MSG2("id[%lld]",  n);
    }
  }
  for(; it3 != se_map_.end(); ++it3) {
    s = it3->first;
    LOG_MSG2("key[%s]", s.c_str());
    std::map<int64, int64> tmap = it3->second;
    std::map<int64, int64>::iterator it11 = tmap.begin();
    for(; it11 != tmap.end(); ++it11) {
      n = it11->first;
      LOG_MSG2("id[%lld]",  n);
    }
  }
  return;
}

}  // namespace news
