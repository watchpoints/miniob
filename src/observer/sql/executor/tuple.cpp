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

Tuple::Tuple(const Tuple &other)
{
  LOG_PANIC("Copy constructor of tuple is not supported");
  exit(1);
}

Tuple::Tuple(Tuple &&other) noexcept : values_(std::move(other.values_))
{
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
  add(new IntValue(value));
}

void Tuple::add(float value)
{
  add(new FloatValue(value));
}
//按照列的名字，添加 values
void Tuple::add(const char *s, int len)
{
  add(new StringValue(s, len));
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
      schema.add(field_meta->type(), table_name, field_meta->name());
    }
  }
}

void TupleSchema::add(AttrType type, const char *table_name, const char *field_name)
{
  fields_.emplace_back(type, table_name, field_name);
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
}

/////////////////////////////////////////////////////////////////////////////
TupleSet::TupleSet(TupleSet &&other) : tuples_(std::move(other.tuples_)), schema_(other.schema_)
{
  other.schema_.clear();
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

  tuples_.clear();
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
}

void TupleSet::print(std::ostream &os) const
{
  //列信息: (type_ = INTS, table_name_ = "t1", field_name_ = "id")
  if (schema_.fields().empty())
  {
    LOG_WARN("Got empty schema");
    return;
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
    //tuples_ 多行

    for (const Tuple &item : tuples_)
    {
      //std::vector<std::shared_ptr<TupleValue>>  values_;
      const std::vector<std::shared_ptr<TupleValue>> &values = item.values();
      for (std::vector<std::shared_ptr<TupleValue>>::const_iterator iter = values.begin(), end = --values.end();
           iter != end; ++iter)
      {
        (*iter)->to_string(os);
        os << " | ";
      }
      //小王疑问：为啥还有最后行 不是上面遍历完毕了吗？
      values.back()->to_string(os);
      os << std::endl;
    }
  }
  else if (table_names.size() == 2)
  {
    //实现2个表的 笛卡尔积
    if (tuples_left.size() == 0 || tuples_right.size() == 0)
    {
      return;
    }
     // join字段类型是什么
    
    //a表的多行 tuples_left 多行
    
    for (const Tuple &item_left : tuples_left)
    { 
      std::shared_ptr<TupleValue> sp1;
      int col1 =0;
      std::stringstream os_left;

      {
        //std::vector<std::shared_ptr<TupleValue>>  values_; 每一行 多个字段
        const std::vector<std::shared_ptr<TupleValue>> &values = item_left.values();
        for (std::vector<std::shared_ptr<TupleValue>>::const_iterator iter = values.begin(), end = values.end();
             iter != end; ++iter)
        {  
          if(is_join ==true && joins_index == col1)
          {
            sp1 =*iter;
            cout<< ">>>>>>>>>>>>>join select " <<endl;
          }

          (*iter)->to_string(os_left);
          os_left << " | ";
          
          col1++;
        }
      }

      //b表的多行 tuples_right 多行
      
      for (const Tuple &item_right : tuples_right)
      {  
        std::shared_ptr<TupleValue> sp2; 
        int col2 =0;
        std::stringstream os_right; 
        {
          const std::vector<std::shared_ptr<TupleValue>> &values = item_right.values();
          for (std::vector<std::shared_ptr<TupleValue>>::const_iterator iter = values.begin(), end = --values.end();
               iter != end; ++iter)
          {
            //笛卡尔积:查询条件
            if(is_join ==true && joins_index == col2)
            {
              sp2 =*iter;
              cout<< ">>>>>>>>>>>>>join select " <<endl;
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
        if(is_join ==true )
        {   
          LOG_INFO(" two table join select ");
          if (sp1 && sp2 && 0 ==sp1->compare(*sp2))
          {
             os << os_left.str();
             os << os_right.str();
             
          }else
          {
            LOG_INFO(" not equal  ");
          }
        }else
        {
          os << os_left.str();
          os << os_right.str();
        }
      
      }
    }
  }else
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
    switch (field_meta->type())
    {
    case INTS:
    {
      int value = *(int *)(record + field_meta->offset());
      tuple.add(value);
      LOG_INFO(" tuple add_record INTS,table =%s,name=%s,value=%d", table_meta.name(), field.field_name(), value);
    }
    break;
    case FLOATS:
    {
      float value = *(float *)(record + field_meta->offset());
      tuple.add(value);
      LOG_INFO(" tuple add_record FLOATS,table =%s,name=%s,value=%d", table_meta.name(), field.field_name(), value);
    }
    break;
    case CHARS:
    {
      const char *s = record + field_meta->offset(); // 现在当做Cstring来处理
      tuple.add(s, strlen(s));
      LOG_INFO(" tuple add_record table =%s,type=%d,name=%s,value=%s,len=%d", table_meta.name(), field_meta->type(), field.field_name(), s, strlen(s));
    }
    break;
    case DATES:
    {
      const char *s = record + field_meta->offset(); // 现在当做Cstring来处理
      tuple.add(s, strlen(s));
      LOG_INFO(" tuple add_record table =%s,type=%d,name=%s,value=%s,len=%d", table_meta.name(), field_meta->type(), field.field_name(), s, strlen(s));
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
