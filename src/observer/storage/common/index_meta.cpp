/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by wangyunlai.wyl on 2021/5/18.
//

#include "storage/common/index_meta.h"
#include "storage/common/field_meta.h"
#include "storage/common/table_meta.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "rc.h"
#include "json/json.h"

const static Json::StaticString FIELD_NAME("name");
const static Json::StaticString FIELD_FIELD_NAME("field_name");
const static Json::StaticString FIELD_IS_UNIQUE("unique");

//不在构造函数抛出异常
RC IndexMeta::init(const char *name, const FieldMeta &field)
{
  if (nullptr == name || common::is_blank(name))
  {
    return RC::INVALID_ARGUMENT;
    //https://songlee24.github.io/2015/01/12/cpp-exception-in-constructor/
  }

  name_ = name;
  field_ = field.name();
  isUnique_ = false;
  return RC::SUCCESS;
}

//不在构造函数抛出异常
RC IndexMeta::init(const char *name, std::vector<FieldMeta > fields_meta)
{

  //01 check 索引的名字 
  int length = fields_meta.size();
  if (0 == length)
  {
    return RC::INVALID_ARGUMENT;
  }

  if (nullptr == name || common::is_blank(name))
  {
    return RC::INVALID_ARGUMENT;
  }

  name_ = name;          //索引名字

  //field_ = field.name(); //索引字段
  //fields
  fields.clear();
  for(int i=0;i<length;i++)
  {
    fields.push_back(fields_meta[i].name());
  }
  
  isUnique_ = false;
  return RC::SUCCESS;
}

RC IndexMeta::init(const char *name, const FieldMeta &field, bool isUnique)
{
  if (nullptr == name || common::is_blank(name))
  {
    return RC::INVALID_ARGUMENT;
    //https://songlee24.github.io/2015/01/12/cpp-exception-in-constructor/
  }

  name_ = name;
  field_ = field.name();
  isUnique_ = isUnique;
  return RC::SUCCESS;
}

void IndexMeta::to_json(Json::Value &json_value) const
{
  json_value[FIELD_NAME] = name_;
  json_value[FIELD_FIELD_NAME] = field_;
  json_value[FIELD_IS_UNIQUE] = isUnique_;
}

RC IndexMeta::from_json(const TableMeta &table, const Json::Value &json_value, IndexMeta &index)
{
  const Json::Value &name_value = json_value[FIELD_NAME];
  const Json::Value &field_value = json_value[FIELD_FIELD_NAME];
  const Json::Value &unique_value = json_value[FIELD_IS_UNIQUE];

  if (!name_value.isString())
  {
    LOG_ERROR("Index name is not a string. json value=%s", name_value.toStyledString().c_str());
    return RC::GENERIC_ERROR;
  }

  if (!field_value.isString())
  {
    LOG_ERROR("Field name of index [%s] is not a string. json value=%s",
              name_value.asCString(), field_value.toStyledString().c_str());
    return RC::GENERIC_ERROR;
  }
  if (!unique_value.isBool())
  {
    //LOG_ERROR("Visible field is not a bool value. json value=%s", visible_value.toStyledString().c_str());
    return RC::GENERIC_ERROR;
  }

  const FieldMeta *field = table.field(field_value.asCString());
  if (nullptr == field)
  {
    LOG_ERROR("Deserialize index [%s]: no such field: %s", name_value.asCString(), field_value.asCString());
    return RC::SCHEMA_FIELD_MISSING;
  }

  bool unique = unique_value.asBool();

  return index.init(name_value.asCString(), *field, unique);
}

const char *IndexMeta::name() const
{
  return name_.c_str();
}

const char *IndexMeta::field() const
{
  return field_.c_str();
}

void IndexMeta::desc(std::ostream &os) const
{
  os << "index name=" << name_
     << ", field=" << field_;
}