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
const static Json::StaticString FIELD_FIELDS_NAME("fields_name"); //这里应该是多个，field_name下是json 这默认写2
const static Json::StaticString INDEX_MUTIL("is_mutil_index"); //判断是单个字段索引还是多个字段索引


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
  fields.clear();
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
  }

  name_ = name;
  field_ = field.name();
  isUnique_ = isUnique;
  fields.clear();
  return RC::SUCCESS;
}

RC IndexMeta::init(const char *name, const FieldMeta &field, bool isUnique,std::vector<const FieldMeta*> &fieldMetas,bool is_multi_index)
{
  if (nullptr == name || common::is_blank(name))
  {
    return RC::INVALID_ARGUMENT;
  }
  name_ = name;
  //
  if(is_multi_index ==false)
  {    
      LOG_INFO("题目：多列索引 multi-index,我是个  单字段索引 ,init name=%s",name);
      field_ = field.name();
  }else
  {
      fields.clear();
      for(int i=0;i<fieldMetas.size();i++)
      {
        std::string temp(fieldMetas[i]->name());
        fields.push_back(temp);
      }
      LOG_INFO("题目：多列索引 multi-index,我是多  单字段索引 ,init name=%s",name);
  }
  
  isUnique_ = isUnique;

  
  this->is_multi_index =is_multi_index;
  return RC::SUCCESS;
}

void IndexMeta::to_json(Json::Value &json_value) const
{

  //索引名字
  json_value[FIELD_NAME] = name_;

  if(fields.size() >=2)
  {
      json_value[INDEX_MUTIL] = true;
  }else
  {
    json_value[INDEX_MUTIL] = false;
  }
  
  //单个索引部分
  json_value[FIELD_FIELD_NAME] = field_;
  
  //多个索引部分
   Json::Value fields_value;
  for (std::string field : fields)
  {
    Json::Value field_value;
    field_value[FIELD_FIELD_NAME] = field;
    fields_value.append(std::move(field_value));
  }
  //1:多
  json_value[FIELD_FIELDS_NAME] = std::move(fields_value); 

  json_value[FIELD_IS_UNIQUE] = isUnique_;
}

RC IndexMeta::from_json(const TableMeta &table, const Json::Value &json_value, IndexMeta &index)
{
  const Json::Value &name_value = json_value[FIELD_NAME];
  const Json::Value &field_value = json_value[FIELD_FIELD_NAME];
  Json::Value fields_list = json_value[FIELD_FIELDS_NAME];  

  const Json::Value &unique_value = json_value[FIELD_IS_UNIQUE];
  const Json::Value &is_multi_value = json_value[INDEX_MUTIL];

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
  if (!is_multi_value.isBool())
  {
    //LOG_ERROR("Visible field is not a bool value. json value=%s", visible_value.toStyledString().c_str());
    return RC::GENERIC_ERROR;
  }
  bool is_mutil_index = is_multi_value.asBool();

  //恢复单个索引部分
  const FieldMeta *field = table.field(field_value.asCString());
  //前提条件：
  if (nullptr == field && is_mutil_index ==false)
  {
    LOG_ERROR("Deserialize index [%s]: no such field: %s", name_value.asCString(), field_value.asCString());
    return RC::SCHEMA_FIELD_MISSING;
  }
  //恢复多个索引部分
  std::vector<const FieldMeta*> fields;
  for(int i=0;i<fields_list.size() && true == is_mutil_index;i++)
  {
    const FieldMeta *field = table.field(fields_list[i][FIELD_FIELD_NAME].asCString());
    if (nullptr == field)
    {
      LOG_ERROR("Deserialize index [%s]: no such field: %s", name_value.asCString(), field_value.asCString());
      return RC::SCHEMA_FIELD_MISSING;
    }
    fields.push_back(field);
  }

  bool unique = unique_value.asBool();
  

  return index.init(name_value.asCString(), *field,unique,fields,is_mutil_index);
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