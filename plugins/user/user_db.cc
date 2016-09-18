//  Copyright (c) 2015-2015 The geprge Authors. All rights reserved.
//  Created on: 2016年5月18日 Author: kerry

#include "user_db.h"
#include "basic/basic_util.h"
#include <mysql.h>
namespace user_logic {

UserDB::UserDB(config::FileConfig* config) {
  mysql_engine_ = base_logic::DataEngine::Create(MYSQL_TYPE);
  mysql_engine_->InitParam(config->mysql_db_list_);
}

UserDB::~UserDB() {
  if (mysql_engine_) {
    delete mysql_engine_;
    mysql_engine_ = NULL;
  }
}

bool UserDB::RegisterUser(const std::string& phone,
                          const std::string& password) {
  bool r = false;
  //call vip.proc_RegisterUser('15168236344','1234456')
  std::string sql = "call proc_RegisterUser('" + phone + "','" + password
      + "');";
  scoped_ptr<base_logic::DictionaryValue> dict(
      new base_logic::DictionaryValue());
  dict->SetString(L"sql", sql);
  r = mysql_engine_->ReadData(0, (base_logic::Value*) (dict.get()), NULL);
  if (!r)
    return false;
  return true;
}

bool UserDB::LoginUser(const std::string& phone, const std::string& password,
                       user_logic::UserInfo& info) {
  bool r = false;
  //call vip.proc_LoginUser('18668169052','123456x')
  base_logic::DictionaryValue*  dictvalue;
  std::string sql = "call proc_LoginUser('" + phone + "','" + password + "');";
  scoped_ptr<base_logic::DictionaryValue> dict(
      new base_logic::DictionaryValue());
  dict->SetString(L"sql", sql);
  r = mysql_engine_->ReadData(0, (base_logic::Value*) (dict.get()),
                              CallLoginUserInfo);
  if (!r)
    return false;
  dict->GetDictionary(L"resultvalue", &dictvalue);
  info.ValueSerialization(dictvalue);
  return true;
}

bool UserDB::LoginSMS(const std::string& phone, user_logic::UserInfo& info) {
  bool r = false;
  //call vip.proc_LoginSMSUserInfo('18668169052')
  base_logic::DictionaryValue*  dictvalue;
  std::string sql = "call proc_LoginSMSUserInfo('" + phone + "');";
  scoped_ptr<base_logic::DictionaryValue> dict(
      new base_logic::DictionaryValue());
  dict->SetString(L"sql", sql);
  r = mysql_engine_->ReadData(0, (base_logic::Value*) (dict.get()),
                              CallLoginUserInfo);
  if (!r)
    return false;
  dict->GetDictionary(L"resultvalue", &dictvalue);
  info.ValueSerialization(dictvalue);
  return true;
}

void UserDB::CallLoginUserInfo(void* param, base_logic::Value* value) {
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) (value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  base_logic::DictionaryValue* info_value =
            new base_logic::DictionaryValue();
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      if (rows[0] != NULL)
        info_value->SetBigInteger(L"uid", atoll(rows[0]));
      if (rows[1] != NULL)
        info_value->SetString(L"phone", rows[1]);
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*) (info_value));
}

}
