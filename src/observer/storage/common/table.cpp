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
// Created by Wangyunlai on 2021/5/13.
//

#include <limits.h>
#include <string.h>
#include <algorithm>

#include "storage/common/table.h"
#include "storage/common/table_meta.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "storage/default/disk_buffer_pool.h"
#include "storage/common/record_manager.h"
#include "storage/common/condition_filter.h"
#include "storage/common/meta_util.h"
#include "storage/common/index.h"
#include "storage/common/bplus_tree_index.h"
#include "storage/trx/trx.h"
#include "common/math/regex.h"

static time_t StringToDatetime(string str)
{
  char *cha = (char *)str.data();                                                 // 将string转换成char*。
  tm tm_;                                                                         // 定义tm结构体。
  int year, month, day, hour, minute, second;                                     // 定义时间的各个int临时变量。
  sscanf(cha, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second); // 将string存储的日期时间，转换为int临时变量。
  tm_.tm_year = year - 1900;                                                      // 年，由于tm结构体存储的是从1900年开始的时间，所以tm_year为int临时变量减去1900。
  tm_.tm_mon = month - 1;                                                         // 月，由于tm结构体的月份存储范围为0-11，所以tm_mon为int临时变量减去1。
  tm_.tm_mday = day;                                                              // 日。
  tm_.tm_hour = hour;                                                             // 时。
  tm_.tm_min = minute;                                                            // 分。
  tm_.tm_sec = second;                                                            // 秒。
  tm_.tm_isdst = 0;                                                               // 非夏令时。
  time_t t_ = mktime(&tm_);                                                       // 将tm结构体转换成time_t格式。
  return t_;                                                                      // 返回值。
}

Table::Table() : data_buffer_pool_(nullptr),
                 file_id_(-1),
                 record_handler_(nullptr)
{
}

Table::~Table()
{
  delete record_handler_;
  record_handler_ = nullptr;

  if (data_buffer_pool_ != nullptr && file_id_ >= 0)
  {
    data_buffer_pool_->close_file(file_id_);
    data_buffer_pool_ = nullptr;
  }

  LOG_INFO("Table has been closed: %s", name());
}

RC Table::create(const char *path, const char *name, const char *base_dir, int attribute_count, const AttrInfo attributes[])
{

  if (nullptr == name || common::is_blank(name))
  {
    LOG_WARN("Name cannot be empty");
    return RC::INVALID_ARGUMENT;
  }
  LOG_INFO("Begin to create table %s:%s", base_dir, name);

  if (attribute_count <= 0 || nullptr == attributes)
  {
    LOG_WARN("Invalid arguments. table_name=%s, attribute_count=%d, attributes=%p",
             name, attribute_count, attributes);
    return RC::INVALID_ARGUMENT;
  }

  RC rc = RC::SUCCESS;

  // 使用 table_name.table记录一个表的元数据
  // 判断表文件是否已经存在

  int fd = ::open(path, O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, 0600);
  if (-1 == fd)
  {
    if (EEXIST == errno)
    {
      LOG_ERROR("Failed to create table file, it has been created. %s, EEXIST, %s",
                path, strerror(errno));
      return RC::SCHEMA_TABLE_EXIST;
    }
    LOG_ERROR("Create table file failed. filename=%s, errmsg=%d:%s",
              path, errno, strerror(errno));
    return RC::IOERR;
  }

  close(fd);

  // 创建文件
  if ((rc = table_meta_.init(name, attribute_count, attributes)) != RC::SUCCESS)
  {
    LOG_ERROR("Failed to init table meta. name:%s, ret:%d", name, rc);
    return rc; // delete table file
  }

  std::fstream fs;
  fs.open(path, std::ios_base::out | std::ios_base::binary);
  if (!fs.is_open())
  {
    LOG_ERROR("Failed to open file for write. file name=%s, errmsg=%s", path, strerror(errno));
    return RC::IOERR;
  }

  // 记录元数据到文件中
  table_meta_.serialize(fs); //class-->json
  fs.close();

  std::string data_file = std::string(base_dir) + "/" + name + TABLE_DATA_SUFFIX;
  data_buffer_pool_ = theGlobalDiskBufferPool();
  rc = data_buffer_pool_->create_file(data_file.c_str());
  if (rc != RC::SUCCESS)
  {
    LOG_ERROR("Failed to create disk buffer pool of data file. file name=%s", data_file.c_str());
    return rc;
  }

  rc = init_record_handler(base_dir);

  base_dir_ = base_dir;
  LOG_INFO("Successfully create table %s,%s:%s", path, data_file.c_str(), name);
  return rc;
}

//
RC Table::drop(const char *path, const char *name, const char *base_dir)
{

  if (nullptr == name || common::is_blank(name))
  {
    LOG_WARN("Name cannot be empty");
    return RC::INVALID_ARGUMENT;
  }
  LOG_INFO("Begin to drop table path=%s,name=%s,base_dir=%s", path, name, base_dir);

  RC rc = RC::SUCCESS;

  //删除表元数据
  // 判断表文件是否已经存在
  int ret = remove(path);
  if (-1 == ret)
  {
    /**
      if (EEXIST == errno) {
      LOG_ERROR("Failed to create table file, it has been created. %s, EEXIST, %s",
              path, strerror(errno));
      return RC::SCHEMA_TABLE_EXIST;
      }**/

    LOG_ERROR(" drop table file failed. filename=%s, errmsg=%d:%s",
              path, errno, strerror(errno));
    return RC::IOERR;
  }

  ret = remove(base_dir);
  if (-1 == ret)
  {
    LOG_ERROR(" drop table file failed. filename=%s, errmsg=%d:%s",
              base_dir, errno, strerror(errno));
    return RC::IOERR;
  }

  return rc;
}
RC Table::open(const char *meta_file, const char *base_dir)
{
  // 加载元数据文件
  std::fstream fs;
  std::string meta_file_path = std::string(base_dir) + "/" + meta_file;
  fs.open(meta_file_path, std::ios_base::in | std::ios_base::binary);
  if (!fs.is_open())
  {
    LOG_ERROR("Failed to open meta file for read. file name=%s, errmsg=%s", meta_file, strerror(errno));
    return RC::IOERR;
  }
  if (table_meta_.deserialize(fs) < 0)
  {
    LOG_ERROR("Failed to deserialize table meta. file name=%s", meta_file);
    return RC::GENERIC_ERROR;
  }
  fs.close();

  // 加载数据文件
  RC rc = init_record_handler(base_dir);

  base_dir_ = base_dir;

  const int index_num = table_meta_.index_num();
  for (int i = 0; i < index_num; i++)
  {
    //根据索引查询
    const IndexMeta *index_meta = table_meta_.index(i);
    const FieldMeta *field_meta = table_meta_.field(index_meta->field());
    if (field_meta == nullptr)
    {
      LOG_PANIC("Found invalid index meta info which has a non-exists field. table=%s, index=%s, field=%s",
                name(), index_meta->name(), index_meta->field());
      return RC::GENERIC_ERROR;
    }

    BplusTreeIndex *index = new BplusTreeIndex();
    std::string index_file = index_data_file(base_dir, name(), index_meta->name());
    rc = index->open(index_file.c_str(), *index_meta, *field_meta);
    if (rc != RC::SUCCESS)
    {
      delete index;
      LOG_ERROR("Failed to open index. table=%s, index=%s, file=%s, rc=%d:%s",
                name(), index_meta->name(), index_file.c_str(), rc, strrc(rc));
      return rc;
    }
    indexes_.push_back(index);
  }
  return rc;
}

RC Table::commit_insert(Trx *trx, const RID &rid)
{
  Record record;
  RC rc = record_handler_->get_record(&rid, &record);
  if (rc != RC::SUCCESS)
  {
    return rc;
  }

  return trx->commit_insert(this, record);
}

RC Table::rollback_insert(Trx *trx, const RID &rid)
{

  Record record;
  RC rc = record_handler_->get_record(&rid, &record);
  if (rc != RC::SUCCESS)
  {
    return rc;
  }

  // remove all indexes
  rc = delete_entry_of_indexes(record.data, rid, false);
  if (rc != RC::SUCCESS)
  {
    LOG_ERROR("Failed to delete indexes of record(rid=%d.%d) while rollback insert, rc=%d:%s",
              rid.page_num, rid.slot_num, rc, strrc(rc));
  }
  else
  {
    rc = record_handler_->delete_record(&rid);
  }
  return rc;
}

RC Table::insert_record(Trx *trx, Record *record)
{
  RC rc = RC::SUCCESS;

  if (trx != nullptr)
  {
    trx->init_trx_info(this, *record);
  }

  LOG_INFO("题目：超长字段text insert_record,record_size =%d",table_meta_.record_size());
  rc = record_handler_->insert_record(record->data, table_meta_.record_size(), &record->rid);
  if (rc != RC::SUCCESS)
  {
    LOG_ERROR("Insert record failed. table name=%s, rc=%d:%s", table_meta_.name(), rc, strrc(rc));
    return rc;
  }

  if (trx != nullptr)
  {
    //记录事务中该操作
    rc = trx->insert_record(this, record);
    if (rc != RC::SUCCESS)
    {
      LOG_ERROR("Failed to log operation(insertion) to trx");

      RC rc2 = record_handler_->delete_record(&record->rid);
      if (rc2 != RC::SUCCESS)
      {
        LOG_PANIC("Failed to rollback record data when insert index entries failed. table name=%s, rc=%d:%s",
                  name(), rc2, strrc(rc2));
      }
      return rc;
    }
  }

  rc = insert_entry_of_indexes(record->data, record->rid);
  if (rc != RC::SUCCESS)
  {
    RC rc2 = delete_entry_of_indexes(record->data, record->rid, true);
    if (rc2 != RC::SUCCESS)
    {
      LOG_PANIC("Failed to rollback index data when insert index entries failed. table name=%s, rc=%d:%s",
                name(), rc2, strrc(rc2));
    }
    rc2 = record_handler_->delete_record(&record->rid);
    if (rc2 != RC::SUCCESS)
    {
      LOG_PANIC("Failed to rollback record data when insert index entries failed. table name=%s, rc=%d:%s",
                name(), rc2, strrc(rc2));
    }
    if (rc == RC::RECORD_DUPLICATE_KEY)
    {
      //题目：唯一索引 unique 插入之前做判断
      LOG_PANIC("出现重复 key，出现重复key");
    }
    return rc;
  }
  return rc;
}
RC Table::insert_record(Trx *trx, int value_num, const Value *values)
{
  if (value_num <= 0 || nullptr == values)
  {
    LOG_ERROR("Invalid argument. value num=%d, values=%p", value_num, values);
    return RC::INVALID_ARGUMENT;
  }
  //根据表属性和行属性 构造插入值
  char *record_data;
  RC rc = make_record(value_num, values, record_data);
  if (rc != RC::SUCCESS)
  {
    LOG_ERROR("Failed to create a record. rc=%d:%s", rc, strrc(rc));
    return rc;
  }

  Record record;
  record.data = record_data;
  // record.valid = true;
  rc = insert_record(trx, &record);
  delete[] record_data;
  return rc;
}

const char *Table::name() const
{
  return table_meta_.name();
}

const TableMeta &Table::table_meta() const
{
  return table_meta_;
}

RC Table::make_record(int value_num, const Value *values, char *&record_out)
{
  // 检查字段类型是否一致
  if (value_num + table_meta_.sys_field_num() != table_meta_.field_num())
  {
    LOG_INFO(" make_record  SCHEMA_FIELD_MISSING ");
    return RC::SCHEMA_FIELD_MISSING;
  }

  const int normal_field_start_index = table_meta_.sys_field_num();
  for (int i = 0; i < value_num; i++)
  {
    const FieldMeta *field = table_meta_.field(i + normal_field_start_index);
    const Value &value = values[i];

    //在字段允许null的情况下 可以插入null。
    //如果不允许null,插入null 肯定是报错的
    //value：来着sql命令
    //field：创建表时候确定了
    if (value.type == AttrType::NULLVALUES && 1 == field->nullable())
    {
      LOG_INFO("题目：支持NULL类型 AttrType::NULLVALUE，null可以插入任何类型");
      //这个类型不校验
    }
    else if (field->type() == AttrType::DATES)
    {
      //LOG_INFO(" insert:AttrType::DATES, value.data=%s", value.data);
      const char *pattern = "[0-9]{4}-[0-9]{1,2}-[0-9]{1,2}";
      if (0 == common::regex_match((char *)value.data, pattern))
      {
        //ok
      }
      else
      {
        // LOG_INFO(" make_record  [0-9]{4}-[0-9]{1,2}-[0-9]{1,2}  value.data=%s", value.data);
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }

      //检查日期是否合法
      char *ptr = static_cast<char *>(value.data);
      if (false == isValid_date(ptr))
      {
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
    }
    else if (field->type() == AttrType::TEXTS && value.type == AttrType::CHARS)
    {
      //超长字段text:日期 字符串 text都是字符串
    }
    else if (field->type() != value.type)
    {
      {
        LOG_ERROR("Invalid value type. field name=%s, type=%d, but given=%d",
                  field->name(), field->type(), value.type);
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
    }
  }

  // 复制所有字段的值
  int record_size = table_meta_.record_size();
  char *record = new char[record_size];
  LOG_INFO(">>>>超长字段text =%d",record_size);
  for (int i = 0; i < value_num; i++)
  {
    const FieldMeta *field = table_meta_.field(i + normal_field_start_index);
    const Value &value = values[i];
    if (value.type == AttrType::NULLVALUES)
    {
      //说明 memcpy nullptr会core。这个该怎么处理呢
      LOG_INFO("题目：支持NULL类型 AttrType::NULLVALUE，null可以插入任何类型");
      //自己约定："999"" 看看有没有问题
      memcpy(record + field->offset(), "999", field->len());
    }
    else if (field->type() == AttrType::DATES)
    {
      //字符串变成时间戳4字节存储
      //检查日期是否合法
      char *ptr = static_cast<char *>(value.data);
      string temp(ptr);
      temp.append(" 00:00:00");
      int time_t = StringToDatetime(temp);
      memcpy(record + field->offset(), &time_t, field->len());
    }
    else if (field->type() == AttrType::TEXTS)
    {
      int length = strlen((char *)value.data);
      if (length > 4096)
      {
        LOG_INFO("如果输入的字符串长度，超过4096，那么应该保存4096字节，剩余的数据截断");
        memcpy(record + field->offset(), value.data, 4096);
        // memcpy(record + field->offset(), value.data,4096);
      }
      else
      {
        //长度不超过4096 真是字符串长度 需要从哪里获取
        LOG_INFO("如果输入的字符串长度，length=%d", length);
        memcpy(record + field->offset(), value.data, length);
      }
    }
    else
    {
      memcpy(record + field->offset(), value.data, field->len());
    }
  }

  record_out = record;

  return RC::SUCCESS;
}

RC Table::init_record_handler(const char *base_dir)
{
  std::string data_file = std::string(base_dir) + "/" + table_meta_.name() + TABLE_DATA_SUFFIX;
  if (nullptr == data_buffer_pool_)
  {
    data_buffer_pool_ = theGlobalDiskBufferPool();
  }

  int data_buffer_pool_file_id;
  RC rc = data_buffer_pool_->open_file(data_file.c_str(), &data_buffer_pool_file_id);
  if (rc != RC::SUCCESS)
  {
    LOG_ERROR("Failed to open disk buffer pool for file:%s. rc=%d:%s",
              data_file.c_str(), rc, strrc(rc));
    return rc;
  }

  record_handler_ = new RecordFileHandler();
  rc = record_handler_->init(*data_buffer_pool_, data_buffer_pool_file_id);
  if (rc != RC::SUCCESS)
  {
    LOG_ERROR("Failed to init record handler. rc=%d:%s", rc, strrc(rc));
    return rc;
  }

  file_id_ = data_buffer_pool_file_id;
  return rc;
}

/**
 * 为了不把Record暴露出去，封装一下
 */
class RecordReaderScanAdapter
{
public:
  explicit RecordReaderScanAdapter(void (*record_reader)(const char *data, void *context), void *context)
      : record_reader_(record_reader), context_(context)
  {
  }

  void consume(const Record *record)
  {
    record_reader_(record->data, context_);
  }

private:
  void (*record_reader_)(const char *, void *);
  void *context_;
};
static RC scan_record_reader_adapter(Record *record, void *context)
{
  RecordReaderScanAdapter &adapter = *(RecordReaderScanAdapter *)context;
  adapter.consume(record);
  return RC::SUCCESS;
}

RC Table::scan_record(Trx *trx, ConditionFilter *filter, int limit, void *context, void (*record_reader)(const char *data, void *context))
{
  RecordReaderScanAdapter adapter(record_reader, context);
  return scan_record(trx, filter, limit, (void *)&adapter, scan_record_reader_adapter);
}

RC Table::scan_record(Trx *trx, ConditionFilter *filter, int limit, void *context, RC (*record_reader)(Record *record, void *context))
{
  if (nullptr == record_reader)
  {
    return RC::INVALID_ARGUMENT;
  }

  if (0 == limit)
  {
    return RC::SUCCESS;
  }

  if (limit < 0)
  {
    limit = INT_MAX;
  }

  IndexScanner *index_scanner = find_index_for_scan(filter);
  if (index_scanner != nullptr)
  {
    return scan_record_by_index(trx, index_scanner, filter, limit, context, record_reader);
  }

  RC rc = RC::SUCCESS;
  RecordFileScanner scanner;
  rc = scanner.open_scan(*data_buffer_pool_, file_id_, filter);
  if (rc != RC::SUCCESS)
  {
    LOG_ERROR("failed to open scanner. file id=%d. rc=%d:%s", file_id_, rc, strrc(rc));
    return rc;
  }

  int record_count = 0;
  Record record;
  rc = scanner.get_first_record(&record);
  for (; RC::SUCCESS == rc && record_count < limit; rc = scanner.get_next_record(&record))
  {
    if (trx == nullptr || trx->is_visible(this, &record))
    {
      rc = record_reader(&record, context);
      if (rc != RC::SUCCESS)
      {
        break;
      }
      record_count++;
    }
  }

  if (RC::RECORD_EOF == rc)
  {
    rc = RC::SUCCESS;
  }
  else
  {
    LOG_ERROR("failed to scan record. file id=%d, rc=%d:%s", file_id_, rc, strrc(rc));
  }
  scanner.close_scan();
  return rc;
}

RC Table::scan_record_by_index(Trx *trx, IndexScanner *scanner, ConditionFilter *filter, int limit, void *context,
                               RC (*record_reader)(Record *, void *))
{
  RC rc = RC::SUCCESS;
  RID rid;
  Record record;
  int record_count = 0;
  while (record_count < limit)
  {
    rc = scanner->next_entry(&rid);
    if (rc != RC::SUCCESS)
    {
      if (RC::RECORD_EOF == rc)
      {
        rc = RC::SUCCESS;
        break;
      }
      LOG_ERROR("Failed to scan table by index. rc=%d:%s", rc, strrc(rc));
      break;
    }

    rc = record_handler_->get_record(&rid, &record);
    if (rc != RC::SUCCESS)
    {
      LOG_ERROR("Failed to fetch record of rid=%d:%d, rc=%d:%s", rid.page_num, rid.slot_num, rc, strrc(rc));
      break;
    }

    if ((trx == nullptr || trx->is_visible(this, &record)) && (filter == nullptr || filter->filter(record)))
    {
      rc = record_reader(&record, context);
      if (rc != RC::SUCCESS)
      {
        LOG_TRACE("Record reader break the table scanning. rc=%d:%s", rc, strrc(rc));
        break;
      }
    }

    record_count++;
  }

  scanner->destroy();
  return rc;
}

class IndexInserter
{
public:
  explicit IndexInserter(Index *index) : index_(index)
  {
  }

  RC insert_index(const Record *record)
  {
    return index_->insert_entry(record->data, &record->rid);
  }

private:
  Index *index_;
};

static RC insert_index_record_reader_adapter(Record *record, void *context)
{
  IndexInserter &inserter = *(IndexInserter *)context;
  return inserter.insert_index(record);
}

RC Table::create_index(Trx *trx, const char *index_name, const char *attribute_name)
{
  if (index_name == nullptr || common::is_blank(index_name) ||
      attribute_name == nullptr || common::is_blank(attribute_name))
  {
    return RC::INVALID_ARGUMENT;
  }
  if (table_meta_.index(index_name) != nullptr ||
      table_meta_.find_index_by_field((attribute_name)))
  {
    return RC::SCHEMA_INDEX_EXIST;
  }

  const FieldMeta *field_meta = table_meta_.field(attribute_name);
  if (!field_meta)
  {
    return RC::SCHEMA_FIELD_MISSING;
  }

  IndexMeta new_index_meta;
  RC rc = new_index_meta.init(index_name, *field_meta);
  if (rc != RC::SUCCESS)
  {
    return rc;
  }

  // 创建索引相关数据
  BplusTreeIndex *index = new BplusTreeIndex();
  std::string index_file = index_data_file(base_dir_.c_str(), name(), index_name);
  rc = index->create(index_file.c_str(), new_index_meta, *field_meta);
  if (rc != RC::SUCCESS)
  {
    delete index;
    LOG_ERROR("Failed to create bplus tree index. file name=%s, rc=%d:%s", index_file.c_str(), rc, strrc(rc));
    return rc;
  }

  // 遍历当前的所有数据，插入这个索引
  IndexInserter index_inserter(index);
  rc = scan_record(trx, nullptr, -1, &index_inserter, insert_index_record_reader_adapter);
  if (rc != RC::SUCCESS)
  {
    // rollback
    delete index;
    LOG_ERROR("Failed to insert index to all records. table=%s, rc=%d:%s", name(), rc, strrc(rc));
    return rc;
  }
  indexes_.push_back(index);

  TableMeta new_table_meta(table_meta_);
  rc = new_table_meta.add_index(new_index_meta);
  if (rc != RC::SUCCESS)
  {
    LOG_ERROR("Failed to add index (%s) on table (%s). error=%d:%s", index_name, name(), rc, strrc(rc));
    return rc;
  }
  // 创建元数据临时文件
  std::string tmp_file = table_meta_file(base_dir_.c_str(), name()) + ".tmp";
  std::fstream fs;
  fs.open(tmp_file, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
  if (!fs.is_open())
  {
    LOG_ERROR("Failed to open file for write. file name=%s, errmsg=%s", tmp_file.c_str(), strerror(errno));
    return RC::IOERR; // 创建索引中途出错，要做还原操作
  }
  if (new_table_meta.serialize(fs) < 0)
  {
    LOG_ERROR("Failed to dump new table meta to file: %s. sys err=%d:%s", tmp_file.c_str(), errno, strerror(errno));
    return RC::IOERR;
  }
  fs.close();

  // 覆盖原始元数据文件
  std::string meta_file = table_meta_file(base_dir_.c_str(), name());
  int ret = rename(tmp_file.c_str(), meta_file.c_str());
  if (ret != 0)
  {
    LOG_ERROR("Failed to rename tmp meta file (%s) to normal meta file (%s) while creating index (%s) on table (%s). "
              "system error=%d:%s",
              tmp_file.c_str(), meta_file.c_str(), index_name, name(), errno, strerror(errno));
    return RC::IOERR;
  }

  table_meta_.swap(new_table_meta);

  LOG_INFO("add a new index (%s) on the table (%s)", index_name, name());

  return rc;
}

class RecordDeleter
{
public:
  RecordDeleter(Table &table, Trx *trx) : table_(table), trx_(trx)
  {
  }

  RC delete_record(Record *record)
  {
    RC rc = RC::SUCCESS;
    rc = table_.delete_record(trx_, record);
    if (rc == RC::SUCCESS)
    {
      deleted_count_++;
    }
    return rc;
  }

  int deleted_count() const
  {
    return deleted_count_;
  }

private:
  Table &table_;
  Trx *trx_;
  int deleted_count_ = 0;
};

//add by wangchuanyi
class RecordUpdated
{
public:
  RecordUpdated(Table &table, Trx *trx, const char *attribute_name, const Value *value) : table_(table), trx_(trx), attribute_name_(attribute_name), value_(value)
  {
  }

  RC update_record(Record *record)
  {
    RC rc = RC::SUCCESS;
    //构造更新记录
    rc = table_.update_record(trx_, record); //record没有更新字段
    if (rc == RC::SUCCESS)
    {
      update_count_++;
    }
    return rc;
  }

  int updated_count() const
  {
    return update_count_;
  }
  const Value *get_value() const
  {
    return value_;
  }
  const char *get_attribute_name() const
  {
    return attribute_name_;
  }

private:
  Table &table_; //成员初始化列表
  Trx *trx_;
  int update_count_ = 0;
  const char *attribute_name_;
  const Value *value_;
};

static RC record_reader_delete_adapter(Record *record, void *context)
{
  RecordDeleter &record_deleter = *(RecordDeleter *)context;
  return record_deleter.delete_record(record);
}

RC Table::delete_record(Trx *trx, ConditionFilter *filter, int *deleted_count)
{
  RecordDeleter deleter(*this, trx);
  RC rc = scan_record(trx, filter, -1, &deleter, record_reader_delete_adapter);
  if (deleted_count != nullptr)
  {
    *deleted_count = deleter.deleted_count();
  }
  return rc;
}

RC Table::delete_record(Trx *trx, Record *record)
{
  RC rc = RC::SUCCESS;
  if (trx != nullptr)
  {
    rc = trx->delete_record(this, record);
  }
  else
  {
    rc = delete_entry_of_indexes(record->data, record->rid, false); // 重复代码 refer to commit_delete
    if (rc != RC::SUCCESS)
    {
      LOG_ERROR("Failed to delete indexes of record (rid=%d.%d). rc=%d:%s",
                record->rid.page_num, record->rid.slot_num, rc, strrc(rc));
    }
    else
    {
      rc = record_handler_->delete_record(&record->rid);
    }
  }
  return rc;
}

//更新记录
static RC record_reader_updater_adapter(Record *record, void *context)
{
  RecordUpdated &record_updater = *(RecordUpdated *)context;
  return record_updater.update_record(record);
}
//record_reader_updater_adapter放在后面
//题目：实现update功能
RC Table::update_record(Trx *trx, const char *attribute_name, const Value *value, int condition_num, const Condition conditions[], int *updated_count)
{
  //LOG_INFO("update_record table: (%s),update value= %p", attribute_name, value);

  if (nullptr == attribute_name || nullptr == value)
  {
    LOG_ERROR("Invalid argument. attribute_name=%p, values=%p", attribute_name, value);
    return RC::INVALID_ARGUMENT;
  }

  //设置过滤条件
  CompositeConditionFilter condition_filter;
  RC rc = condition_filter.init(*this, conditions, condition_num);
  if (rc != RC::SUCCESS)
  {
    return rc;
  }

  //value 从哪里设置呀？？？
  trx->attribute_name = const_cast<char *>(attribute_name);
  trx->value = const_cast<Value *>(value);
  RecordUpdated updater(*this, trx, attribute_name, value);
  rc = scan_record(trx, &condition_filter, -1, &updater, record_reader_updater_adapter);
  if (updated_count != nullptr)
  {
    *updated_count = updater.updated_count();
  }

  if (rc == RC::RECORD_NO_MORE_IDX_IN_MEM)
  {
    return RC::SUCCESS;
  }

  return rc;
}
//题目：实现update功能
RC Table::update_record(Trx *trx, Record *record)
{
  RC rc = RC::SUCCESS;
  if (trx != nullptr)
  {
    LOG_INFO("update_record  begin");
    rc = trx->update_record(this, record);
  }
  /**
  else
  {

    LOG_INFO(" trx is nullptr Failed to delete indexes of record (rid=%d.%d).",
             record->rid.page_num, record->rid.slot_num);

    //rc = delete_entry_of_indexes(record->data, record->rid, false);// 重复代码 refer to commit_delete
    if (rc != RC::SUCCESS)
    {
      LOG_ERROR("Failed to delete indexes of record (rid=%d.%d). rc=%d:%s",
                record->rid.page_num, record->rid.slot_num, rc, strrc(rc));
    }
    else
    {
      rc = record_handler_->update_record(record);
    }
  }**/
  return rc;
}
RC Table::commit_delete(Trx *trx, const RID &rid)
{
  RC rc = RC::SUCCESS;
  Record record;
  rc = record_handler_->get_record(&rid, &record);
  if (rc != RC::SUCCESS)
  {
    return rc;
  }
  rc = delete_entry_of_indexes(record.data, record.rid, false);
  if (rc != RC::SUCCESS)
  {
    LOG_ERROR("Failed to delete indexes of record(rid=%d.%d). rc=%d:%s",
              rid.page_num, rid.slot_num, rc, strrc(rc)); // panic?
  }

  rc = record_handler_->delete_record(&rid);
  if (rc != RC::SUCCESS)
  {
    return rc;
  }

  return rc;
}

RC Table::rollback_delete(Trx *trx, const RID &rid)
{
  RC rc = RC::SUCCESS;
  Record record;
  rc = record_handler_->get_record(&rid, &record);
  if (rc != RC::SUCCESS)
  {
    return rc;
  }

  return trx->rollback_delete(this, record); // update record in place
}
//在索引上插入一行记录
RC Table::insert_entry_of_indexes(const char *record, const RID &rid)
{
  RC rc = RC::SUCCESS;
  //LOG_INFO("insert_entry_of_indexes size=%d \n", indexes_.size());

  for (Index *index : indexes_)
  {
    if (true == index->index_meta().isUnique_)
    {
      LOG_INFO("我是唯一索引, 我是唯一索引 %s:%s", index->index_meta().name(), index->index_meta().field());
    }
    //LOG_INFO("insert_entry_of_indexes name=%s,field=%s,value=%s \n", index->index_meta().name(), index->index_meta().field(), record);
    rc = index->insert_entry(record, &rid);
    if (rc != RC::SUCCESS)
    {
      break;
    }
  }
  return rc;
}

RC Table::delete_entry_of_indexes(const char *record, const RID &rid, bool error_on_not_exists)
{
  RC rc = RC::SUCCESS;
  //LOG_INFO("delete_entry_of_indexes size=%d \n", indexes_.size());
  for (Index *index : indexes_)
  {
    //rc=2309:RECORD_INVALID_KEY
    //LOG_INFO("delete_entry_of_indexes name=%s,field=%s,value=%s", index->index_meta().name(), index->index_meta().field(), record);
    rc = index->delete_entry(record, &rid);
    if (rc != RC::SUCCESS)
    {
      if (rc != RC::RECORD_INVALID_KEY || !error_on_not_exists)
      {

        break;
      }
    }
  }
  return rc;
}

Index *Table::find_index(const char *index_name) const
{
  for (Index *index : indexes_)
  {
    if (0 == strcmp(index->index_meta().name(), index_name))
    {
      return index;
    }
  }
  return nullptr;
}

IndexScanner *Table::find_index_for_scan(const DefaultConditionFilter &filter)
{
  const ConDesc *field_cond_desc = nullptr;
  const ConDesc *value_cond_desc = nullptr;
  if (filter.left().is_attr && !filter.right().is_attr)
  {
    field_cond_desc = &filter.left();
    value_cond_desc = &filter.right();
  }
  else if (filter.right().is_attr && !filter.left().is_attr)
  {
    field_cond_desc = &filter.right();
    value_cond_desc = &filter.left();
  }
  if (field_cond_desc == nullptr || value_cond_desc == nullptr)
  {
    return nullptr;
  }

  const FieldMeta *field_meta = table_meta_.find_field_by_offset(field_cond_desc->attr_offset);
  if (nullptr == field_meta)
  {
    LOG_PANIC("Cannot find field by offset %d. table=%s",
              field_cond_desc->attr_offset, name());
    return nullptr;
  }

  const IndexMeta *index_meta = table_meta_.find_index_by_field(field_meta->name());
  if (nullptr == index_meta)
  {
    return nullptr;
  }

  Index *index = find_index(index_meta->name());
  if (nullptr == index)
  {
    return nullptr;
  }

  return index->create_scanner(filter.comp_op(), (const char *)value_cond_desc->value);
}

IndexScanner *Table::find_index_for_scan(const ConditionFilter *filter)
{
  if (nullptr == filter)
  {
    return nullptr;
  }

  // remove dynamic_cast
  const DefaultConditionFilter *default_condition_filter = dynamic_cast<const DefaultConditionFilter *>(filter);
  if (default_condition_filter != nullptr)
  {
    LOG_INFO("find_index_for_scan  DefaultConditionFilter");
    return find_index_for_scan(*default_condition_filter);
  }

  const CompositeConditionFilter *composite_condition_filter = dynamic_cast<const CompositeConditionFilter *>(filter);
  if (composite_condition_filter != nullptr)
  {

    LOG_INFO("find_index_for_scan  CompositeConditionFilter");
    int filter_num = composite_condition_filter->filter_num();
    for (int i = 0; i < filter_num; i++)
    {
      IndexScanner *scanner = find_index_for_scan(&composite_condition_filter->filter(i));
      if (scanner != nullptr)
      {
        return scanner; // 可以找到一个最优的，比如比较符号是=
      }
    }
  }
  return nullptr;
}

RC Table::sync()
{
  RC rc = data_buffer_pool_->flush_all_pages(file_id_);
  if (rc != RC::SUCCESS)
  {
    LOG_ERROR("Failed to flush table's data pages. table=%s, rc=%d:%s", name(), rc, strrc(rc));
    return rc;
  }

  for (Index *index : indexes_)
  {
    rc = index->sync();
    if (rc != RC::SUCCESS)
    {
      LOG_ERROR("Failed to flush index's pages. table=%s, index=%s, rc=%d:%s",
                name(), index->index_meta().name(), rc, strrc(rc));
      return rc;
    }
  }
  LOG_INFO("Sync table over. table=%s", name());
  return rc;
}

RC Table::commit_update(Trx *trx, const RID &rid)
{

  LOG_INFO(">>>>>>>>>Table::commit_update");
  RC rc = RC::SUCCESS;
  //01 查询记录
  Record record;
  rc = record_handler_->get_record(&rid, &record);
  if (rc != RC::SUCCESS)
  {
    LOG_INFO("commit_update get_record failed =%d", rc);
    return rc;
  }
  //02 set id=10;

  //rc = delete_entry_of_indexes(record.data, record.rid, false);
  //if (rc != RC::SUCCESS) {
  //  LOG_ERROR("Failed to delete indexes of record(rid=%d.%d). rc=%d:%s",
  //             rid.page_num, rid.slot_num, rc, strrc(rc));// panic?
  // }

  //03  update_record

  rc = record_handler_->update_record(&record);
  if (rc != RC::SUCCESS)
  {
    LOG_INFO("delete_record get_record failed =%d", rc);
    return rc;
  }

  return rc;
}

RC Table::commit_update(Trx *trx, const RID &rid, const char *attribute_name, const Value *value)
{
  //("commit_update. attribute_name=%s, values=%p", attribute_name, value);

  RC rc = RC::SUCCESS;
  if (nullptr == attribute_name || nullptr == value || nullptr == value->data)
  {
    LOG_ERROR("Invalid argument. attribute_name=%s, values=%p", attribute_name, value);
    return RC::INVALID_ARGUMENT;
  }

  //步骤1:检查更新字段属性是否合法：set id=10;

  //根据upate字段----查找属性----获取偏移量
  const FieldMeta *field_meta = table_meta_.field(attribute_name);
  if (nullptr == field_meta)
  {
    //("table_meta_.field(attribute_name) =%s", attribute_name);
    return RC::SCHEMA_FIELD_NOT_EXIST;
  }
  //特殊处理：AttrType::DATES
  if (field_meta->type() == AttrType::DATES && value->type == AttrType::CHARS)
  {
    //LOG_INFO(" insert:AttrType::DATES, value.data=%s", value->data);
    const char *pattern = "[0-9]{4}-[0-9]{1,2}-[0-9]{1,2}";
    if (0 == common::regex_match((char *)value->data, pattern))
    {
      //ok
    }
    else
    {
      // LOG_INFO(" make_record  [0-9]{4}-[0-9]{1,2}-[0-9]{1,2}  value.data=%s", value->data);
      return RC::SCHEMA_FIELD_TYPE_MISMATCH;
    }

    //检查日期是否合法

    char *ptr = static_cast<char *>(value->data);
    if (false == isValid_date(ptr))
    {
      return RC::SCHEMA_FIELD_TYPE_MISMATCH;
    }
    //特殊处理：AttrType::DATES
  }
  else if (field_meta->type() == AttrType::DATES && value->type != AttrType::CHARS)
  {
    LOG_ERROR("Invalid value type. field name=%s, type=%d, but given=%d",
              field_meta->name(), field_meta->type(), value->type);
    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  }
  else if (field_meta->type() != value->type)
  {
    ////对于整数与浮点数之间的转换，不做考察。学有余力的同学，可以做一下。
    //&& > ||
    //if ((field_meta->type() == AttrType::INTS && value->type == AttrType::FLOATS) || (field_meta->type() == AttrType::FLOATS && value->type == AttrType::INTS))
    //{
    //LOG_INFO(" 对于整数与浮点数之间的转换，不做考察。学有余力的同学，可以做一下");
    //}
    //else
    {
      LOG_ERROR("Invalid value type. field name=%s, type=%d, but given=%d",
                field_meta->name(), field_meta->type(), value->type);
      return RC::SCHEMA_FIELD_TYPE_MISMATCH;
    }
  }

  //步骤2  查询记录---修改value--更新
  Record oldrecord;
  rc = record_handler_->get_record(&rid, &oldrecord);
  if (rc != RC::SUCCESS)
  {
    LOG_INFO("commit_update get_record failed =%d", rc);
    return rc;
  }

  Record newrecord;
  newrecord.rid = oldrecord.rid;
  newrecord.data = strdup(oldrecord.data);

  if (field_meta->type() == AttrType::DATES && value->type == AttrType::CHARS)
  {
    char *ptr = static_cast<char *>(value->data);
    string temp(ptr);
    temp.append(" 00:00:00");
    int time_t = StringToDatetime(temp);
    memcpy(oldrecord.data + field_meta->offset(), &time_t, field_meta->len());
  }
  else
  {
    memcpy(oldrecord.data + field_meta->offset(), value->data, field_meta->len());
  }

  //03  update_record
  rc = record_handler_->update_record(&oldrecord);
  if (rc != RC::SUCCESS)
  {
    LOG_INFO("delete_record get_record failed =%d", rc);
    return rc;
  }

  //04 重建索引算法描述：
  //步骤01：判断更新字段是否存在索引
  const IndexMeta *index_meta = table_meta_.find_index_by_field(attribute_name);
  //例如 map[key]=value || 二叉查找树（Binary Search Tree）
  //步骤02:无索引--value 怎么变化都不影响key排序 。直接返回结束。
  //步骤03：有索引---key变化，排序位置发送变化。需要调整。先删除后插入方式。
  //查缺补漏： b+,map 二查搜索tree
  if (nullptr != index_meta)
  {
    Index *index = find_index(index_meta->name());
    if (nullptr != index)
    {
      rc = index->delete_entry(newrecord.data, &newrecord.rid);
      if (rc != RC::SUCCESS)
      {
        LOG_ERROR(" update:Failed to delete indexes of record(rid=%d.%d). rc=%d:%s",
                  rid.page_num, rid.slot_num, rc, strrc(rc)); // panic?
      }
      rc = index->insert_entry(oldrecord.data, &oldrecord.rid);
      if (rc != RC::SUCCESS)
      {
        LOG_ERROR(" update:Failed to delete indexes of record(rid=%d.%d). rc=%d:%s",
                  rid.page_num, rid.slot_num, rc, strrc(rc)); // panic?
      }
      if (rc == RC::RECORD_DUPLICATE_KEY)
      {
        rc = RC::SUCCESS;
      }
    }
  }
  return rc;
}
/**
 * 结构：
 * 
 * 步骤
 * 1. 删除文件 
 */
RC Table::drop_index(Trx *trx, const char *relation_name, const char *index_name)
{
  RC rc = RC::SUCCESS;
  if (index_name == nullptr || common::is_blank(index_name) ||
      relation_name == nullptr || common::is_blank(relation_name))
  {
    return RC::INVALID_ARGUMENT;
  }
  /**
  if (table_meta_.index(index_name) != nullptr ||
      table_meta_.find_index_by_field((attribute_name)))
  {
    return RC::SCHEMA_INDEX_EXIST;
  }

  const FieldMeta *field_meta = table_meta_.field(attribute_name);
  if (!field_meta)
  {
    return RC::SCHEMA_FIELD_MISSING;
  }

  IndexMeta new_index_meta;
  RC rc = new_index_meta.init(index_name, *field_meta);
  if (rc != RC::SUCCESS)
  {
    return rc;
  }

  // 创建索引相关数据
  BplusTreeIndex *index = new BplusTreeIndex();
  std::string index_file = index_data_file(base_dir_.c_str(), name(), index_name);
  rc = index->create(index_file.c_str(), new_index_meta, *field_meta);
  if (rc != RC::SUCCESS)
  {
    delete index;
    LOG_ERROR("Failed to create bplus tree index. file name=%s, rc=%d:%s", index_file.c_str(), rc, strrc(rc));
    return rc;
  }

  // 遍历当前的所有数据，插入这个索引
  IndexInserter index_inserter(index);
  rc = scan_record(trx, nullptr, -1, &index_inserter, insert_index_record_reader_adapter);
  if (rc != RC::SUCCESS)
  {
    // rollback
    delete index;
    LOG_ERROR("Failed to insert index to all records. table=%s, rc=%d:%s", name(), rc, strrc(rc));
    return rc;
  }
  indexes_.push_back(index);

  TableMeta new_table_meta(table_meta_);
  rc = new_table_meta.add_index(new_index_meta);
  if (rc != RC::SUCCESS)
  {
    LOG_ERROR("Failed to add index (%s) on table (%s). error=%d:%s", index_name, name(), rc, strrc(rc));
    return rc;
  }**/

  // 创建元数据临时文件
  std::string index_file = index_data_file(base_dir_.c_str(), relation_name, index_name);

  int ret = remove(index_file.c_str());
  if (-1 == ret)
  {
    LOG_ERROR(" drop table file failed. filename=%s, errmsg=%d:%s",
              index_file.c_str(), errno, strerror(errno));
    return RC::IOERR;
  }

  //("drop index_file=%s", index_file.c_str());

  /**
  // 覆盖原始元数据文件
  std::string meta_file = table_meta_file(base_dir_.c_str(), name());
  int ret = rename(tmp_file.c_str(), meta_file.c_str());
  if (ret != 0)
  {
    LOG_ERROR("Failed to rename tmp meta file (%s) to normal meta file (%s) while creating index (%s) on table (%s). "
              "system error=%d:%s",
              tmp_file.c_str(), meta_file.c_str(), index_name, name(), errno, strerror(errno));
    return RC::IOERR;
  }

  table_meta_.swap(new_table_meta);

  LOG_INFO("add a new index (%s) on the table (%s)", index_name, name());
  **/
  return rc;
}

bool Table::isValid_date(const char *pdata) //判断日期（年月日）是否合法的函数定义
{
  if (nullptr == pdata)
  {
    return false;
  }
  int year, month, day;
  sscanf(pdata, "%d-%d-%d", &year, &month, &day);

  cout << "1please input year:" << year << endl;
  cout << "1please input month:" << month << endl;
  cout << "1please input day:" << day << endl;

  if (year < 1970 || year > 2038 || (year == 2038 && month >= 2))
  {
    return false;
  }

  int leap = 0; //判断闰年的标记

  if (month < 1 || month > 12 || day < 1 || day > 31)
    return 0; //返回不合法

  if ((month == 4 || month == 6 || month == 9 || month == 11) && (day == 31))
    return 0; //返回不合法

  if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) //判断是否是闰年
    leap = 1;                                                //是闰年

  if ((leap == 1 && month == 2 && day > 29) || (leap == 0 && month == 2 && day > 28))
    return false; //返回不合法

  return true; //返回合法
}

//唯一索引unique
RC Table::create_unique_index(Trx *trx, const char *index_name, const char *attribute_name)
{
  if (index_name == nullptr || common::is_blank(index_name) ||
      attribute_name == nullptr || common::is_blank(attribute_name))
  {
    return RC::INVALID_ARGUMENT;
  }
  if (table_meta_.index(index_name) != nullptr ||
      table_meta_.find_index_by_field((attribute_name)))
  {
    return RC::SCHEMA_INDEX_EXIST;
  }
  //create unique index unique_index_01 on t(id);
  //attribute_name:id
  const FieldMeta *field_meta = table_meta_.field(attribute_name);
  if (!field_meta)
  {
    return RC::SCHEMA_FIELD_MISSING;
  }

  IndexMeta new_index_meta;
  //create unique index unique_index_01 on t(id);
  //attribute_name:id
  //index_name:unique_index_01
  RC rc = new_index_meta.init(index_name, *field_meta, true);
  if (rc != RC::SUCCESS)
  {
    return rc;
  }

  // 创建索引相关数据
  BplusTreeIndex *index = new BplusTreeIndex();
  // ".index";
  std::string index_file = index_data_file(base_dir_.c_str(), name(), index_name);
  rc = index->create(index_file.c_str(), new_index_meta, *field_meta);
  if (rc != RC::SUCCESS)
  {
    delete index;
    LOG_ERROR("Failed to create bplus tree index. file name=%s, rc=%d:%s", index_file.c_str(), rc, strrc(rc));
    return rc;
  }

  // 遍历当前的所有数据，插入这个索引
  IndexInserter index_inserter(index);
  rc = scan_record(trx, nullptr, -1, &index_inserter, insert_index_record_reader_adapter);
  if (rc != RC::SUCCESS)
  {
    // rollback
    delete index;
    LOG_ERROR("Failed to insert index to all records. table=%s, rc=%d:%s", name(), rc, strrc(rc));
    return rc;
  }
  indexes_.push_back(index);

  TableMeta new_table_meta(table_meta_);
  rc = new_table_meta.add_index(new_index_meta);
  if (rc != RC::SUCCESS)
  {
    LOG_ERROR("Failed to add index (%s) on table (%s). error=%d:%s", index_name, name(), rc, strrc(rc));
    return rc;
  }
  // 创建元数据临时文件
  std::string tmp_file = table_meta_file(base_dir_.c_str(), name()) + ".tmp";
  std::fstream fs;
  fs.open(tmp_file, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
  if (!fs.is_open())
  {
    LOG_ERROR("Failed to open file for write. file name=%s, errmsg=%s", tmp_file.c_str(), strerror(errno));
    return RC::IOERR; // 创建索引中途出错，要做还原操作
  }
  if (new_table_meta.serialize(fs) < 0)
  {
    LOG_ERROR("Failed to dump new table meta to file: %s. sys err=%d:%s", tmp_file.c_str(), errno, strerror(errno));
    return RC::IOERR;
  }
  fs.close();

  // 覆盖原始元数据文件
  std::string meta_file = table_meta_file(base_dir_.c_str(), name());
  int ret = rename(tmp_file.c_str(), meta_file.c_str());
  if (ret != 0)
  {
    LOG_ERROR("Failed to rename tmp meta file (%s) to normal meta file (%s) while creating index (%s) on table (%s). "
              "system error=%d:%s",
              tmp_file.c_str(), meta_file.c_str(), index_name, name(), errno, strerror(errno));
    return RC::IOERR;
  }

  table_meta_.swap(new_table_meta);

  LOG_INFO("add a new index (%s) on the table (%s)", index_name, name());

  return rc;
}

RC Table::insert_record_rows(Trx *trx, int values_length, const InsertLeft *pValues)
{
  RC rc = RC::SUCCESS;
  if (values_length <= 1 || nullptr == pValues)
  {
    return RC::INVALID_ARGUMENT;
  }
  //步骤01：指针数组
  //https://www.cjavapy.com/article/1753/

  char *pRecordArray[values_length];
  for (int rows = 0; rows < values_length; rows++)
  {
    int value_num = pValues[rows].value_num;
    const Value *values = pValues[rows].values;

    //指针的引用：二级指针 可以修改pRecordArray里的值
    rc = make_record(value_num, values, pRecordArray[rows]);
    if (rc != RC::SUCCESS)
    {
      LOG_ERROR("Failed to create a record. rc=%d:%s", rc, strrc(rc));
      return rc;
    }
  }
  /**
  char *record_data;
  RC rc = make_record(value_num, values, record_data);
  if (rc != RC::SUCCESS)
  {
    LOG_ERROR("Failed to create a record. rc=%d:%s", rc, strrc(rc));
    return rc;
  }**/

  //步骤02:批量插入
  for (int rows = 0; rows < values_length; rows++)
  {
    Record record;
    record.data = pRecordArray[rows];
    rc = insert_record(trx, &record);
    if (rc != RC::SUCCESS)
    {
      LOG_ERROR("insert_record rc=%d:%s", rc, strrc(rc));
      return rc;
    }
    delete[] pRecordArray[rows];
  }

  return rc;
}

RC Table::create_index_multi(Trx *trx, const char *index_name, int attr_num, char *const attributes[])
{
  //至少2个
  if (attr_num <= 1 || index_name == nullptr || common::is_blank(index_name))
  {
    return RC::INVALID_ARGUMENT;
  }

  std::vector<string> multiIndex;
  for (int i = 0; i < attr_num; i++)
  {
    if (attributes[i] == nullptr || common::is_blank(attributes[i]))
    {
      return RC::INVALID_ARGUMENT;
    }
    multiIndex.push_back(attributes[i]);
  }

  //01 判断索引-字段 是否重复

  if (table_meta_.index(index_name) != nullptr ||
      table_meta_.find_index_by_field_multi(attr_num, attributes))
  {

    LOG_INFO("题目：多列索引 multi-index  索引重复了 错误");
    return RC::SCHEMA_INDEX_EXIST;
  }

  //02判断创建的索引-字段 是否在表中存在
  std::vector<FieldMeta> fields_meta(attr_num);
  for (int i = 0; i < attr_num; i++)
  {
    const FieldMeta *field_meta = table_meta_.field(attributes[i]);
    if (!field_meta)
    {
      return RC::SCHEMA_FIELD_MISSING;
    }
    fields_meta[i] = *field_meta;
  }

  //03 创建multi-index 类
  IndexMeta new_index_meta;
  RC rc = new_index_meta.init(index_name, fields_meta);
  if (rc != RC::SUCCESS)
  {
    return rc;
  }

  // 创建索引相关数据
  BplusTreeIndex *index = new BplusTreeIndex();
  std::string index_file = index_data_file(base_dir_.c_str(), name(), index_name);
  rc = index->create_multi(index_file.c_str(), new_index_meta, fields_meta);
  if (rc != RC::SUCCESS)
  {
    delete index;
    LOG_ERROR("Failed to create bplus tree index. file name=%s, rc=%d:%s", index_file.c_str(), rc, strrc(rc));
    return rc;
  }

  // 遍历当前的所有数据，插入这个索引
  IndexInserter index_inserter(index);
  rc = scan_record(trx, nullptr, -1, &index_inserter, insert_index_record_reader_adapter);
  if (rc != RC::SUCCESS)
  {
    // rollback
    delete index;
    LOG_ERROR("Failed to insert index to all records. table=%s, rc=%d:%s", name(), rc, strrc(rc));
    return rc;
  }
  indexes_.push_back(index);

  TableMeta new_table_meta(table_meta_);
  rc = new_table_meta.add_index(new_index_meta);
  if (rc != RC::SUCCESS)
  {
    LOG_ERROR("Failed to add index (%s) on table (%s). error=%d:%s", index_name, name(), rc, strrc(rc));
    return rc;
  }
  // 创建元数据临时文件
  std::string tmp_file = table_meta_file(base_dir_.c_str(), name()) + ".tmp";
  std::fstream fs;
  fs.open(tmp_file, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
  if (!fs.is_open())
  {
    LOG_ERROR("Failed to open file for write. file name=%s, errmsg=%s", tmp_file.c_str(), strerror(errno));
    return RC::IOERR; // 创建索引中途出错，要做还原操作
  }
  if (new_table_meta.serialize_mutil(fs) < 0)
  {
    LOG_ERROR("Failed to dump new table meta to file: %s. sys err=%d:%s", tmp_file.c_str(), errno, strerror(errno));
    return RC::IOERR;
  }
  fs.close();

  // 覆盖原始元数据文件
  std::string meta_file = table_meta_file(base_dir_.c_str(), name());
  int ret = rename(tmp_file.c_str(), meta_file.c_str());
  if (ret != 0)
  {
    LOG_ERROR("Failed to rename tmp meta file (%s) to normal meta file (%s) while creating index (%s) on table (%s). "
              "system error=%d:%s",
              tmp_file.c_str(), meta_file.c_str(), index_name, name(), errno, strerror(errno));
    return RC::IOERR;
  }

  table_meta_.swap(new_table_meta);

  LOG_INFO("add a new index (%s) on the table (%s)", index_name, name());

  return rc;
}