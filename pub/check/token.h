//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年6月1日 Author: kerry



#ifndef CHECK_TOKEN_H_
#define CHECK_TOKEN_H_
#include "logic/base_values.h"

namespace base_logic {

class TokenMgr {
 public:
	TokenMgr();
	virtual ~TokenMgr();
 public:
	bool CreateToken(const base_logic::Value* uid,
			const base_logic::ListValue* key);

	bool CheckToken(const base_logic::Value* uid,
			const base_logic::ListValue* key);

 private:
	bool CreateToken(const std::string& uid,const std::string& key);
};
}





#endif /* TOKEN_H_ */
