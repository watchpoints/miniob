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

#ifndef __OBSERVER_SQL_EXECUTOR_TUPLE_H_
#define __OBSERVER_SQL_EXECUTOR_TUPLE_H_

#include <memory>
#include <vector>

#include "sql/parser/parse.h"
#include "sql/executor/value.h"
#include <sstream>
#include <iostream>
using namespace std;
class Table;

class FilterField
{
public:
  FilterField()
  {
    m_index = -1;
    m_table_name = nullptr;
    m_col_name = nullptr;
    comp=CompOp::NO_OP;
  }

public:
  std::shared_ptr<TupleValue> m_value;
  int m_index;
  char *m_table_name;
  char *m_col_name;
  CompOp comp;         // comparison operator
};
class Tuple
{
public:
  Tuple() 
  {
    selectComareIndex =-1;
  }

  Tuple(const Tuple &other);

  ~Tuple();

  Tuple(Tuple &&other) noexcept;
  Tuple &operator=(Tuple &&other) noexcept;

  void add(TupleValue *value);
  void add(const std::shared_ptr<TupleValue> &other);
  void add(int value);
  void add(float value);
  void add(const char *s, int len);
  void add_date(int value);
  void add_text(const char *s, int len);
  void add_null_value();

  const std::vector<std::shared_ptr<TupleValue>> &values() const
  {
    return values_;
  }

  int size() const
  {
    return values_.size();
  }

  const TupleValue &get(int index) const
  {
    return *values_[index];
  }

  const std::shared_ptr<TupleValue> &get_pointer(int index) const
  {
    return values_[index];
  }
  //t1| t2 |t3
  void head_table_row_string(std::ostream &os) ;
  void tail_table_row_string(std::ostream &os) ;
  void head_table_row_string(std::ostream &os,int type) ;
  void tail_table_row_string(std::ostream &os,int type) ;

private:
  std::vector<std::shared_ptr<TupleValue>> values_;
public:

std::shared_ptr<TupleValue> sp1;
std::shared_ptr<TupleValue> sp2;
std::shared_ptr<TupleValue> sp3;
int selectComareIndex;
 

};

class TupleField
{
public:
  TupleField(AttrType type, const char *table_name, const char *field_name) : type_(type), table_name_(table_name), field_name_(field_name)
  {
    visible_ = true; //显示
    function_type = FunctionType::FUN_NO;
    nullable_ =0;//不允许 插入null值
  }
  TupleField(AttrType type, const char *table_name, const char *field_name,int nullable) : type_(type), table_name_(table_name), field_name_(field_name)
  {
    visible_ = true; //显示
    function_type = FunctionType::FUN_NO;
    nullable_ =nullable;//0 不允许 插入null值 1 允许
  }
  TupleField(AttrType type, const char *table_name, const char *field_name, FunctionType functiontype) : type_(type), table_name_(table_name), field_name_(field_name)
  {
    visible_ = true;
    function_type = functiontype;
    nullable_ =0;//不允许插入null值
  }
  TupleField(AttrType type, const char *table_name, const char *field_name, bool visible) : type_(type), table_name_(table_name), field_name_(field_name)
  {
    visible_ = visible;
    function_type = FunctionType::FUN_NO;
    nullable_ =0;//不允许 入null值
  }

  //最后统一用这个
  TupleField(AttrType type, const char *table_name, const char *field_name, FunctionType functiontype,int nullable) : type_(type), table_name_(table_name), field_name_(field_name)
  {
    visible_ = true;
    function_type = functiontype;
    nullable_ =nullable;//不允许插入null值
  }
  AttrType type() const
  {
    return type_;
  }

  const char *table_name() const
  {
    return table_name_.c_str();
  }
  const char *field_name() const
  {
    return field_name_.c_str();
  }

  std::string to_string() const;
  bool visible() const
  {
    return visible_;
  }
  void set_visible(bool show)
  {
    visible_ = show;
  }
  FunctionType get_function_type() const
  {
    return function_type;
  }
  const char *field_name_count_number() const
  {
    return field_name_count_number_.c_str();
  }
  int nullable() const
  {
    return nullable_;
  }

private:
  AttrType type_;          //
  std::string table_name_; //t
  std::string field_name_; //id
  bool visible_;
  FunctionType function_type; //函数
  int nullable_;
public:
  std::string field_name_count_number_; //count(1)
};

class TupleSchema
{
public:
  TupleSchema() { realTabeNumber = -1; };
  ~TupleSchema() { realTabeNumber = -1; };
  //统一一个函数实现 下面5个太多了。
  void add(AttrType type, const char *table_name, const char *field_name);
  void add(AttrType type, const char *table_name, const char *field_name,int nullable);
  void add(AttrType type, const char *table_name, const char *field_name, FunctionType functiontype);
  void add(AttrType type, const char *table_name, const char *field_name, bool visible);
  void add(AttrType type, const char *table_name, const char *field_name, FunctionType functiontype,int nullable);

  void add_number(AttrType type, const char *table_name, const char *field_name, FunctionType functiontype, const char *field_name_count_number,int nullable);

  void add_if_not_exists(AttrType type, const char *table_name, const char *field_name);
  void add_if_not_exists1(AttrType type, const char *table_name, const char *field_name,int nullable);
  void add_if_not_exists(AttrType type, const char *table_name, const char *field_name, FunctionType ftype);
  void add_if_not_exists_visible(AttrType type, const char *table_name, const char *field_name, bool visible);

  // void merge(const TupleSchema &other);
  void append(const TupleSchema &other);

  const std::vector<TupleField> &fields() const
  {
    return fields_;
  }

  std::vector<TupleField> &fields_two()
  {
    return fields_;
  }
  const TupleField &field(int index) const
  {
    return fields_[index];
  }

  int index_of_field(const char *table_name, const char *field_name) const;
  void clear()
  {
    fields_.clear();
  }
  int size()
  {
    return fields_.size();
  }

  int get_size() const
  {
       return fields_.size();
  }

  void print(std::ostream &os) const;

public:
  static void from_table(const Table *table, TupleSchema &schema);
  static void from_table_first(const Table *table, TupleSchema &schema, FunctionType functiontype);
  static void from_table_first_count_number(const Table *table, TupleSchema &schema, FunctionType functiontype, const char *field_name_count_number);

public:
  std::vector<TupleField> fields_;

public:
  int realTabeNumber;
};

class TupleSet
{
public:
  TupleSet()
  {
    realTabeNumber = -1;
    commonIndex =-1;
    is_join = false;
    select_table_type =0;
  };
  TupleSet(TupleSet &&other);
  explicit TupleSet(const TupleSchema &schema) : schema_(schema)
  {
    realTabeNumber = -1;
    commonIndex =-1;
    is_join = false;
    select_table_type =0;
  }
  TupleSet &operator=(TupleSet &&other);

  ~TupleSet() = default;

  void set_schema(const TupleSchema &schema);

  const TupleSchema &get_schema() const;

  void add(Tuple &&tuple);

  void clear();

  bool is_empty() const;
  int size() const;
  const Tuple &get(int index) const;
  const std::vector<Tuple> &tuples() const;

  void print(std::ostream &os);

  void print_two(std::ostream &os) ;         //two
  void print_multi_table(std::ostream &os) ; //three

  bool avg_print(std::ostream &os) const;
  void order_by_two();
  void add_tuple_schema(const TupleSchema &schema)
  {
    schema_.append(schema);
  }
  void add_old_tuple_schema(const TupleSchema &schema)
  {
    old_schema.append(schema);
  }
  void add_tuple_value(std::vector<Tuple> &other)
  {
    tuples_.reserve(tuples_.size() + other.size());
    for (const auto &tuple : other)
    {
      tuples_.emplace_back(tuple);
    }
  }

public:
  const TupleSchema &schema() const
  {
    return schema_;
  }
  TupleSchema &schema_two()
  {
    return schema_;
  }
  std::vector<Tuple> &get_tuple()
  {
    return tuples_;
  }
  std::vector<Tuple> &tuples1()
  {
    return tuples1_;
  }
  std::vector<Tuple> &tuples2()
  {
    return tuples2_;
  }
  std::vector<Tuple> &tuples3()
  {
    return tuples3_;
  }

  void set_tuples1(std::vector<Tuple> &&tuples)
  {
    tuples1_ = std::move(tuples);
  }
  void set_tuples2(std::vector<Tuple> &&tuples)
  {
    tuples2_ = std::move(tuples);
  }
  void set_tuples3(std::vector<Tuple> &&tuples)
  {
    tuples3_ = std::move(tuples);
  }
  /**
  void set_filter(std::vector<DefaultConditionFilter*>&& filter) 
  {
     filter_ =std::move(filter);
  }
  std::vector<DefaultConditionFilter*>& get_filter() 
  {
     return filter_ ;
  }**/

  void set_join(bool join, int index)
  {
    is_join = join;
    joins_index = index;
  }
  void set_schema1(const TupleSchema &schema)
  {
    schema1_ = schema;
  }

  void set_schema2(const TupleSchema &schema)
  {
    schema2_ = schema;
  }

  void set_schema3(const TupleSchema &schema)
  {
    schema3_ = schema;
  }

  TupleSchema &schema1()
  {
    return schema1_;
  }

  TupleSchema &schema2()
  {
    return schema2_;
  }

  TupleSchema &schema3()
  {
    return schema3_;
  }

private:
  std::vector<Tuple> tuples_; //一个表头信息
  TupleSchema schema_;        //一个表内容信息

  bool is_join;
  int joins_index;

  //扩展：多个
  std::vector<Tuple> tuples1_; //table1
  std::vector<Tuple> tuples2_; //table2;
  std::vector<Tuple> tuples3_; //table3;
  TupleSchema schema1_;        //join 没有使用
  TupleSchema schema2_;        //join 没有使用
  TupleSchema schema3_;        //join 没有使用

  //如果有多个表，每个表头放到 schem vector<TupleSchema>
  //如果有多个表，每个表行 vector<vector<Tuple>:

  //合并操作：多个表结构合并一个表结构 ，多个内容合并一个内容。

public:
  vector<vector<FilterField>> dp;
  int realTabeNumber;
  TupleSchema old_schema; //一个表内容信息  缺失字段
  int commonIndex;
  int select_table_type ;
  bool b_not_know;

public:
 //int group_type =0; //1 order 2 group 
 //RelAttr attr_order_by;
 //RelAttr attr_group_by;
 Selects* ptr_group_selects =nullptr;


};

class TupleRecordConverter
{
public:
  TupleRecordConverter(Table *table, TupleSet &tuple_set);

  void add_record(const char *record);
  Table *table()
  {
    return table_;
  }

private:
  Table *table_;
  TupleSet &tuple_set_;
};

#endif //__OBSERVER_SQL_EXECUTOR_TUPLE_H_
