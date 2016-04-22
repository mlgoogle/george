#ifndef __DISPATCH__LOGIN_SVC__LOGIN_LOGIC___
#define __DISPATCH__LOGIN_SVC__LOGIN_LOGIC___

#include <string>
#include "core/common.h"
#include "net/http_data.h"

#define DEFAULT_CONFIG_PATH     "./plugins/user/user.xml"
#define VERIFY_TEMP_PATH    "./plugins/user/verify.html"

namespace user_svc_logic{
class Userlogic{

public:
    Userlogic();
    virtual ~Userlogic();

private:
    class crelease {
      ~crelease() {
        if (instance_ != NULL) {
          delete instance_;
          instance_ = NULL;
        }
      }
    };

    static crelease release_;

    static Userlogic    *instance_;

public:
    static Userlogic *GetInstance();
    std::string email_html_;

public:
    bool OnUserConnect (struct server *srv,const int socket);

    bool OnUserMessage (struct server *srv, const int socket,const void *msg, const int len);

    bool OnUserClose (struct server *srv,const int socket);

    bool OnBroadcastConnect(struct server *srv,const int socket,const void *data, const int len);

    bool OnBroadcastMessage (struct server *srv, const int socket, const void *msg, const int len);

    bool OnBroadcastClose (struct server *srv, const int socket);

    bool OnIniTimer (struct server *srv);

    bool OnTimeout (struct server *srv, char* id, int opcode, int time);


private:
    bool OnUserAttr(struct server *srv, \
                       const int socket, \
                       NetBase* netbase, \
                 const void* msg = NULL, \
                       const int len = 0);

    bool OnVerifyEmail(struct server *srv, \
                   const int socket, \
                   NetBase* netbase, \
             const void* msg = NULL, \
                   const int len = 0);
    bool OnLogin(struct server *srv, \
                   const int socket, \
                   NetBase* netbase, \
             const void* msg = NULL, \
                   const int len = 0);

    bool OnRegister(struct server *srv, \
                   const int socket, \
                   NetBase* netbase, \
             const void* msg = NULL, \
                   const int len = 0);
    bool Init();

    bool InitVerifyTemp();

};


}

#endif

