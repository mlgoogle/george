//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年6月2日 Author: kerry

#include "token.h"
#include "algo/tea.h"
#include "basic/basic_util.h"

template<class T>
void SeparatorStr(const std::string &instr, \
  char separator, T *l, bool repeat = true) {
  std::string str(instr);

  if (str == "") return;

  str = std::string(1, separator) + str;

  if (str[str.length() - 1] != separator) {
    str = str + std::string(1, separator);
  }

  MapStrToInt64 map_repeat;
  map_repeat.clear();
  MapStrToInt64::iterator tmp_it;
  std::string value;
  std::string::size_type start = 0;
  std::string::size_type end = str.find(separator, start + 1);
  while (end != std::string::npos) {
    if (end - start == 1) {
      value = "";
    } else {
      value = str.substr(start + 1, end - (start + 1));
    }

    start = end;
    end = str.find(separator, start + 1);

    if (repeat == false) {
      tmp_it = map_repeat.find(value);
      if (tmp_it == map_repeat.end()) {
        map_repeat[value] = 1;
      } else {
        continue;
      }
    }

    l->push_back(value);
  }
}


namespace base_logic {

TokenMgr::TokenMgr() {
}

TokenMgr::~TokenMgr() {

}

bool TokenMgr::CheckToken(const base_logic::Value* uid,
			const base_logic::Value* key) {
	return true;
}

bool TokenMgr::CheckToken(const std::string& uid,
		const std::string& key,const std::string& token) {
	struct tea_data td;
	std::string temp_str(token);
	td.l = FormatNum(&td.d, temp_str);

	algo::Encrypt(&td);
	std::string temp("");
	for (int i = 0; i < td.l; ++i) {
		temp.append(1, (reinterpret_cast<char*>(td.d))[i]);
	}
	return true;
}


std::string TokenMgr::CreateToken(const base_logic::Value* uid,
		const base_logic::Value* key) {
	std::string str_uid;
	std::string str_key;
	bool r =false;
	r = uid->GetAsString(&str_uid);
	if (!r)
		return false;
	r = key->GetAsString(&str_key);
	if (!r)
		return false;
	return CreateToken(str_uid, str_key);
}

std::string TokenMgr::CreateToken(const std::string& uid,const std::string& key) {
	return CreateToken(uid,time(NULL),key);
}

std::string TokenMgr::CreateToken(const std::string& uid,const int64 timestamp,
		const std::string& key) {

	std::string token;
	std::string src = uid + "," +
			base::BasicUtil::StringUtil::Int64ToString(timestamp) + key;
	size_t tea_len = ((src.length() - 1)/8 + 1) * 8; //tea加密长度为8的倍数
	char* in = reinterpret_cast<char*>(malloc(tea_len));
	memset(in, 0, tea_len);
	strcpy(in,src.c_str());

	struct tea_data td;
	td.d = reinterpret_cast<void*>(in);
	td.l = tea_len;

	algo::Encrypt(&td);
	FomatString(td.d,td.l,&token);
	return token;
}

void TokenMgr::FomatString(void* d, size_t l, std::string* token) {
	char* p = reinterpret_cast<char*>(d);
	int32 temp = 0;
	for (int i = 0; i< l; ++i) {
		temp = p[i];
		(*token).append(base::BasicUtil::StringUtil::Int64ToString(temp));
		(*token).append(",");
	}
}

size_t TokenMgr::FormatNum(void** d,std::string& token) {
	ContainerStr out;
	SeparatorStr<ContainerStr>(token, ',', &out, true);
	*d = reinterpret_cast<void *>(malloc(out.size()));
	char *p = reinterpret_cast<char*>(*d);
	for (int i = 0; i < out.size(); ++i) {
		p[i] = atoi(out[i].c_str());
	}

	return out.size();
}

}



