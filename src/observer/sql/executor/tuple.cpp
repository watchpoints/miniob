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
// Created by Wangyunlai on 2021/5/14.
//

#include "sql/executor/tuple.h"
#include "storage/common/table.h"
#include "common/log/log.h"
#include <stdio.h>
#include <stdlib.h>

Tuple::Tuple(const Tuple &other)
{
  LOG_PANIC("Copy constructor of tuple is not supported");
  exit(1);
}

Tuple::Tuple(Tuple &&other) noexcept : values_(std::move(other.values_))
{
  selectComareIndex = -1;
}

Tuple &Tuple::operator=(Tuple &&other) noexcept
{
  if (&other == this)
  {
    return *this;
  }

  values_.clear();
  values_.swap(other.values_);
  return *this;
}

Tuple::~Tuple()
{
}

// add (Value && value)
void Tuple::add(TupleValue *value)
{
  values_.emplace_back(value);
}
void Tuple::add(const std::shared_ptr<TupleValue> &other)
{
  values_.emplace_back(other);
}
void Tuple::add(int value)
{
  add(new IntValue(value, 0));
}

void Tuple::add_null_value()
{
  add(new NullValue());
}

void Tuple::add(float value)
{
  add(new FloatValue(value, 0));
}
//按照列的名字，添加 values
void Tuple::add(const char *s, int len)
{
  add(new StringValue(s, len, 0));
}

void Tuple::add_text(const char *s, int len)
{
  add(new TextValue(s, len, 0));
}

void Tuple::add_date(int value)
{
  add(new DateValue(value, 0));
}

////////////////////////////////////////////////////////////////////////////////

std::string TupleField::to_string() const
{
  return std::string(table_name_) + "." + field_name_ + std::to_string(type_);
}

////////////////////////////////////////////////////////////////////////////////
void TupleSchema::from_table(const Table *table, TupleSchema &schema)
{
  const char *table_name = table->name();            //表名字
  const TableMeta &table_meta = table->table_meta(); //表结构
  const int field_num = table_meta.field_num();      //字段个数
  for (int i = 0; i < field_num; i++)
  {
    const FieldMeta *field_meta = table_meta.field(i);
    if (field_meta->visible())
    {
      schema.add(field_meta->type(), table_name, field_meta->name(), field_meta->nullable());
    }
  }
}

void TupleSchema::add(AttrType type, const char *table_name, const char *field_name, int nullable)
{
  fields_.emplace_back(type, table_name, field_name, nullable);
}

void TupleSchema::add(AttrType type, const char *table_name, const char *field_name)
{
  fields_.emplace_back(type, table_name, field_name);
}
void TupleSchema::add(AttrType type, const char *table_name, const char *field_name, bool visible)
{
  fields_.emplace_back(type, table_name, field_name, visible);
}

void TupleSchema::add_if_not_exists(AttrType type, const char *table_name, const char *field_name)
{
  for (const auto &field : fields_)
  {
    if (0 == strcmp(field.table_name(), table_name) &&
        0 == strcmp(field.field_name(), field_name))
    {
      LOG_INFO(">>>>>>>>>>add_if_exists. %s.%s", table_name, field_name);
      return;
    }
  }
  LOG_INFO("add_if_not_exists. %s.%s", table_name, field_name);
  add(type, table_name, field_name);
}
void TupleSchema::add_if_not_exists1(AttrType type, const char *table_name, const char *field_name, int nullable)
{
  for (const auto &field : fields_)
  {
    if (0 == strcmp(field.table_name(), table_name) &&
        0 == strcmp(field.field_name(), field_name))
    {
      LOG_INFO(">>>>>>>>>>add_if_exists. %s.%s", table_name, field_name);
      return;
    }
  }
  // LOG_INFO("add_if_not_exists. %s.%s", table_name, field_name);
  add(type, table_name, field_name, nullable);
}

void TupleSchema::add_if_not_exists(AttrType type, const char *table_name, const char *field_name, FunctionType ftype)
{
  //判断列是否存在
  for (const auto &field : fields_)
  {
    if (0 == strcmp(field.table_name(), table_name) &&
        0 == strcmp(field.field_name(), field_name) &&
        ftype == field.get_function_type())
    {
      LOG_INFO(">>>>>>>>>>add_if_exists. %s.%s", table_name, field_name);
      return;
    }
  }
  LOG_INFO("add_if_not_exists. %s.%s", table_name, field_name);
  add(type, table_name, field_name, ftype);
}

void TupleSchema::append(const TupleSchema &other)
{
  fields_.reserve(fields_.size() + other.fields_.size());
  for (const auto &field : other.fields_)
  {
    fields_.emplace_back(field);
  }
}

int TupleSchema::index_of_field(const char *table_name, const char *field_name) const
{
  const int size = fields_.size();
  for (int i = 0; i < size; i++)
  {
    const TupleField &field = fields_[i];
    if (0 == strcmp(field.table_name(), table_name) && 0 == strcmp(field.field_name(), field_name))
    {
      return i;
    }
  }
  return -1;
}

//列信息: fields_:(type_ = INTS, table_name_ = "t1", field_name_ = "id")
void TupleSchema::print(std::ostream &os) const
{
  if (fields_.empty())
  {
    os << "No schema";
    return;
  }
  // 判断有多张表还是只有一张表
  //并不使用 table_names的数据
  std::set<std::string> table_names;
  for (const auto &field : fields_)
  {
    table_names.insert(field.table_name());
  }
  //单表逻辑
  if (table_names.size() == 1)
  {
    //遍历n-1个元素.
    for (std::vector<TupleField>::const_iterator iter = fields_.begin(), end = --fields_.end();
         iter != end; ++iter)
    {
      //如果多个表:添加表名t.id
      if (table_names.size() > 1 || realTabeNumber > 1)
      {
        os << iter->table_name() << ".";
      }

      if (FunctionType::FUN_COUNT_ALL == iter->get_function_type())
      {
        //count(1)
        //if (0 == strcmp("*", iter->field_name()))
        //{
        //os << "count(*)"
        // << " | ";
        //}
        //else
        //{
        os << "count(" << iter->field_name_count_number() << ")"
           << " | ";
        //}
      }
      if (FunctionType::FUN_COUNT_ALL_ALl == iter->get_function_type())
      {
        //count(*)
        os << "count(*)"
           << " | ";
      }
      else if (iter->get_function_type() == FunctionType::FUN_COUNT)
      {
        os << "count(" << iter->field_name() << ")"
           << " | ";
      }
      else if (iter->get_function_type() == FunctionType::FUN_MAX)
      {
        os << "max(" << iter->field_name() << ")"
           << " | ";
      }
      else if (iter->get_function_type() == FunctionType::FUN_MIN)
      {
        os << "min(" << iter->field_name() << ")"
           << " | ";
      }
      else if (iter->get_function_type() == FunctionType::FUN_AVG)
      {
        os << "avg(" << iter->field_name() << ")"
           << " | ";
      }
      else
      {
        //正常情况
        os << iter->field_name() << " | ";
      }
    }
    //last col
    //visible

    if (table_names.size() > 1 || realTabeNumber > 1)
    {
      os << fields_.back().table_name() << ".";
    }
    //id ---- 最后一个列,后面没有 ｜，只有名字
    if (FunctionType::FUN_COUNT_ALL_ALl == fields_.back().get_function_type())
    {
      //count(*)
      os << "count(*)" << std::endl;
    }
    else if (fields_.back().get_function_type() == FunctionType::FUN_COUNT_ALL)
    {
      //os << "count(*)" << std::endl; //select count(*) from t;
      //if (0 == strcmp("*", fields_.back().field_name()))
      //{
      //os << "count(*)" << std::endl;
      //}
      //else
      //{
      os << "count(" << fields_.back().field_name_count_number() << ")" << std::endl;
      //}
    }
    else if (fields_.back().get_function_type() == FunctionType::FUN_COUNT)
    {
      os << "count(" << fields_.back().field_name() << ")" << std::endl;
    }
    else if (fields_.back().get_function_type() == FunctionType::FUN_MAX)
    {
      os << "max(" << fields_.back().field_name() << ")" << std::endl;
    }
    else if (fields_.back().get_function_type() == FunctionType::FUN_MIN)
    {
      os << "min(" << fields_.back().field_name() << ")" << std::endl;
    }
    else if (fields_.back().get_function_type() == FunctionType::FUN_AVG)
    {
      os << "avg(" << fields_.back().field_name() << ")" << std::endl;
    }
    //bug1 else if ->if
    else
    { //正常情况
      os << fields_.back().field_name() << std::endl;
    }

    //单表 列逻辑 ///////////////////////////////////////// end
  }
  else
  {
    //多表逻辑///////////////////////////////////////// 开始
    //https://github.com/oceanbase/miniob/blob/main/src/observer/sql/executor/tuple.cpp#123

    LOG_INFO(" join  query cols >>>>>>>>>>>>>>");
    //删除不显示的列
    /**
    std::vector<TupleField> tuplefields;
    for (std::vector<TupleField>::const_iterator iter = fields_.begin(), end =fields_.end();
         iter != end; ++iter)
    {
      tuplefields.push_back(*iter);
    }

     for (std::vector<TupleField>::iterator iter = tuplefields.begin(), end =tuplefields.end();
         iter != end; )
    {
       if( false ==iter->visible()){
            iter = tuplefields.erase(iter);
        }else{
              iter++;
        }
    }**/

    for (std::vector<TupleField>::const_iterator iter = fields_.begin(), end = --fields_.end();
         iter != end; ++iter)
    {
      if (table_names.size() > 1)
      {
        os << iter->table_name() << ".";
      }
      os << iter->field_name() << " | ";
    }

    if (table_names.size() > 1)
    {
      os << fields_.back().table_name() << ".";
    }
    os << fields_.back().field_name() << std::endl;

    ///////////////////////////end,多表逻辑////////////////////////////end
  }
}

/////////////////////////////////////////////////////////////////////////////
TupleSet::TupleSet(TupleSet &&other) : tuples_(std::move(other.tuples_)), schema_(other.schema_)
{
  other.schema_.clear();
  realTabeNumber = other.realTabeNumber;
  //push_back(std::move(TupleSet))
  old_schema = other.old_schema;
  commonIndex = other.commonIndex;
  select_table_type = other.select_table_type;
  dp = other.dp;
}

TupleSet &TupleSet::operator=(TupleSet &&other)
{
  if (this == &other)
  {
    return *this;
  }

  schema_.clear();
  schema_.append(other.schema_);
  other.schema_.clear();
  realTabeNumber = -1;
  tuples_.clear();
  //swap 交换
  tuples_.swap(other.tuples_);
  return *this;
}

void TupleSet::add(Tuple &&tuple)
{
  tuples_.emplace_back(std::move(tuple));
}

void TupleSet::clear()
{
  tuples_.clear();
  schema_.clear();
  old_schema.clear();
  dp.clear();
}
//print shows
void TupleSet::print(std::ostream &os)
{
  //列信息: (type_ = INTS, table_name_ = "t1", field_name_ = "id")
  if (schema_.fields().empty())
  {
    LOG_WARN("Got empty schema");
    return;
  }
  if (realTabeNumber > 1)
  {

    schema_.realTabeNumber = realTabeNumber;
  }
  else
  {
    schema_.realTabeNumber = -1;
  }

  schema_.print(os); //打印 列字段 （已经考虑到多个表）

  // 判断有多张表还是只有一张表
  std::set<std::string> table_names;
  for (const auto &field : schema_.fields())
  {
    table_names.insert(field.table_name());
  }

  //一个表：
  if (table_names.size() == 1)
  {
    //单表聚合:只有一行
    if (true == avg_print(os))
    {
      LOG_INFO("this is avg query >>>>>>>>>>>>>>>>>> ");
      return;
    }
    //单表显示多行 tuples_ 多行
    for (const Tuple &item : tuples_)
    {
      //第n-1列
      const std::vector<std::shared_ptr<TupleValue>> &values = item.values();
      for (std::vector<std::shared_ptr<TupleValue>>::const_iterator iter = values.begin(), end = --values.end();
           iter != end; ++iter)
      {
        (*iter)->to_string(os);
        os << " | ";
      }

      //最后一列
      values.back()->to_string(os);
      os << std::endl;
      //1 | 2 | 3
    }
  }
  else if (table_names.size() == 2)
  {
    ////多表操作/////////////////////////////////////
    //笛卡尔积算法描述

    if (tuples1_.size() == 0 || tuples2_.size() == 0)
    {
      return;
    }

    //t1.rows[i][j]
    //t2.rows[i][j]
    for (const Tuple &item_left : tuples1_)
    {
      std::shared_ptr<TupleValue> sp1;
      int col1 = 0;
      std::stringstream os_left;
      {
        //std::vector<std::shared_ptr<TupleValue>>  values_; 每一行 多个字段
        const std::vector<std::shared_ptr<TupleValue>> &values = item_left.values();
        for (std::vector<std::shared_ptr<TupleValue>>::const_iterator iter = values.begin(), end = values.end();
             iter != end; ++iter)
        {
          if (is_join == true && joins_index == col1)
          {
            sp1 = *iter;
            cout << ">>>>>>>>>>>>>join select " << endl;
          }

          (*iter)->to_string(os_left);
          os_left << " | ";

          col1++;
        }
      }

      //b表的多行 tuples_right 多行
      for (const Tuple &item_right : tuples2_)
      {
        std::shared_ptr<TupleValue> sp2;
        int col2 = 0;
        std::stringstream os_right;
        {
          const std::vector<std::shared_ptr<TupleValue>> &values = item_right.values();
          for (std::vector<std::shared_ptr<TupleValue>>::const_iterator iter = values.begin(), end = --values.end();
               iter != end; ++iter)
          {
            //笛卡尔积:查询条件
            if (is_join == true && joins_index == col2)
            {
              sp2 = *iter;
              cout << ">>>>>>>>>>>>>join select " << endl;
            }
            (*iter)->to_string(os_right);

            os_right << " | ";
            col2++;
          }
          //小王疑问：为啥还有最后行 不是上面遍历完毕了吗？
          values.back()->to_string(os_right);
          os_right << std::endl;
        }
        //多表：join查询
        if (is_join == true)
        {
          LOG_INFO(" two table join select ");
          if (sp1 && sp2 && 0 == sp1->compare(*sp2))
          {
            os << os_left.str();
            os << os_right.str();
          }
          else
          {
            LOG_INFO(" not equal  ");
          }
        }
        else
        {
          os << os_left.str();
          os << os_right.str();
        }
      }
    }
  }
  else
  {
    LOG_INFO(" no support three table query ");
  }
}

void TupleSet::set_schema(const TupleSchema &schema)
{
  schema_ = schema;
}

const TupleSchema &TupleSet::get_schema() const
{
  return schema_;
}
bool TupleSet::is_empty() const
{
  return tuples_.empty();
}
int TupleSet::size() const
{
  return tuples_.size();
}

const Tuple &TupleSet::get(int index) const
{
  return tuples_[index];
}

const std::vector<Tuple> &TupleSet::tuples() const
{
  return tuples_;
}

/////////////////////////////////////////////////////////////////////////////
TupleRecordConverter::TupleRecordConverter(Table *table, TupleSet &tuple_set) : table_(table), tuple_set_(tuple_set)
{
}
//record:value 开始地址
//record --->显示
void TupleRecordConverter::add_record(const char *record)
{
  const TupleSchema &schema = tuple_set_.schema();
  //查询条件的表信息  可能全部列 可能部分
  Tuple tuple;
  const TableMeta &table_meta = table_->table_meta();
  for (const TupleField &field : schema.fields())
  {
    const FieldMeta *field_meta = table_meta.field(field.field_name());
    assert(field_meta != nullptr);
    int null_able = field_meta->nullable();
    switch (field_meta->type())
    {
    case INTS:
    {
      if (null_able == 1)
      {
        const char *s = record + field_meta->offset();
        if (0 == strcmp(s, "999"))
        {

          tuple.add_null_value();
        }
        else
        {
          int value = *(int *)(record + field_meta->offset());
          //LOG_INFO(" tuple add int =%d ", value);
          tuple.add(value);
        }
      }
      else
      {
        int value = *(int *)(record + field_meta->offset());
        // LOG_INFO(" tuple add int =%d ", value);
        tuple.add(value);
      }
    }
    break;
    case FLOATS:
    {
      if (null_able == 1)
      {
        //memset 改为
        const char *s = record + field_meta->offset();
        if (0 == strcmp(s, "999"))
        {
          //LOG_INFO("99999 FLOATS");
          tuple.add_null_value();
        }
        else
        {

          float value = *(float *)(record + field_meta->offset());
          //LOG_INFO(" tuple add float =%d ", value);
          tuple.add(value);
        }
      }
      else
      {
        float value = *(float *)(record + field_meta->offset());
        LOG_INFO(" tuple add float =%d ", value);
        tuple.add(value);
      }
    }
    break;
    case CHARS:
    {
      if (null_able == 1)
      {
        //memset 改为
        const char *s = record + field_meta->offset();
        if (0 == strcmp(s, "999"))
        {
          //LOG_INFO("99999 FLOATS");
          tuple.add_null_value();
        }
        else
        {
          const char *s = record + field_meta->offset(); // 现在当做Cstring来处理
                                                         // LOG_INFO(" tuple add string =%s ", s);
          tuple.add(s, strlen(s));
        }
      }
      else
      {

        const char *s = record + field_meta->offset(); // 现在当做Cstring来处理
        LOG_INFO(" tuple add string =%s ", s);
        tuple.add(s, strlen(s));
      }
    }
    break;
    case DATES:
    {
      if (null_able == 1)
      {
        //memset memcpy
        const char *s = record + field_meta->offset();
        if (0 == strcmp(s, "999"))
        {
          // LOG_INFO("99999 FLOATS");
          tuple.add_null_value();
        }
        else
        {
          int value = *(int *)(record + field_meta->offset());
          // LOG_INFO(" tuple.add_date=%d ", value);
          tuple.add_date(value);
        }
      }
      else
      {
        int value = *(int *)(record + field_meta->offset());
        // LOG_INFO(" tuple.add_date=%d ", value);
        tuple.add_date(value);
      }
    }
    break;
    case TEXTS:
    {
      if (null_able == 1)
      {
        //memset 改为
        const char *s = record + field_meta->offset();
        if (0 == strcmp(s, "999"))
        {
          //LOG_INFO("99999 FLOATS");
          tuple.add_null_value();
        }
        else
        {
          const char *s = record + field_meta->offset(); // 现在当做Cstring来处理
                                                         // LOG_INFO(" tuple add string =%s ", s);
          int key = *(int *)s;
          if (table()->pTextMap.count(key) == 1)
          {
            s = table()->pTextMap[key];
            LOG_INFO(" 题目 超长文本 >>>>>>>>>>>> key=%d,value=%s ",key,s);
          }
          else
          {
            LOG_INFO(" 题目 超长文本 失败 失败 失败 失败  =%s ", s);
          }
          tuple.add_text(s, strlen(s));
        }
      }
      else
      {

        const char *s = record + field_meta->offset(); // 现在当做Cstring来处理
        int key = *(int *)s;
        if (table()->pTextMap.count(key) == 1)
        {
          s = table()->pTextMap[key];
          LOG_INFO(" 题目 超长文本 >>>>>>>>>>>> key=%d,value=%s ",key,s);
        }
        else
        {
          LOG_INFO(" 题目 超长文本 失败 失败 失败 失败 key=%d,value=%s ",key,s);
        }
        tuple.add(s, strlen(s));
      }
    }
    break;
    default:
    {
      LOG_PANIC("Unsupported field type. type=%d", field_meta->type());
    }
    }
  }

  tuple_set_.add(std::move(tuple));
}

//聚合
void TupleSchema::from_table_first(const Table *table, TupleSchema &schema, FunctionType functiontype)
{
  const char *table_name = table->name();            //表名字
  const TableMeta &table_meta = table->table_meta(); //表结构
                                                     //const int field_num = table_meta.field_num();      //字段个数

  const FieldMeta *field_meta = table_meta.field(1);
  if (field_meta && field_meta->visible())
  {
    schema.add(field_meta->type(), table_name, field_meta->name(), functiontype, field_meta->nullable());
  }
}
void TupleSchema::add(AttrType type, const char *table_name, const char *field_name, FunctionType functiontype)
{
  fields_.emplace_back(type, table_name, field_name, functiontype);
}
void TupleSchema::add(AttrType type, const char *table_name, const char *field_name, FunctionType functiontype, int nullable)
{
  fields_.emplace_back(type, table_name, field_name, functiontype, nullable);
}

bool TupleSet::avg_print(std::ostream &os) const
{
  //步骤
  //1. 遍历 属性
  //2. 根据不同属性函数,做不同的计算.
  //3. 返回是存在聚合
  bool isWindows = false;

  const std::vector<TupleField> &fields = schema_.fields();
  int count = fields.size();
  int index = 0;
  //遍历n个元素.
  for (std::vector<TupleField>::const_iterator iter = fields.begin(), end = fields.end();
       iter != end; ++iter)
  {
    FunctionType window_function = iter->get_function_type();
    if (FunctionType::FUN_COUNT_ALL_ALl == window_function || FunctionType::FUN_COUNT_ALL == window_function || FunctionType::FUN_COUNT == window_function)
    {
      isWindows = true;
      int count = 0;
      //count(*)
      if (FunctionType::FUN_COUNT_ALL_ALl == window_function || FunctionType::FUN_COUNT_ALL == window_function)
      {
        count = tuples_.size();
      }
      else
      {
        //rows count(id)
        //字段值是NULL时，比较特殊，不需要统计在内。如果是AVG，不会增加统计行数，也不需要默认值。
        for (const Tuple &item : tuples_)
        {
          int colIndex = 0;
          bool null_able = true;
          //cols
          const std::vector<std::shared_ptr<TupleValue>> &values = item.values();
          for (std::vector<std::shared_ptr<TupleValue>>::const_iterator iter = values.begin(), end = values.end();
               iter != end; ++iter)
          {
            if (colIndex == index)
            {
              std::shared_ptr<TupleValue> temp = *iter;
              if (AttrType::NULLVALUES == temp->get_type())
              {
                null_able = false;
              }
              break;
            }
            colIndex++;
          }

          if (true == null_able)
          {
            count++;
          }
        }
      } //end else
      os << count;
    }
    else if (FunctionType::FUN_MAX == window_function)
    {
      //属性值类型 typedef enum { UNDEFINED, CHARS, INTS, FLOATS,DATES } AttrType;
      isWindows = true;
      std::shared_ptr<TupleValue> maxValue;

      if (0 == tuples_.size())
      {
        return true;
      }
      for (const Tuple &item : tuples_)
      {
        int colIndex = 0;
        //第n-1列
        const std::vector<std::shared_ptr<TupleValue>> &values = item.values();
        if (0 == values.size())
        {
          continue;
        }
        for (std::vector<std::shared_ptr<TupleValue>>::const_iterator iter = values.begin(), end = values.end();
             iter != end; ++iter)
        {
          //(*iter)->to_string(os);
          if (colIndex == index)
          {
            std::shared_ptr<TupleValue> temp = *iter;

            //if (AttrType::NULLVALUES == temp->get_type())
            //{
            //不处理
            //}
            //else
            {
              if (nullptr == maxValue)
              {
                maxValue = temp;
              }
              else
              {
                if (maxValue->compare(*temp) < 0)
                {
                  maxValue = temp;
                }
              }
            }

            break; //get
          }
          colIndex++;
        }
      } //end
      maxValue->to_string(os);
    }
    else if (FunctionType::FUN_MIN == window_function)
    {
      //属性值类型 typedef enum { UNDEFINED, CHARS, INTS, FLOATS,DATES } AttrType;
      isWindows = true;
      std::shared_ptr<TupleValue> minValue;

      if (0 == tuples_.size())
      {
        return true;
      }
      for (const Tuple &item : tuples_)
      {
        int colIndex = 0;

        //列
        const std::vector<std::shared_ptr<TupleValue>> &values = item.values();
        if (0 == values.size())
        {
          continue;
        }
        for (std::vector<std::shared_ptr<TupleValue>>::const_iterator iter = values.begin(), end = values.end();
             iter != end; ++iter)
        {
          //(*iter)->to_string(os);
          if (colIndex == index)
          {
            std::shared_ptr<TupleValue> temp = *iter;

            //if (AttrType::NULLVALUES == temp->get_type())
            //{
            //不处理
            //}
            //else
            {
              if (nullptr == minValue)
              {
                minValue = *iter;
              }
              else
              {
                std::shared_ptr<TupleValue> temp = *iter;
                if (minValue->compare(*temp) > 0)
                {
                  minValue = temp;
                }
              }
            }

            break; //get
          }
          colIndex++;
        }
      } //end
      minValue->to_string(os);
    }
    else if (FunctionType::FUN_AVG == window_function)
    {
      //属性值类型 typedef enum { UNDEFINED, CHARS, INTS, FLOATS,DATES } AttrType;
      isWindows = true;

      std::shared_ptr<TupleValue> sumValue;
      int count = 0;
      bool exits_null_value = false;
      if (0 == tuples_.size())
      {
        return true;
      }
      for (const Tuple &item : tuples_)
      {
        int colIndex = 0;
        bool null_able = true;
        //第n列
        const std::vector<std::shared_ptr<TupleValue>> &values = item.values();
        if (0 == values.size())
        {
          continue;
        }

        for (std::vector<std::shared_ptr<TupleValue>>::const_iterator iter = values.begin(), end = values.end();
             iter != end; ++iter)
        {
          //(*iter)->to_string(os);
          if (colIndex == index)
          {
            std::shared_ptr<TupleValue> temp = *iter;
            if (AttrType::NULLVALUES == temp->get_type())
            {
              //不处理
              null_able = false;
              exits_null_value = true;
            }
            else
            {
              if (nullptr == sumValue)
              {
                sumValue = temp;
              }
              else
              {
                sumValue->add_value(*temp);
              }
            }

            break; //get
          }
          colIndex++;
        }
        if (true == null_able)
        {
          count++;
        }
      } //end
      //防溢出求平均算法

      if (0 == count)
      {
        if (exits_null_value == true)
        {
          os << "NULL";
          os << std::endl;
        }

        return true; //是聚合运算
      }

      sumValue->to_avg(count, os);
    }

    //聚合函数显示
    if (FunctionType::FUN_AVG == window_function ||
        FunctionType::FUN_COUNT == window_function ||
        FunctionType::FUN_COUNT_ALL == window_function ||
        FunctionType::FUN_COUNT_ALL_ALl == window_function ||
        FunctionType::FUN_MIN == window_function ||
        FunctionType::FUN_MAX == window_function)
    {
      if (index == count - 1)
      {
        //os << result.c_str();
        os << std::endl;
      }
      else
      {
        //os << result.c_str();
        os << " | ";
      }
    }

    index++;
  } //const std::vector<TupleField> &fields = schema_.fields();

  return isWindows;
}

void TupleSchema::add_if_not_exists_visible(AttrType type, const char *table_name, const char *field_name, bool visible)
{
  for (const auto &field : fields_)
  {
    if (0 == strcmp(field.table_name(), table_name) &&
        0 == strcmp(field.field_name(), field_name))
    {
      LOG_INFO(">>>>>>>>>>add_if_exists. %s.%s", table_name, field_name);
      return;
    }
  }

  add(type, table_name, field_name, visible);
}

//2个表的join操作
void TupleSet::print_two(std::ostream &os)
{
  //列信息: (type_ = INTS, table_name_ = "t1", field_name_ = "id")
  if (schema_.fields().empty())
  {
    LOG_WARN("Got empty schema");
    return;
  }
  if (old_schema.get_size() > 0)
  {
    old_schema.realTabeNumber = 2;
    //修改old_schema成员变量， 去掉const函数
    old_schema.print(os);
    // 原始查询条件
    //select t2.age from t1 ,t2 where t1.age=21;
  }
  else
  {
    schema_.print(os); //打印 列字段 （已经考虑到多个表）
  }

  // 判断有多张表还是只有一张表
  std::set<std::string> table_names;
  for (const auto &field : schema_.fields())
  {
    table_names.insert(field.table_name());
  }
  if (2 != table_names.size())
  {
    return;
  }

  ////多表操作/////////////////////////////////////
  //笛卡尔积算法描述

  if (tuples1_.size() == 0 || tuples2_.size() == 0)
  {
    return;
  }

  //select t1.age,t2.age from t1 ,t2 where t1.id =t2.id;
  //[id(隐藏),age] [age,id(隐藏)）

  std::map<int, bool> leftVisibleMap;
  leftVisibleMap.clear();
  int index1 = 0;
  int count1 = schema1_.get_size() - 1;
  for (const auto &field : schema1_.fields())
  {
    if (false == field.visible())
    {
      if (index1 == count1)
      {
        leftVisibleMap[index1] = true; //最后一个字段
      }
      else
      {
        leftVisibleMap[index1] = false;
      }
    }
    index1++;
  }

  std::map<int, bool> rightVisibleMap;
  rightVisibleMap.clear();
  int index2 = 0;
  int count2 = schema2_.get_size() - 1;
  for (const auto &field : schema2_.fields())
  {
    if (false == field.visible())
    {
      if (index2 == count2)
      {
        rightVisibleMap[index2] = true;
      }
      else
      {
        rightVisibleMap[index2] = false;
      }
    }
    index2++;
  }

  //t1.rows[i][j]
  //t2.rows[i][j]
  //item_left 一行记录
  for (const Tuple &item_left : tuples1_)
  {
    std::shared_ptr<TupleValue> sp1;

    vector<std::shared_ptr<TupleValue>> left(dp.size());

    //多个过滤条件
    //几个过滤条件
    //select t1.age,t1.id ,t2.id,t2.age  from t1,t2 where  t1.id=t2.id  and t1.age =t2.age;

    int col1 = 0;
    std::stringstream os_left; //第一个表的 全部行
    std::stringstream os_left1;
    std::stringstream os_left2;
    {
      //std::vector<std::shared_ptr<TupleValue>>  values_; 每一行 多个字段
      const std::vector<std::shared_ptr<TupleValue>> &values = item_left.values();
      for (std::vector<std::shared_ptr<TupleValue>>::const_iterator iter = values.begin(), end = values.end();
           iter != end; ++iter)
      {
        if (is_join == true && joins_index == col1)
        {
          sp1 = *iter;
          cout << ">>>>>>>>>>>>>join select " << endl;
        }

        if (is_join == true)
        {
          for (int i = 0; i < dp.size(); i++)
          {
            if (col1 == dp[i][0].m_index)
            {
              left[i] = *iter;
              cout << "left " << col1 << endl;
            }
          }
        }
        //一个表：是否隐藏
        if (leftVisibleMap.size() > 0 && leftVisibleMap.count(col1) == 1)
        {
        }
        else
        {
          if (b_not_know == true && col1 == 1)
          {
            (*iter)->to_string(os_left1);
            LOG_INFO("11111111111111111 left=");
          }
          else
          {
            (*iter)->to_string(os_left);
            os_left << " | ";
          }
        }

        col1++;
      }
    }

    //b表的多行 tuples_right 多行
    for (const Tuple &item_right : tuples2_)
    {
      std::shared_ptr<TupleValue> sp2;
      vector<std::shared_ptr<TupleValue>> right(dp.size());
      right.clear();
      int col2 = 0;
      std::stringstream os_right; //第二个表：一行记录
      {
        const std::vector<std::shared_ptr<TupleValue>> &values = item_right.values();
        for (std::vector<std::shared_ptr<TupleValue>>::const_iterator iter = values.begin(), end = values.end();
             iter != end; ++iter)
        {
          //笛卡尔积:查询条件
          if (is_join == true && joins_index == col2)
          {
            sp2 = *iter;
          }
          if (is_join == true)
          {
            for (int ri = 0; ri < dp.size(); ri++)
            {
              if (col2 == dp[ri][1].m_index)
              {
                right[ri] = *iter;
                cout << "right " << col2 << endl;
              }
            }
          }
          //判断是否最后一行
          if (col2 == values.size() - 1)
          {

            //一个表：是否隐藏
            if (rightVisibleMap.size() > 0 && rightVisibleMap.count(col2) == 1)
            {
              //隐藏 什么都不操作
            }
            else
            {

              if (b_not_know == true)
              {
                LOG_INFO("11111111111111111 ");
                (*iter)->to_string(os_right);
                os_right << " | ";
                os_right << os_left1.str();
                os_right << std::endl;
              }
              else
              {
                (*iter)->to_string(os_right);
                os_right << std::endl;
              }
            }
          }
          else
          {

            if (rightVisibleMap.size() > 0 && rightVisibleMap.count(col2) == 1)
            {
              //隐藏 什么都不操作
            }
            else if (rightVisibleMap.size() > 0 && rightVisibleMap.count(col2) == 0)
            {
              //自己不隐藏，下一行隐藏，下一行是最后一行
              ////age（当前）,id(隐藏)
              int next = col2 + 1;
              if (rightVisibleMap.count(next) == 1 && true == rightVisibleMap[next])
              {
                (*iter)->to_string(os_right);

                os_right << std::endl;
              }
            }
            else
            {
              (*iter)->to_string(os_right);
              os_right << " | ";
            }
          }

          col2++;
        }
      }

      //多表：有join条件
      if (is_join == true)
      {
        LOG_INFO(" two table join select ");
        bool b_equal = false; //符合条件
        //join条件全部相等
        for (int i = 0; i < dp.size(); i++)
        {

          CompOp two_comp = dp[i][0].comp;
          std::stringstream s1;
          std::stringstream s2;
          left[i]->to_string(s1);
          right[i]->to_string(s2);
          std::cout << " >>>>>>> left:" << s1.str() << "right:" << s2.str() << std::endl;
          //"=="
          if (two_comp == EQUAL_TO)
          {
            if (left[i] && right[i] && 0 == left[i]->compare(*right[i]))
            {

              b_equal = true;
            }
          }
          else if (two_comp == GREAT_EQUAL)
          {
            // ">=" t1.id >=t2.id
            if (left[i] && right[i] && left[i]->compare(*right[i]) >= 0)
            {
              b_equal = true;
            }
          }
          else if (two_comp == GREAT_THAN)
          {
            // ">"
            if (left[i] && right[i] && left[i]->compare(*right[i]) > 0)
            {
              b_equal = true;
            }
          }
          else if (two_comp == LESS_EQUAL)
          {
            // "<="
            if (left[i] && right[i] && left[i]->compare(*right[i]) <= 0)
            {
              b_equal = true;
            }
          }
          else if (two_comp == LESS_THAN)
          {
            // "<"
            if (left[i] && right[i] && left[i]->compare(*right[i]) < 0)
            {
              b_equal = true;
            }
          }
        }

        if (true == b_equal)
        {
          os << os_left.str();
          os << os_right.str();
        }

        /**
        if (sp1 && sp2 && 0 == sp1->compare(*sp2))
        {
          os << os_left.str();
          os << os_right.str();
        }
        else
        {
          LOG_INFO(" not equal  ");
        }**/
      }
      else
      { //没有join条件
        os << os_left.str();
        os << os_right.str();
      }
    }
  }
}

//聚合
void TupleSchema::from_table_first_count_number(const Table *table, TupleSchema &schema, FunctionType functiontype, const char *field_name_count_number)
{
  const char *table_name = table->name();            //表名字
  const TableMeta &table_meta = table->table_meta(); //表结构
                                                     //const int field_num = table_meta.field_num();      //字段个数

  const FieldMeta *field_meta = table_meta.field(1);
  if (field_meta && field_meta->visible())
  {
    // schema.add(field_meta->type(), table_name, field_meta->name(), functiontype);

    schema.add_number(field_meta->type(), table_name, field_meta->name(), functiontype, field_name_count_number, field_meta->nullable());
  }
}
void TupleSchema::add_number(AttrType type, const char *table_name, const char *field_name, FunctionType functiontype, const char *field_name_count_number, int nullable)
{
  TupleField temp(type, table_name, field_name, functiontype);
  temp.field_name_count_number_ = field_name_count_number;
  fields_.push_back(temp);
  //fields_.emplace_back(std::move(temp));

  //fields_.emplace_back(type, table_name, field_name, functiontype);
}

//至少3个表
void TupleSet::print_multi_table(std::ostream &os)
{
  if (schema_.fields().empty())
  {
    LOG_WARN(" print_multi_table Got empty schema");
    return;
  }

  schema_.print(os); // 如果显示列有问题 请在看这里 多个表合并一个表

  // 判断有多张表还是只有一张表
  std::set<std::string> table_names;
  for (const auto &field : schema_.fields())
  {
    table_names.insert(field.table_name());
  }
  if (3 != table_names.size())
  {
    return;
  }

  ////多表操作/////////////////////////////////////
  //笛卡尔积算法描述

  if (tuples1_.size() == 0 || tuples2_.size() == 0 || tuples3_.size() == 0)
  {
    return;
  }

  //三次循环
  for (Tuple &item_1 : tuples1_)
  {
    std::stringstream os_tuples_1; //node1
    os_tuples_1.clear();

    // std::shared_ptr<TupleValue> sp1;
    item_1.sp1.reset();
    item_1.selectComareIndex = commonIndex; //比较字段位置

    item_1.head_table_row_string(os_tuples_1, 1);

    for (Tuple &item_2 : tuples2_)
    {
      std::stringstream os_tuples_2; //node2
      os_tuples_2.clear();
      item_2.sp2.reset();
      item_2.selectComareIndex = commonIndex; //比较字段位置
      item_2.head_table_row_string(os_tuples_2, 2);

      std::stringstream os_tuples_1_2;
      os_tuples_1_2.clear();

      if (select_table_type == 2)
      {
        LOG_WARN(" >>>>>>>>>>>>>>>>>> select_table_type =2");
        if (item_1.sp1 && item_2.sp2 && 0 == item_1.sp1->compare(*item_2.sp2))
        {
          os_tuples_1_2 << os_tuples_1.str();
          os_tuples_1_2 << os_tuples_2.str();
          LOG_WARN(" >>>>>>>>>>>>>>>>>> 相等 select_table_type =2");
        }
        else
        {
          LOG_WARN(" >>>>>>>>>>>>>>>>>> 不相等 select_table_type =2");
        }
      }
      else
      {
        os_tuples_1_2 << os_tuples_1.str();
        os_tuples_1_2 << os_tuples_2.str();
      }

      for (Tuple &item_3 : tuples3_)
      {

        std::stringstream os_tuples_3; //node3
        os_tuples_3.clear();

        item_3.sp3.reset();
        item_3.selectComareIndex = commonIndex; //比较字段位置
        item_3.tail_table_row_string(os_tuples_3, 3);

        //多表：有join条件
        //select * from t1,t2,t3;
        if (false == is_join)
        {
          //没有join条件
          os << os_tuples_1_2.str();
          os << os_tuples_3.str();
        }
        else
        {
          //select_table_type
          //0 查询无过滤条件
          //1 三表完全过滤
          //2表过滤 //a ok b ok  c (no)
          //3 //a (no)  b ok  c ok
          ////a (no)  b ok  c ok
          if (select_table_type == 1)
          {
            bool b_equal = true;
            if (item_1.sp1 && item_2.sp2 && 0 == item_1.sp1->compare(*item_2.sp2))
            {
            }
            else
            {
              b_equal = false;
            }

            if (item_1.sp1 && item_3.sp3 && 0 == item_1.sp1->compare(*item_3.sp3))
            {
            }
            else
            {
              b_equal = false;
            }

            if (true == b_equal)
            {
              os << os_tuples_1_2.str();
              os << os_tuples_3.str();
            }
            //end if 1
          }
          else if (select_table_type == 2)
          {
            //2表过滤 //a ok b ok  c (no)
            //组合完毕---过滤
            bool b_equal = true;
            if (item_1.sp1 && item_2.sp2 && 0 == item_1.sp1->compare(*item_2.sp2))
            {
            }
            else
            {
              b_equal = false;
            }

            if (true == b_equal)
            {
              os << os_tuples_1_2.str();
              os << os_tuples_3.str();
            }
          } //end select_table_type == 2
          else if (select_table_type == 3)
          {
            //3 //a (no)  b ok  c ok
            bool b_equal = true;
            //组合完毕---过滤
            if (item_2.sp2 && item_3.sp3 && 0 == item_2.sp2->compare(*item_3.sp3))
            {
            }
            else
            {
              b_equal = false;
            }

            if (true == b_equal)
            {
              os << os_tuples_1_2.str();
              os << os_tuples_3.str();
            }
            ////end select_table_type == 3
          }
          else if (4 == select_table_type)
          {
            //a (ok)  b (no)  c ok
            //组合完毕---过滤
            bool b_equal = true;
            if (item_1.sp1 && item_3.sp3 && 0 == item_1.sp1->compare(*item_3.sp3))
            {
            }
            else
            {
              b_equal = false;
            }
            if (true == b_equal)
            {
              os << os_tuples_1_2.str();
              os << os_tuples_3.str();
            }
          }

        } //end else
      }
    }
  }
}

void Tuple::head_table_row_string(std::ostream &os)
{

  const std::vector<std::shared_ptr<TupleValue>> &values = this->values();
  //int cols =0;
  for (std::vector<std::shared_ptr<TupleValue>>::const_iterator iter = values.begin(), end = values.end();
       iter != end; ++iter)
  {
    (*iter)->to_string(os);
    os << " | ";

    //if(cols ==selectComareIndex)
    //{
    // sp1 = *iter;
    //}
    //cols++;
  }
}

void Tuple::tail_table_row_string(std::ostream &os)
{
  size_t col2 = 0;
  const std::vector<std::shared_ptr<TupleValue>> &values = this->values();
  for (std::vector<std::shared_ptr<TupleValue>>::const_iterator iter = values.begin(), end = values.end();
       iter != end; ++iter)
  {
    //判断是否最后一行
    if (col2 == values.size() - 1)
    {

      (*iter)->to_string(os);
      os << std::endl;
    }
    else
    {

      (*iter)->to_string(os);
      os << " | ";
    }

    col2++;
  }
}

void Tuple::head_table_row_string(std::ostream &os, int type)
{
  const std::vector<std::shared_ptr<TupleValue>> &values = this->values();
  int cols = 0;
  for (std::vector<std::shared_ptr<TupleValue>>::const_iterator iter = values.begin(), end = values.end();
       iter != end; ++iter)
  {
    (*iter)->to_string(os);
    os << " | ";

    //如果多个查询条件，考虑 需要更多考虑
    if (cols == selectComareIndex)
    {
      if (1 == type)
      {
        sp1 = *iter;
      }
      else if (2 == type)
      {
        sp2 = *iter;
      }
      else if (3 == type)
      {
        sp3 = *iter;
      }
    }
    cols++;
  }
}

void Tuple::tail_table_row_string(std::ostream &os, int type)
{
  size_t col2 = 0;
  const std::vector<std::shared_ptr<TupleValue>> &values = this->values();
  for (std::vector<std::shared_ptr<TupleValue>>::const_iterator iter = values.begin(), end = values.end();
       iter != end; ++iter)
  {
    //判断是否最后一行
    if (col2 == values.size() - 1)
    {

      (*iter)->to_string(os);
      os << std::endl;
    }
    else
    {

      (*iter)->to_string(os);
      os << " | ";
    }
    //如果多个查询条件，考虑 需要更多考虑
    if (col2 == selectComareIndex)
    {
      if (1 == type)
      {
        sp1 = *iter;
      }
      else if (2 == type)
      {
        sp2 = *iter;
      }
      else if (3 == type)
      {
        sp3 = *iter;
      }
    }
    col2++;
  }
}