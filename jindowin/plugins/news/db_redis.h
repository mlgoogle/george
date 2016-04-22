//  Copyright (c) 2015-2015 The db_redis.h Authors. All rights reserved.
//  Created on: 2016年1月8日 Author: paco

#ifndef _HOME_TIANYIHENG_TEST_JINDOWIN_RUNNER_JINDOWIN_PLUGINS_NEWS_DB_REDIS_H_
#define _HOME_TIANYIHENG_TEST_JINDOWIN_RUNNER_JINDOWIN_PLUGINS_NEWS_DB_REDIS_H_

#include <list>
#include <string>
#include <vector>
#include "public/basic/basic_info.h"
#include "base/dic/base_dic_redis_auto.h"
#include "news/news_base.h"
#include "news/news_packet.h"
#include "news/news_logic.h"

#define MIN_COUNT                   5
#define PAGE_NEWS_NUM               10
#define DAY_TIME_SEC                (24*3600)
#define NEWS_SUMMARY_LENGTH_MAX     300 
#define CHAR_MAX_VALUE              127
#define UTF8_SECOND_VALUE_MIN       128
#define UTF8_FIRST_VALUE_MIN        192

namespace news {

class DbRedis {
 public:
  DbRedis();
  ~DbRedis();
  static void Init(std::list<base::ConnAddr> *const addrlist);
  static void Dest();

 public:
  static int32 GetAllNews(int64 count, StockNewsSend* send_news);
  static int32 GetStockNews(std::string stock, int64 count, \
      StockNewsSend* send_news);
  static int32 GetHyNews(std::string hy_name, \
      int64 count, StockNewsSend* send_news);
  static int32 GetGnNews(std::string gn_name, \
      int64 count, StockNewsSend* send_news);
  static int32 GetYbNews(std::string gn_name, \
      int64 count, StockNewsSend* send_news);
  static int32 AterNews(int64 date, std::string news_id, \
      int64 commit_type, StockNewsSend* send_news, int64 user_id);
  static int32 QueryNewsById(std::string filed, \
      std::string news_id, std::string* news_json);

 private:
  static std::string GetRationNewsId(std::string hy_name, std::string filed, \
      base_storage::DictionaryStorageEngine* redis);
  // convert news date time from yyyymmddhhmmss to format of yyyy-mm-dd hh:mm:ss
  static void ConvertNewsDateTimeFmt(NetBase *news_kv);

 public:
  static std::string TimeNum2TimeFormat(int64 time_num);
  static std::string TimeFormat2TimeNumStr(std::string timefomat);
  static bool GetHashKeysFileds(std::string table_name, \
      std::vector<std::string> & out_field_list);
  // get page news
  static int32 GetRationNewsByPage(int32 &page_index, int32 page, \
      StockNewsSend* send_news, std::string filed, \
      base_storage::DictionaryStorageEngine* redis, int32* suc);
  static int32 GetNewsTotalSize(uint64 starttime, \
      std::vector<int32> &vec_day_news_size);
  static int32 GetAllPageNews(int64 page, StockNewsSend* send_news);
  static int32 SaveNews(const std::string &news_table, \
      const std::string &news_id, const std::string &news_json_str);
  static news_logic::CommentType CheckUserComment(int64 user_id, \
      std::string news_id, std::string news_date_fmt);
  static bool SaveUserComment(int64 user_id, std::string news_id, \
      std::string news_date_fmt, news_logic::CommentType etype);
  static void SetSendByPage(std::string filed, \
      std::string value, StockNewsSend* send_news, int32 page);
  static void AppendNewsData(NetBase *news_kv);
  static bool GetNewsImg(std::string news_id, std::string & out_img);
  static bool GetSentimentValue(news_logic::SentimentType etype, \
      std::string key_field, \
      std::string *out_val_ptr);
  static bool TruncatedUTF8String(std::string* src, size_t len);
  static void FilterNews(std::vector<std::string> * vec_news_ptr);
};

}  // namespace news


#endif  // _HOME_TIANYIHENG_TEST_JINDOWIN_RUNNER_JINDOWIN_PLUGINS_NEWS_DB_REDIS_H_
