
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <iostream>
#include <sstream>

// #include "client/linux/handler/exception_handler.h"

//#if defined (FCGI_STD)
//#include <fcgi_stdio.h>
//#elif defined(FCGI_PLUS)
//#include <fcgio.h>
//#include <fcgi_config.h>
//#endif

#include "pub/net/comm_head.h"
#include "fcgimodule/fcgimodule.h"
#include "base/logic/logic_comm.h"
#include "plugins/news_check/operator_code.h"

// 设置请求类型
#define API_TYPE george_logic::NEWS_TYPE
//#define LOG_TYPE log_trace::TRACE_API_LOG
#define OPERATOR_TYPE RELATE_NEWS_REQ

int main(int agrc, char* argv[]) {
  fcgi_module::FcgiModule fcgi_client;
  std::string core_sock_file = "/var/www/tmp/pacocorefile";
  fcgi_client.Init(core_sock_file, API_TYPE, RELATE_NEWS_REQ, 1);
  LOG_MSG("333333333");
  fcgi_client.Run();
  LOG_MSG("44444444444");
  fcgi_client.Close();
  return 0;
}
