//  Copyright (c) 2015-2015 The geo Authors. All rights reserved.
//  Created on: 2016年5月23日 Author: kerry

#ifndef USER_OPERATOR_CODE_H__
#define USER_OPERATOR_CODE_H__

#include "net/comm_head.h"


namespace user_logic {

#define REGISTER_VERCODE_REQ   george_logic::USER_TYPE * 1000 + 1
#define REGISTER_VERCODE_RLY   george_logic::USER_TYPE * 1000 + 2

#define REGISTER_USER_REQ   george_logic::USER_TYPE * 1000 + 3
#define REGISTER_USER_RLY   george_logic::USER_TYPE * 1000 + 4

#define LOGIN_USER_REQ   george_logic::USER_TYPE * 1000 + 5
#define LOGIN_USER_RLY   george_logic::USER_TYPE * 1000 + 6

#define SMS_LOGIN_REQ   george_logic::USER_TYPE * 1000 + 7
#define SMS_LOGIN_RLY   george_logic::USER_TYPE * 1000 + 8

#define LOGIN_VERCODE_REQ   george_logic::USER_TYPE * 1000 + 9
#define LOGIN_VERCODE_RLY   george_logic::USER_TYPE * 1000 + 10

}
#endif
