//  Copyright (c) 2015-2015 The news_logic.h Authors. All rights reserved.

#ifndef _HOME_TIANYIHENG_TEST_JINDOWIN_RUNNER_JINDOWIN_PLUGINS_NEWS_NEWS_LOGIC_H_
#define _HOME_TIANYIHENG_TEST_JINDOWIN_RUNNER_JINDOWIN_PLUGINS_NEWS_NEWS_LOGIC_H_
#include <string>
#include "core/common.h"
#include "net/http_data.h"
#include "news/news_packet.h"

#define DEFAULT_CONFIG_PATH           "./plugins/news/news_config.xml"
#define NUM_DATE_DIV                  1000000
#define CONTENT_LENGTH_MAX            400
#define NEWS_DATE_LEN                 19
#define DATETIME_STRING_TIME_POS      8
#define NEWS_NUM_REFERENCE            2

namespace news_logic {

enum QueryNewsType{
  ESTOCK_NEWS = 1,
  EINDUSTRY_NEWS = 2,
  ESECTION_NEWS = 3,
  ESTUDY_NEWS = 4,
  EALL_NEWS = 5
};

enum CommentType{
  ECOMMENT_UNCOMMENT = 0,
  ECOMMENT_UP = 1,
  ECOMMENT_DOWN = 2
};

enum TableType {
  ETABLE_NEWS = 0,
  ETABLE_INDUSTRY = 1,
  ETABLE_STOCK = 2,
  ETABLE_SECTION = 3,
  ETABLE_IMG = 4
};

enum SentimentType { 
  ESENTIMENT_STOCK,
  ESENTIMENT_INDUS,
  ESENTIMENT_SECT
};

class NewsLogic{
 public:
  NewsLogic();
  virtual ~NewsLogic();

 private:
  static NewsLogic  *instance_;

 public:
  static NewsLogic *GetInstance();
  static void FreeInstance();

 public:
  bool OnNewsConnect(struct server *srv, const int socket);
  bool OnNewsMessage(struct server *srv, const int socket, \
      const void *msg, const int len);
  bool OnNewsClose(struct server *srv, const int socket);
  bool OnBroadcastConnect(struct server *srv, const int socket, \
      const void *data, const int len);
  bool OnBroadcastMessage(struct server *srv, const int socket, \
      const void *msg, const int len);
  bool OnBroadcastClose(struct server *srv, const int socket);
  bool OnIniTimer(struct server *srv);
  bool OnTimeout(struct server *srv, char* id, int opcode, int time);

 public:
  static bool AppendNewsDetail(NetBase* news_kv);
  void GetStockReference(std::string related_stock, \
      NewsReferencSend *send_news, int64 user_id);
  void GetIndustryReference(std::string related_indus, \
      NewsReferencSend *send_news, int64 user_id);
  void GetSectReference(std::string related_sect, \
      NewsReferencSend *send_news, int64 user_id);
  static std::string GetTableByDate(std::string date, TableType etype);
  static void AppendSentiment(StockNewsSend* send_news, \
      news_logic::SentimentType etype, std::string key_field);
 private:
  bool Init();
  bool GetNews(struct server* srv,
      const int socket, NetBase* netbase,
      const void* msg = NULL,
      const int len = 0);
  int32 QueryNews(std::string stock_code,
      std::string hy_name,
      std::string sec_name,
      int64 news_type,
      int64 news_count,
      StockNewsSend* send_news);
  bool SetNews(struct server* srv,
      const int socket, NetBase* netbase,
      const void* msg = NULL,
      const int len = 0);

  bool GetNewsDetail(struct server* srv,
      const int socket, NetBase* netbase,
      const void* msg = NULL,
      const int len = 0);

  bool ShareNews(struct server* srv,
      const int socket, NetBase* netbase,
      const void* msg = NULL,
      const int len = 0);

  bool TransmitNews(struct server* srv,
      const int socket, NetBase* netbase,
      const void* msg = NULL,
      const int len = 0);

  bool RelatedNews(struct server* srv,
      const int socket, NetBase* netbase,
      const void* msg = NULL,
      const int len = 0);
  bool NewsReference(struct server* srv,
      const int socket, NetBase* netbase,
      const void* msg = NULL,
      const int len = 0);
};

}  // namespace news_logic

#endif  // _HOME_TIANYIHENG_TEST_JINDOWIN_RUNNER_JINDOWIN_PLUGINS_NEWS_NEWS_LOGIC_H_

