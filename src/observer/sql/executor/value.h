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

#ifndef __OBSERVER_SQL_EXECUTOR_VALUE_H_
#define __OBSERVER_SQL_EXECUTOR_VALUE_H_

#include <string.h>

#include <string>
#include <ostream>
#include <iostream>
#include "common/math/regex.h"

using namespace std;

static bool removeLastZero(char *numstr)
{
  if (NULL == strchr(numstr, '.'))
    return false;
  int length = strlen(numstr);
  for (int i = length - 1; i > 0; --i)
  {
    if ('\0' == numstr[i])
    {
      continue;
    }
    else if ('0' == numstr[i])
    {
      numstr[i] = '\0';
    }
    else if ('.' == numstr[i]) // 小数点之后全为零
    {
      numstr[i] = '\0';
      break;
    }
    else // 小数点后有非零数字
    {
      break;
    }
  }

  return true;
}

class TupleValue
{
public:
  TupleValue() = default;
  virtual ~TupleValue() = default;

  virtual void to_string(std::ostream &os) const = 0;
  virtual int compare(const TupleValue &other) const = 0;
  virtual AttrType get_type() const = 0;
  virtual void add_value(const TupleValue &other) = 0;
  virtual void to_avg(int total, std::ostream &os) = 0;
  virtual int get_null_type() const = 0;
  virtual std::string print_string() const = 0;

private:
};

class IntValue : public TupleValue
{
public:
  explicit IntValue(int value) : value_(value)
  {
    null_able_is_null = 0;
  }
  explicit IntValue(int value, int null_able) : value_(value), null_able_is_null(null_able)
  {
  }
  void to_string(std::ostream &os) const override
  {
    std::cout << "IntValue:value_" << value_ << std::endl;
    if (1 == null_able_is_null)
    {
      os << "NULL";
    }
    else
    {
      os << value_;
    }
  }
  std::string print_string() const override
  {
    if (1 == null_able_is_null)
    {
      return "NULL";
    }
    else
    {
      return std::to_string(value_);
    }
  }
  int get_null_type() const override
  {
    return null_able_is_null;
  }
  int compare(const TupleValue &other) const override
  {
    if (1 == null_able_is_null || 1 == other.get_null_type())
    {
      return -1;
    }
    const IntValue &int_other = (const IntValue &)other;
    std::cout << " >>> compare::IntValue " << value_ << ":" << int_other.value_ << std::endl;

    return value_ - int_other.value_;
  }
  AttrType get_type() const override
  {
    return AttrType::INTS;
  }

  void add_value(const TupleValue &other) override
  {

    if (1 == null_able_is_null)
    {
      return;
    }
    const IntValue &int_other = (const IntValue &)other;
    value_ += int_other.value_;
  }
  void to_avg(int total, std::ostream &os) override
  {
    if (0 == total)
    {
      return;
    }
    //防溢出求平均算法

    //判断是否能整除
    // 4/2=2;
    if (0 == value_ % total)
    {
      int temp = value_ / total;
      os << temp;
    }
    else
    {
      float a = value_;
      float b = total;
      float temp = a / b;
      char str[1024];
      sprintf(str, "%.2f", temp);

      bool ret = removeLastZero(str);
      std::cout << "to_avg: " << temp << "str:" << str << std::endl;
      if (false == ret)
      {
        os << temp;
      }
      else
      {
        // 将字符串转换为浮点数
        float num_float = std::stof(str);
        os << num_float;
      }
    }
  }

private:
  int value_;

public:
  int null_able_is_null; //1 允许为null，并且是null值
};
class FloatValue : public TupleValue
{
public:
  explicit FloatValue(float value) : value_(value)
  {
    null_able_is_null = 0;
  }

  explicit FloatValue(float value, int null_able) : value_(value), null_able_is_null(null_able)
  {
  }

  int get_null_type() const override
  {
    return null_able_is_null;
  }
  int compare(const TupleValue &other) const override
  {

    if (1 == null_able_is_null || 1 == other.get_null_type())
    {
      return -1;
    }

    const FloatValue &float_other = (const FloatValue &)other;
    float result = value_ - float_other.value_;
    std::cout << " >>> compare::FloatValue " << value_ << ":" << float_other.value_ << std::endl;

    if (result > 0)
    { // 浮点数没有考虑精度问题
      return 1;
    }
    if (result < 0)
    {
      return -1;
    }
    return 0;
  }

  void to_string(std::ostream &os) const override
  {

    if (1 == null_able_is_null)
    {
      os << "NULL";
      return;
    }

    //按照输出要求，浮点数最多保留两位小数，并且去掉多余的0
    //os << value_;
    char str[1024];
    sprintf(str, "%.2f", value_);
    bool ret = removeLastZero(str);
    std::cout << "to_avg: " << value_ << "str:" << str << std::endl;
    if (false == ret)
    {
      os << value_;
    }
    else
    {
      // 将字符串转换为浮点数
      float num_float = std::stof(str);
      os << num_float;
    }
  }
  std::string print_string() const override
  {
    if (1 == null_able_is_null)
    {
      return "NULL";
    }
    char str[1024];
    sprintf(str, "%.2f", value_);
    removeLastZero(str);
    // 将字符串转换为浮点数
    float num_float = std::stof(str);
    return std::to_string(num_float);
  }

  AttrType get_type() const override
  {
    return AttrType::FLOATS;
  }

  void add_value(const TupleValue &other) override
  {
    if (1 == null_able_is_null)
    {
      return;
    }
    const FloatValue &float_other = (const FloatValue &)other;
    value_ += float_other.value_;
  }
  void to_avg(int total, std::ostream &os) override
  {
    if (0 == total)
    {
      return;
    }

    float temp = value_ / total;
    char str[1024];
    sprintf(str, "%.2f", temp);

    bool ret = removeLastZero(str);
    std::cout << "to_avg: " << temp << "str:" << str << std::endl;

    if (false == ret)
    {
      os << temp;
    }
    else
    {
      // 将字符串转换为浮点数
      float num_float = std::stof(str);
      os << num_float;
    }
  }

private:
  float value_;

public:
  int null_able_is_null; //1 允许为null，并且是null值
};

class StringValue : public TupleValue
{

public:
  StringValue(const char *value, int len) : value_(value, len)
  {
    null_able_is_null = 0;
  }
  StringValue(const char *value) : value_(value)
  {
    null_able_is_null = 0;
  }

  StringValue(const char *value, int len, int null_able) : value_(value, len)
  {
    null_able_is_null = null_able;
  }
  /**
  StringValue(const char *value,int null_able) : value_(value)
  {
    null_able_is_null =null_able;
  }**/

  void to_string(std::ostream &os) const override
  {
    if (1 == null_able_is_null)
    {
      os << "NULL";
      return;
    }
    os << value_;
  }
  std::string print_string() const override
  {
    if (1 == null_able_is_null)
    {
      return "NULL";
    }
    return value_;
  }

  int get_null_type() const override
  {
    return null_able_is_null;
  }

  int compare(const TupleValue &other) const override
  {

    if (1 == null_able_is_null || 1 == other.get_null_type())
    {
      return -1;
    }
    const StringValue &string_other = (const StringValue &)other;
    return strcmp(value_.c_str(), string_other.value_.c_str());
  }
  AttrType get_type() const override
  {
    return AttrType::CHARS;
  }

  void add_value(const TupleValue &other) override
  {
    if (1 == null_able_is_null)
    {
      return;
    }
  }
  void to_avg(int total, std::ostream &os) override
  {
    os << "string type have no avg";
  }

private:
  std::string value_;

public:
  int null_able_is_null; //1 允许为null，并且是null值
};

class DateValue : public TupleValue
{

public:
  explicit DateValue(int value) : value_(value)
  {
    null_able_is_null = 0;
  }
  explicit DateValue(int value, int null_able) : value_(value), null_able_is_null(null_able)
  {
  }
  //日期格式输出:
  void to_string(std::ostream &os) const override
  {

    if (1 == null_able_is_null)
    {
      os << "NULL";
      return;
    }
    //std::cout << "IntValue:value_" << value_ << std::endl;
    time_t t = (time_t)value_;
    tm *tm_ = localtime(&t);    // 将time_t格式转换为tm结构体
    int year, month, day;       // 定义时间的各个int临时变量。
    year = tm_->tm_year + 1900; // 临时变量，年，由于tm结构体存储的是从1900年开始的时间，所以临时变量int为tm_year加上1900。
    month = tm_->tm_mon + 1;    // 临时变量，月，由于tm结构体的月份存储范围为0-11，所以临时变量int为tm_mon加上1。
    day = tm_->tm_mday;         // 临时变量，日。

    char rightdate[30];
    sprintf(rightdate, "%04d-%02d-%02d", year, month, day);
    cout << "to_string:data =" << rightdate << endl;
    os << rightdate;
  }

  std::string print_string() const override
  {
    if (1 == null_able_is_null)
    {
      return "NULL";
    }
    //std::cout << "IntValue:value_" << value_ << std::endl;
    time_t t = (time_t)value_;
    tm *tm_ = localtime(&t);    // 将time_t格式转换为tm结构体
    int year, month, day;       // 定义时间的各个int临时变量。
    year = tm_->tm_year + 1900; // 临时变量，年，由于tm结构体存储的是从1900年开始的时间，所以临时变量int为tm_year加上1900。
    month = tm_->tm_mon + 1;    // 临时变量，月，由于tm结构体的月份存储范围为0-11，所以临时变量int为tm_mon加上1。
    day = tm_->tm_mday;         // 临时变量，日。

    char rightdate[30];
    sprintf(rightdate, "%04d-%02d-%02d", year, month, day);
    return string(rightdate);
  }

  int get_null_type() const override
  {
    return null_able_is_null;
  }

  int compare(const TupleValue &other) const override
  {

    if (1 == null_able_is_null || 1 == other.get_null_type())
    {
      return -1;
    }
    const DateValue &int_other = (const DateValue &)other;
    std::cout << " >>> compare::DateValue " << value_ << ":" << int_other.value_ << std::endl;

    return value_ - int_other.value_;
  }

  AttrType get_type() const override
  {
    return AttrType::FLOATS;
  }

  void add_value(const TupleValue &other) override
  {
    const DateValue &int_other = (const DateValue &)other;
    value_ += int_other.value_;
  }
  void to_avg(int total, std::ostream &os) override
  {
    if (0 == total)
    {
      return;
    }
    //防溢出求平均算法

    //判断是否能整除
    // 4/2=2;
    if (0 == value_ % total)
    {
      int temp = value_ / total;
      os << temp;
    }
    else
    {
      float a = value_;
      float b = total;
      float temp = a / b;
      char str[1024];
      sprintf(str, "%.2f", temp);

      bool ret = removeLastZero(str);
      std::cout << "to_avg: " << temp << "str:" << str << std::endl;
      if (false == ret)
      {
        os << temp;
      }
      else
      {
        // 将字符串转换为浮点数
        float num_float = std::stof(str);
        os << num_float;
      }
    }
  }

private:
  int value_;

public:
  int null_able_is_null; //1 允许为null，并且是null值
};

class NullValue : public TupleValue
{
public:
  explicit NullValue()
  {
  }

public:
  void to_string(std::ostream &os) const override
  {
    os << "NULL";
  }
  int compare(const TupleValue &other) const override
  {
    return -1;
  }
  AttrType get_type() const override
  {
    return AttrType::NULLVALUES;
  }

  void add_value(const TupleValue &other) override
  {
    return;
  }
  void to_avg(int total, std::ostream &os) override
  {
    return;
  }
  int get_null_type() const override
  {
    return 1;
  }
  std::string print_string() const override
  {
    return "NULL";
  }
};

class TextValue : public TupleValue
{
public:
  TextValue(const char *value, int len) : value_(value, len)
  {
    null_able_is_null = 0;
  }
  TextValue(const char *value) : value_(value)
  {
    null_able_is_null = 0;
  }

  TextValue(const char *value, int len, int null_able) : value_(value, len)
  {
    null_able_is_null = null_able;
  }
  void to_string(std::ostream &os) const override
  {
    if (1 == null_able_is_null)
    {
      os << "NULL";
      return;
    }
    os << value_;
  }
  std::string print_string() const override
  {
    if (1 == null_able_is_null)
    {
      return "NULL";
    }
    return value_;
  }

  int get_null_type() const override
  {
    return null_able_is_null;
  }

  int compare(const TupleValue &other) const override
  {

    if (1 == null_able_is_null || 1 == other.get_null_type())
    {
      return -1;
    }
    const TextValue &string_other = (const TextValue &)other;
    return strcmp(value_.c_str(), string_other.value_.c_str());
  }
  AttrType get_type() const override
  {
    return AttrType::TEXTS;
  }

  void add_value(const TupleValue &other) override
  {
    if (1 == null_able_is_null)
    {
      return;
    }
  }
  void to_avg(int total, std::ostream &os) override
  {
    os << "string type have no avg";
  }

private:
  std::string value_;

public:
  int null_able_is_null; //1 允许为null，并且是null值
};

#endif //__OBSERVER_SQL_EXECUTOR_VALUE_H_
