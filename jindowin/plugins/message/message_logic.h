//  Copyright (c) 2016-2016 The jindowin Authors. All rights reserved.
//  Created on: 2016/3/28 Author: jiaoyongqing
#ifndef _HOME_RUNNER_JINDOWIN_JINDOWIN_PLUGINS_SEARCH_SEARCH_LOGIC_H_
#define _HOME_RUNNER_JINDOWIN_JINDOWIN_PLUGINS_SEARCH_SEARCH_LOGIC_H_

#include "net/http_data.h"
#include "net/operator_code.h"
#include "net/error_comm.h"
#include "core/common.h"

#define DEFAULT_CONFIG_PATH     "./plugins/message/message_config.xml"

namespace message_logic {
class Messagelogic {
 public:
  virtual ~Messagelogic();

 private:
  Messagelogic();

class crelease {
  ~crelease() {
    if (instance_ != NULL) {
      delete instance_;
      instance_ = NULL;
    }
  }
};

  static crelease release_;
  static Messagelogic *instance_;

 public:
  static Messagelogic *GetInstance() {
    if (instance_ == NULL) {
      instance_ = new Messagelogic();
    }
    return instance_;
  }

 public:
  bool OnMessageConnect(struct server *srv, const int socket);

  bool OnMessageMessage(struct server *srv, \
                        const int socket, \
                         const void *msg, \
                           const int len);

  bool OnMessageClose(struct server *srv, const int socket);

  bool OnBroadcastConnect(struct server *srv, \
                            const int socket, \
                            const void *data, \
                               const int len);

  bool OnBroadcastMessage(struct server *srv, \
                            const int socket, \
                             const void *msg, \
                                const int len);

  bool OnBroadcastClose(struct server *srv, const int socket);

  bool OnIniTimer(struct server *srv);

  bool OnTimeout(struct server *srv, char* id, int opcode, int time);

 private:
  bool OnMessageCode(struct server *srv, \
                  const int socket, \
                  NetBase* netbase, \
            const void* msg = NULL, \
                 const int len = 0);

  bool OnMessageHyGn(struct server *srv, \
                    const int socket, \
                    NetBase* netbase, \
              const void* msg = NULL, \
                   const int len = 0);
 private:
  bool Init();
};

}  //  namespace stocksvc_logic

#endif  //  _HOME_RUNNER_JINDOWIN_JINDOWIN_PLUGINS_SEARCH_SEARCH_LOGIC_H_

