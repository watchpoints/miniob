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
// Created by Wangyunlai on 2021/5/7.
//

#include <stddef.h>
#include "condition_filter.h"
#include "record_manager.h"
#include "common/log/log.h"
#include "storage/common/table.h"

using namespace common;

static time_t StringToDatetime(string str)
{
    char *cha = (char*)str.data();             // 将string转换成char*。
    tm tm_;                                    // 定义tm结构体。
    int year, month, day, hour, minute, second;// 定义时间的各个int临时变量。
    sscanf(cha, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);// 将string存储的日期时间，转换为int临时变量。
    tm_.tm_year = year - 1900;                 // 年，由于tm结构体存储的是从1900年开始的时间，所以tm_year为int临时变量减去1900。
    tm_.tm_mon = month - 1;                    // 月，由于tm结构体的月份存储范围为0-11，所以tm_mon为int临时变量减去1。
    tm_.tm_mday = day;                         // 日。
    tm_.tm_hour = hour;                        // 时。
    tm_.tm_min = minute;                       // 分。
    tm_.tm_sec = second;                       // 秒。
    tm_.tm_isdst = 0;                          // 非夏令时。
    time_t t_ = mktime(&tm_);                  // 将tm结构体转换成time_t格式。
    return t_;                                 // 返回值。
}
ConditionFilter::~ConditionFilter()
{
}

DefaultConditionFilter::DefaultConditionFilter()
{
  left_.is_attr = false;
  left_.attr_length = 0;
  left_.attr_offset = 0;
  left_.value = nullptr;

  right_.is_attr = false;
  right_.attr_length = 0;
  right_.attr_offset = 0;
  right_.value = nullptr;
}
DefaultConditionFilter::~DefaultConditionFilter()
{
}

RC DefaultConditionFilter::init(const ConDesc &left, const ConDesc &right, AttrType attr_type, CompOp comp_op)
{
  if (attr_type < CHARS || attr_type > DATES)
  {
    LOG_ERROR("Invalid condition with unsupported attribute type: %d", attr_type);
    return RC::INVALID_ARGUMENT;
  }

  if (comp_op < EQUAL_TO || comp_op >= NO_OP)
  {
    LOG_ERROR("Invalid condition with unsupported compare operation: %d", comp_op);
    return RC::INVALID_ARGUMENT;
  }

  left_ = left;
  right_ = right;
  attr_type_ = attr_type;
  comp_op_ = comp_op;
  return RC::SUCCESS;
}

RC DefaultConditionFilter::init(Table &table, const Condition &condition)
{
  const TableMeta &table_meta = table.table_meta();
  ConDesc left;
  ConDesc right;

  AttrType type_left = UNDEFINED;
  AttrType type_right = UNDEFINED;
  //1时，操作符左边是属性名
  //where id=2;
  if (1 == condition.left_is_attr)
  {
    left.is_attr = true;
    const FieldMeta *field_left = table_meta.field(condition.left_attr.attribute_name);
    if (nullptr == field_left)
    {
      LOG_WARN("No such field in condition. %s.%s", table.name(), condition.left_attr.attribute_name);
      return RC::SCHEMA_FIELD_MISSING;
    }
    left.attr_length = field_left->len();
    left.attr_offset = field_left->offset();

    left.value = nullptr;

    type_left = field_left->type();
  }
  else
  {
    //0时，是属性值
    left.is_attr = false;
    left.value = condition.left_value.data; // 校验type 或者转换类型
    type_left = condition.left_value.type;

    left.attr_length = 0;
    left.attr_offset = 0;

  }
  //1时，操作符右边是属性名，0时，是属性值
  if (1 == condition.right_is_attr)
  {
    right.is_attr = true;
    const FieldMeta *field_right = table_meta.field(condition.right_attr.attribute_name);
    if (nullptr == field_right)
    {
      LOG_WARN("No such field in condition. %s.%s", table.name(), condition.right_attr.attribute_name);
      return RC::SCHEMA_FIELD_MISSING;
    }
    right.attr_length = field_right->len();
    right.attr_offset = field_right->offset();
    type_right = field_right->type();

    right.value = nullptr;

     if (type_right == AttrType::DATES && type_left == AttrType::CHARS && left.is_attr == false)
    {  
      LOG_INFO(" check_where_date");
      if (false == check_where_date((char *)condition.left_value.data))
      {
        //LOG_INFO(" check_where_date failed,value=%s", right.value);
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }

      //字符串变成时间戳4字节存储
      //检查日期是否合法
      char *ptr = static_cast<char *>(condition.left_value.data);
      string temp(ptr);
      temp.append(" 00:00:00");
      int time_t=StringToDatetime(temp);
      //right.value = condition.right_value.data;

      left.value  = malloc(sizeof(time_t));
      memcpy(left.value, &time_t, sizeof(time_t));
      type_left =AttrType::DATES;
    }
  }
  else
  {
    right.is_attr = false;
    right.value = condition.right_value.data;
    type_right = condition.right_value.type;

    right.attr_length = 0;
    right.attr_offset = 0;

    if (type_left == AttrType::DATES && type_right == AttrType::CHARS)
    {  
      LOG_INFO(" check_where_date");
      if (false == check_where_date((char *)condition.right_value.data))
      {
        //LOG_INFO(" check_where_date failed,value=%s", right.value);
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }

      //字符串变成时间戳4字节存储
      //检查日期是否合法
      char *ptr = static_cast<char *>(condition.right_value.data);
      string temp(ptr);
      temp.append(" 00:00:00");
      int time_t=StringToDatetime(temp);
      //right.value = condition.right_value.data;

      right.value  = malloc(sizeof(time_t));
      memcpy(right.value, &time_t, sizeof(time_t));
      type_right =AttrType::DATES;
    }
  }

  // 校验和转换
  //  if (!field_type_compare_compatible_table[type_left][type_right]) {
  //    // 不能比较的两个字段， 要把信息传给客户端
  //    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  //  }
  // NOTE：这里没有实现不同类型的数据比较，比如整数跟浮点数之间的对比
  // 但是选手们还是要实现。这个功能在预选赛中会出现

  //查询条件：进行过滤
  //select * from t where birthday='2021-2-30';
  //birthday 是否非法日期类型 需要检查。

  //创建日期类型时候，自己value stirng 类型保存的 后面改成vale 类型也必须是date类型的 【遗漏任务】
  if (type_left != type_right)
  {
    LOG_INFO("init:: type_left != type_right failed type_left=%d,type_right=%d", type_left, type_right);
    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  }

  return init(left, right, type_left, condition.comp);
}

bool DefaultConditionFilter::filter(const Record &rec) const
{
  char *left_value = nullptr;
  char *right_value = nullptr;

  if (left_.is_attr)
  { // value
    left_value = (char *)(rec.data + left_.attr_offset);
  }
  else
  {
    left_value = (char *)left_.value;
  }

  if (right_.is_attr)
  {
    right_value = (char *)(rec.data + right_.attr_offset);
  }
  else
  {
    right_value = (char *)right_.value;
  }

  int cmp_result = 0;
  switch (attr_type_)
  {
  case CHARS:
  { // 字符串都是定长的，直接比较
    // 按照C字符串风格来定
    cmp_result = strcmp(left_value, right_value);
  }
  break;
  case DATES:
  { // 字符串都是定长的，直接比较
    // 没有考虑大小端问题
    // 对int和float，要考虑字节对齐问题,有些平台下直接转换可能会跪
    int left = *(int *)left_value;
    int right = *(int *)right_value;
    cmp_result = left - right;
  }
  break;
  case INTS:
  {
    // 没有考虑大小端问题
    // 对int和float，要考虑字节对齐问题,有些平台下直接转换可能会跪
    int left = *(int *)left_value;
    int right = *(int *)right_value;
    cmp_result = left - right;
  }
  break;
  case FLOATS:
  {
    float left = *(float *)left_value;
    float right = *(float *)right_value;
    cmp_result = (int)(left - right);
  }
  break;
  default:
  {
    LOG_INFO(" SCHEMA_FIELD_TYPE_MISMATCH Unsupported field type to loading: %d", attr_type_);
  }
  }

  switch (comp_op_)
  {
  case EQUAL_TO:
    return 0 == cmp_result;
  case LESS_EQUAL:
    return cmp_result <= 0;
  case NOT_EQUAL:
    return cmp_result != 0;
  case LESS_THAN:
    return cmp_result < 0;
  case GREAT_EQUAL:
    return cmp_result >= 0;
  case GREAT_THAN:
    return cmp_result > 0;

  default:
    break;
  }

  LOG_PANIC("Never should print this.");
  return cmp_result; // should not go here
}

CompositeConditionFilter::~CompositeConditionFilter()
{
  if (memory_owner_)
  {
    delete[] filters_;
    filters_ = nullptr;
  }
}

RC CompositeConditionFilter::init(const ConditionFilter *filters[], int filter_num, bool own_memory)
{
  filters_ = filters;
  filter_num_ = filter_num;
  memory_owner_ = own_memory;
  return RC::SUCCESS;
}
RC CompositeConditionFilter::init(const ConditionFilter *filters[], int filter_num)
{
  return init(filters, filter_num, false);
}

RC CompositeConditionFilter::init(Table &table, const Condition *conditions, int condition_num)
{
  if (condition_num == 0)
  {
    return RC::SUCCESS;
  }
  if (conditions == nullptr)
  {
    return RC::INVALID_ARGUMENT;
  }

  RC rc = RC::SUCCESS;
  ConditionFilter **condition_filters = new ConditionFilter *[condition_num];
  for (int i = 0; i < condition_num; i++)
  {
    DefaultConditionFilter *default_condition_filter = new DefaultConditionFilter();
    rc = default_condition_filter->init(table, conditions[i]);
    if (rc != RC::SUCCESS)
    {
      delete default_condition_filter;
      for (int j = i - 1; j >= 0; j--)
      {
        delete condition_filters[j];
        condition_filters[j] = nullptr;
      }
      delete[] condition_filters;
      condition_filters = nullptr;
      return rc;
    }
    condition_filters[i] = default_condition_filter;
  }
  return init((const ConditionFilter **)condition_filters, condition_num, true);
}

bool CompositeConditionFilter::filter(const Record &rec) const
{
  for (int i = 0; i < filter_num_; i++)
  {
    if (!filters_[i]->filter(rec))
    {
      return false;
    }
  }
  return true;
}
bool DefaultConditionFilter::check_where_date(char *date)
{
  if (nullptr == date)
  {
    return false;
  }
  const char *pattern = "[0-9]{4}-[0-9]{1,2}-[0-9]{1,2}";
  if (0 == common::regex_match(date, pattern))
  {
    //ok
  }
  else
  {
    LOG_INFO(" make_record  [0-9]{4}-[0-9]{1,2}-[0-9]{1,2}  value.data=%s", date);
    return false;
  }

  //检查日期是否合法
  char *ptr = static_cast<char *>(date);
  int len = strlen(ptr);
  char ptemp[len];
  memcpy(ptemp, ptr, len);
  Table table;
  if (false == table.check_date(ptemp))
  {
    LOG_INFO(" check_date failed  value.data=%s", date);
    return false;
  }

  return true;
}
