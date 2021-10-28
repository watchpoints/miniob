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
#include<sstream>
#include<iostream>
using namespace std;
class Table;

class Tuple {
public:
  Tuple() = default;

  Tuple(const Tuple &other);

  ~Tuple();

  Tuple(Tuple &&other) noexcept ;
  Tuple & operator=(Tuple &&other) noexcept ;

  void add(TupleValue *value);
  void add(const std::shared_ptr<TupleValue> &other);
  void add(int value);
  void add(float value);
  void add(const char *s, int len);
  void add_date(int value);

  const std::vector<std::shared_ptr<TupleValue>> &values() const {
    return values_;
  }

  int size() const {
    return values_.size();
  }

  const TupleValue &get(int index) const {
    return *values_[index];
  }

  const std::shared_ptr<TupleValue> &get_pointer(int index) const {
    return values_[index];
  }

private:
  std::vector<std::shared_ptr<TupleValue>>  values_;
};

class TupleField {
public:
  TupleField(AttrType type, const char *table_name, const char *field_name) :
          type_(type), table_name_(table_name), field_name_(field_name){
            isShow_ =true;
  }

  AttrType  type() const{
    return type_;
  }

  const char *table_name() const {
    return table_name_.c_str();
  }
  const char *field_name() const {
    return field_name_.c_str();
  }

  std::string to_string() const;
  bool get_show() const {
    return isShow_;
  }
  void set_show(bool show)  {
    isShow_ =show;
  }
private:
  AttrType  type_;
  std::string table_name_;
  std::string field_name_;
  bool isShow_;
};

class TupleSchema {
public:
  TupleSchema() = default;
  ~TupleSchema() = default;

  void add(AttrType type, const char *table_name, const char *field_name);
  void add_if_not_exists(AttrType type, const char *table_name, const char *field_name);
  // void merge(const TupleSchema &other);
  void append(const TupleSchema &other);

  const std::vector<TupleField> &fields() const {
    return fields_;
  }
  
   std::vector<TupleField> &fields_two()  {
    return fields_;
  }
  const TupleField &field(int index) const {
    return fields_[index];
  }

  int index_of_field(const char *table_name, const char *field_name) const;
  void clear() {
    fields_.clear();
  }

  void print(std::ostream &os) const;
public:
  static void from_table(const Table *table, TupleSchema &schema);
private:
  std::vector<TupleField> fields_;
};

class TupleSet {
public:
  TupleSet() = default;
  TupleSet(TupleSet &&other);
  explicit TupleSet(const TupleSchema &schema) : schema_(schema) {
  }
  TupleSet &operator =(TupleSet &&other);

  ~TupleSet() = default;

  void set_schema(const TupleSchema &schema);

  const TupleSchema &get_schema() const;

  void add(Tuple && tuple);

  void clear();

  bool is_empty() const;
  int size() const;
  const Tuple &get(int index) const;
  const std::vector<Tuple> &tuples() const;

  void print(std::ostream &os) const;
  void add_tuple_schema(const TupleSchema &schema)
  {
    schema_.append(schema);
  }
  void add_tuple_value(std::vector<Tuple>& other)
  {
    tuples_.reserve(tuples_.size() + other.size());
    for (const auto &tuple: other) {
    tuples_.emplace_back(tuple);
    }

  }

public:
  const TupleSchema &schema() const {
    return schema_;
  }
  TupleSchema &schema_two()  {
    return schema_;
  }
  std::vector<Tuple> &get_tuple() 
  {
    return tuples_;
  }
  std::vector<Tuple> &get_tuples_left() 
  {
    return tuples_left;
  }
  std::vector<Tuple> &get_tuples_right() 
  {
    return tuples_right;
  }

  void set_tuples_left( std::vector<Tuple>&& left) 
  {
     tuples_left =std::move(left);
     
  }
  void set_tuples_right(std::vector<Tuple>&& right) 
  {
     tuples_right =std::move(right);
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
  
  void set_join(bool join,int index)
  {
    is_join = join;
    joins_index =index;
  }
 
private:
  std::vector<Tuple> tuples_;
  std::vector<Tuple> tuples_left; //join
  std::vector<Tuple> tuples_right; //join
  //std::vector<DefaultConditionFilter*> filter_;
  TupleSchema schema_; 
  //列信息: schema_ (type_ = INTS, table_name_ = "t1", field_name_ = "id")
  bool is_join;
  int joins_index;

};

class TupleRecordConverter {
public:
  TupleRecordConverter(Table *table, TupleSet &tuple_set);

  void add_record(const char *record);
private:
  Table *table_;
  TupleSet &tuple_set_;
};

#endif //__OBSERVER_SQL_EXECUTOR_TUPLE_H_
