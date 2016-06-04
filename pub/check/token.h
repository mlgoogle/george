//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年6月1日 Author: kerry



#ifndef CHECK_TOKEN_H_
#define CHECK_TOKEN_H_
#include "logic/base_values.h"
#include <map>
#include <string>
#include <vector>

typedef std::map<std::string, int64> MapStrToInt64;
typedef std::vector<std::string> ContainerStr;

namespace base_logic {

class TokenMgr {
 public:
	TokenMgr();
	virtual ~TokenMgr();

 public:
	std::string CreateToken(const base_logic::Value* uid,
			const base_logic::Value* key);

	bool CheckToken(const base_logic::Value* uid,
			const base_logic::Value* key);

	std::string CreateToken(const std::string& uid,const std::string& key);

	bool CheckToken(const std::string& uid,const std::string& key,
			const std::string& token);

 private:
	std::string CreateToken(const std::string& uid,const int64 timestamp,
			const std::string& key);

	bool CheckToken(const std::string& uid,const std::string& key);

	void FomatString(void* d, size_t l, std::string* token);

	size_t FormatNum(void** d,std::string& token);
};
}





#endif /* TOKEN_H_ */
