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
// Created by Wangyunlai on 2021/5/12.
//

#ifndef __OBSERVER_STORAGE_COMMON_TABLE_H__
#define __OBSERVER_STORAGE_COMMON_TABLE_H__

#include "storage/common/table_meta.h"

#include <iostream>
#include <string.h>
#include <stdio.h>
#include<map>
using namespace std;

class DiskBufferPool;
class RecordFileHandler;
class ConditionFilter;
class DefaultConditionFilter;
struct Record;
struct RID;
class Index;
class IndexScanner;
class RecordDeleter;
class RecordUpdated;
class Trx;

class   Table {
public:
  Table();
  ~Table();

  /**
   * 创建一个表
   * @param path 元数据保存的文件(完整路径)
   * @param name 表名
   * @param base_dir 表数据存放的路径
   * @param attribute_count 字段个数
   * @param attributes 字段
   */
  RC create(const char *path, const char *name, const char *base_dir, int attribute_count, const AttrInfo attributes[]);
   
    /**
   * 删除一个表
   * @param path 元数据保存的文件(完整路径)
   * @param name 表名
   * @param base_dir 表数据存放的路径
   */
  RC drop(const char *path, const char *name, const char *base_dir);
  /**
   * 打开一个表
   * @param meta_file 保存表元数据的文件完整路径
   * @param base_dir 表所在的文件夹，表记录数据文件、索引数据文件存放位置
   */
  RC open(const char *meta_file, const char *base_dir);
  
  RC insert_record(Trx *trx, int value_num, const Value *values);
  //插入多行
  RC insert_record_rows(Trx *trx, int values_length, const InsertLeft* pValues);
  RC update_record(Trx *trx, const char *attribute_name, const Value *value, int condition_num, const Condition conditions[], int *updated_count);
  RC delete_record(Trx *trx, ConditionFilter *filter, int *deleted_count);

  RC scan_record(Trx *trx, ConditionFilter *filter, int limit, void *context, void (*record_reader)(const char *data, void *context));

  RC create_index(Trx *trx, const char *index_name, const char *attribute_name);
  RC create_index_multi(Trx *trx, const char *index_name, int attr_num, char * const attributes[]);
  RC create_unique_index(Trx *trx, const char *index_name, const char *attribute_name);
  RC drop_index(Trx *trx, const char *index_name,const char *relation_name);
public:
  const char *name() const;

  const TableMeta &table_meta() const;

  RC sync();

public:
  RC commit_insert(Trx *trx, const RID &rid);
  RC commit_delete(Trx *trx, const RID &rid);
   RC commit_update(Trx *trx, const RID &rid);

  RC rollback_insert(Trx *trx, const RID &rid);
  RC rollback_delete(Trx *trx, const RID &rid);
  RC commit_update(Trx *trx, const RID &rid,const char* attribute_name,const Value* value ) ;


private:
  RC scan_record(Trx *trx, ConditionFilter *filter, int limit, void *context, RC (*record_reader)(Record *record, void *context));
  RC scan_record_by_index(Trx *trx, IndexScanner *scanner, ConditionFilter *filter, int limit, void *context, RC (*record_reader)(Record *record, void *context));
  IndexScanner *find_index_for_scan(const ConditionFilter *filter);
  IndexScanner *find_index_for_scan(const DefaultConditionFilter &filter);

  RC insert_record(Trx *trx, Record *record);
  RC delete_record(Trx *trx, Record *record);
  RC update_record(Trx *trx, Record *record);
  

private:
  friend class RecordUpdater;
  friend class RecordDeleter;
  //add by wangchuanyi
  friend class RecordUpdated;

  RC insert_entry_of_indexes(const char *record, const RID &rid);
  RC delete_entry_of_indexes(const char *record, const RID &rid, bool error_on_not_exists);
private:
  RC init_record_handler(const char *base_dir);
  RC make_record(int value_num, const Value *values, char * &record_out);
  RC make_record_text(int value_num, const Value *values, char *&record_out,char*& record_text,int& text_offset);

private:
  Index *find_index(const char *index_name) const;

private:
  std::string             base_dir_;
  TableMeta               table_meta_;
  DiskBufferPool *        data_buffer_pool_; /// 数据文件关联的buffer pool
  int                     file_id_;
  RecordFileHandler *     record_handler_;   /// 记录操作
  std::vector<Index *>    indexes_;
public:
  bool isValid_date(const char* pdata); 
public:
  //char * ptr_text =nullptr;
  std::map<int,char*> pTextMap;
};

#endif // __OBSERVER_STORAGE_COMMON_TABLE_H__