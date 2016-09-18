
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <iostream>
#include <sstream>

// #include "client/linux/handler/exception_handler.h"

#if defined (FCGI_STD)
#include <fcgi_stdio.h>
#elif defined(FCGI_PLUS)
#include <fcgio.h>
#include <fcgi_config.h>
#endif

#include "log/mig_log.h"
#include "../pub/net/comm_head.h"
#include "../plugins/user/operator_code.h"
//#include "logtrace/log_trace.h"
#include "fcgimodule/fcgimodule.h"
// 设置请求类型
#define API_TYPE            george_logic::USER_TYPE
#define LOG_TYPE            log_trace::TRACE_API_LOG

int main(int agrc, char* argv[]) {
  fcgi_module::FcgiModule fcgi_client;
  //fcgi_client.Init("/var/www/tmp/georgecorefile", API_TYPE, 1);
  std::string core_sock_file = "/var/www/tmp/georgecorefile";
  fcgi_client.Init(core_sock_file,API_TYPE,
                   LOGIN_USER_REQ ,1);
  fcgi_client.Run();
  fcgi_client.Close();
  return 0;
}
