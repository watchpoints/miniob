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

class TupleValue
{
public:
  TupleValue() = default;
  virtual ~TupleValue() = default;

  virtual void to_string(std::ostream &os) const = 0;
  virtual int compare(const TupleValue &other) const = 0;

private:
};

class IntValue : public TupleValue
{
public:
  explicit IntValue(int value) : value_(value)
  {
  }

  void to_string(std::ostream &os) const override
  {
    std::cout << "IntValue:value_" << value_ << std::endl;
    os << value_;
  }

  int compare(const TupleValue &other) const override
  {
    const IntValue &int_other = (const IntValue &)other;
    std::cout << " >>> compare::IntValue " << value_ << ":" << int_other.value_ << std::endl;

    return value_ - int_other.value_;
  }

private:
  int value_;
};
class FloatValue : public TupleValue
{
public:
  explicit FloatValue(float value) : value_(value)
  {

  }
  int compare(const TupleValue &other) const override
  {
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
      //按照输出要求，浮点数最多保留两位小数，并且去掉多余的0
      std::cout << "print float" << value_ << std::endl;
      os <<  value_;
    
  }
private:
  float value_;
};

class StringValue : public TupleValue
{
public:
  StringValue(const char *value, int len) : value_(value, len)
  {
  }
  explicit StringValue(const char *value) : value_(value)
  {
  }
  /**
   *  日期格式显示：
   *  //01 判断字符串是否符合日期格式
   *  //02 如果是日期格式进行格式化显示
   */
  void to_string(std::ostream &os) const override
  {
    //01 判断字符串是否符合日期格式
    const char *pattern = "[0-9]{4}-[0-9]{1,2}-[0-9]{1,2}";
    if (0 == common::regex_match(value_.c_str(), pattern))
    {
      //02 如果是日期格式进行格式化显示
      int len = value_.size();
      std::cout<< "2222222=>>>>>>>>>>>>>"<<len<< std::endl;
      char mydate[len];
      memcpy(mydate, value_.c_str(), len);

      char *p = nullptr;
      const char *split = "-"; //可按多个字符来分割
      p = strtok(mydate, split);
      int count = 0;
      int year = 0;
      int month = 0;
      int day = 0;
      while (p)
      {
        int data = atoi(p);
        // cout<< "data=" <<data;
        if (count == 0)
        {
          year = data;
        }
        else if (count == 1)
        {
          month = data;
        }
        else if (count == 2)
        {
          day = data;
        }
        count++;
        p = strtok(NULL, split);
      }
      char rightdate[len];
      sprintf(rightdate, "%04d-%02d-%02d", year, month, day);
      std::cout << "yyyy-mm-dd-to_string=========" << rightdate << std::endl;
      os << rightdate;
    }
    else
    {
      os << value_;
      std::cout << "print commn stirng  no date " << value_ << std::endl;
    }
  }

  int compare(const TupleValue &other) const override
  {
    const StringValue &string_other = (const StringValue &)other;
    std::cout << " >>> compare::StringValue " << value_.c_str() << ":" << string_other.value_.c_str();
    return strcmp(value_.c_str(), string_other.value_.c_str());
  }

private:
  std::string value_;
};

#endif //__OBSERVER_SQL_EXECUTOR_VALUE_H_
