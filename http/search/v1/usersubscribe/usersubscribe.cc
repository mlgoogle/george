
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

#include "../pub/net/comm_head.h"
#include "fcgimodule/fcgimodule.h"
#include "logic/logic_comm.h"
#include "../plugins/search/operator_code.h"

// 设置请求类型
#define API_TYPE george_logic::SEARCH_TYPE
//#define LOG_TYPE log_trace::TRACE_API_LOG
#define OPERATOR_TYPE QUERY_SUBSCRIBE_REQ

int main(int agrc, char* argv[]) {
  fcgi_module::FcgiModule fcgi_client;
  std::string core_sock_file = "/var/www/tmp/georgecorefile";
  fcgi_client.Init(core_sock_file, API_TYPE, OPERATOR_TYPE, 1);
  fcgi_client.Run();
  fcgi_client.Close();
  return 0;
}
