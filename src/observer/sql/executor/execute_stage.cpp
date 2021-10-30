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
// Created by Longda on 2021/4/13.
//

#include <string>
#include <sstream>

#include "execute_stage.h"

#include "common/io/io.h"
#include "common/log/log.h"
#include "common/seda/timer_stage.h"
#include "common/lang/string.h"
#include "session/session.h"
#include "event/storage_event.h"
#include "event/sql_event.h"
#include "event/session_event.h"
#include "event/execution_plan_event.h"
#include "sql/executor/execution_node.h"
#include "sql/executor/tuple.h"
#include "storage/common/table.h"
#include "storage/default/default_handler.h"
#include "storage/common/condition_filter.h"
#include "storage/trx/trx.h"

using namespace common;
static RC schema_add_field_function(Table *table, const char *field_name, TupleSchema &schema,FunctionType ft);

RC create_selection_executor(Trx *trx, const Selects &selects, const char *db, const char *table_name, SelectExeNode &select_node);

//! Constructor
ExecuteStage::ExecuteStage(const char *tag) : Stage(tag) {}

//! Destructor
ExecuteStage::~ExecuteStage() {}

//! Parse properties, instantiate a stage object
Stage *ExecuteStage::make_stage(const std::string &tag)
{
  ExecuteStage *stage = new (std::nothrow) ExecuteStage(tag.c_str());
  if (stage == nullptr)
  {
    LOG_ERROR("new ExecuteStage failed");
    return nullptr;
  }
  stage->set_properties();
  return stage;
}

//! Set properties for this object set in stage specific properties
bool ExecuteStage::set_properties()
{
  //  std::string stageNameStr(stageName);
  //  std::map<std::string, std::string> section = theGlobalProperties()->get(
  //    stageNameStr);
  //
  //  std::map<std::string, std::string>::iterator it;
  //
  //  std::string key;

  return true;
}

//! Initialize stage params and validate outputs
bool ExecuteStage::initialize()
{
  LOG_TRACE("Enter");

  std::list<Stage *>::iterator stgp = next_stage_list_.begin();
  default_storage_stage_ = *(stgp++);
  mem_storage_stage_ = *(stgp++);

  LOG_TRACE("Exit");
  return true;
}

//! Cleanup after disconnection
void ExecuteStage::cleanup()
{
  LOG_TRACE("Enter");

  LOG_TRACE("Exit");
}

void ExecuteStage::handle_event(StageEvent *event)
{
  LOG_TRACE("Enter\n");

  handle_request(event);

  LOG_TRACE("Exit\n");
  return;
}

void ExecuteStage::callback_event(StageEvent *event, CallbackContext *context)
{
  LOG_TRACE("Enter\n");

  // here finish read all data from disk or network, but do nothing here.
  ExecutionPlanEvent *exe_event = static_cast<ExecutionPlanEvent *>(event);
  SQLStageEvent *sql_event = exe_event->sql_event();
  sql_event->done_immediate();

  LOG_TRACE("Exit\n");
  return;
}

void ExecuteStage::handle_request(common::StageEvent *event)
{
  ExecutionPlanEvent *exe_event = static_cast<ExecutionPlanEvent *>(event);
  SessionEvent *session_event = exe_event->sql_event()->session_event();
  Query *sql = exe_event->sqls();
  const char *current_db = session_event->get_client()->session->get_current_db().c_str();

  CompletionCallback *cb = new (std::nothrow) CompletionCallback(this, nullptr);
  if (cb == nullptr)
  {
    LOG_ERROR("Failed to new callback for ExecutionPlanEvent");
    exe_event->done_immediate();
    return;
  }
  exe_event->push_callback(cb);

  switch (sql->flag)
  {
  case SCF_SELECT:
  { // select
    RC rc = do_select(current_db, sql, exe_event->sql_event()->session_event());
    if (rc != RC::SUCCESS)
    {
      LOG_INFO("do_select failed rc=%d:%s", rc, strrc(rc));
    }

    int len = exe_event->sql_event()->session_event()->get_response_len();
    if (len <= 0)
    { //case1 如果查询无记录,不会进入该逻辑。需要返回列名
      LOG_INFO("  len =0 ,do_select failed rc=%d:%s", rc, strrc(rc));
      exe_event->sql_event()->session_event()->set_response("FAILURE\n"); //返回结果
    }

    exe_event->done_immediate();
  }
  break;

  case SCF_INSERT:
  case SCF_UPDATE:
  case SCF_DELETE:
  case SCF_CREATE_TABLE:
  case SCF_SHOW_TABLES:
  case SCF_DESC_TABLE:
  case SCF_DROP_TABLE:
  case SCF_CREATE_INDEX:
  case SCF_DROP_INDEX:
  case SCF_LOAD_DATA:
  {
    StorageEvent *storage_event = new (std::nothrow) StorageEvent(exe_event);
    if (storage_event == nullptr)
    {
      LOG_ERROR("Failed to new StorageEvent");
      event->done_immediate();
      return;
    }

    default_storage_stage_->handle_event(storage_event);
  }
  break;
  case SCF_SYNC:
  {
    RC rc = DefaultHandler::get_default().sync();
    session_event->set_response(strrc(rc));
    exe_event->done_immediate();
  }
  break;
  case SCF_BEGIN:
  {
    session_event->get_client()->session->set_trx_multi_operation_mode(true);
    session_event->set_response(strrc(RC::SUCCESS));
    exe_event->done_immediate();
  }
  break;
  case SCF_COMMIT:
  {
    Trx *trx = session_event->get_client()->session->current_trx();
    RC rc = trx->commit();
    session_event->get_client()->session->set_trx_multi_operation_mode(false);
    session_event->set_response(strrc(rc));
    exe_event->done_immediate();
  }
  break;
  case SCF_ROLLBACK:
  {
    Trx *trx = session_event->get_client()->session->current_trx();
    RC rc = trx->rollback();
    session_event->get_client()->session->set_trx_multi_operation_mode(false);
    session_event->set_response(strrc(rc));
    exe_event->done_immediate();
  }
  break;
  case SCF_HELP:
  {
    const char *response = "show tables;\n"
                           "desc `table name`;\n"
                           "create table `table name` (`column name` `column type`, ...);\n"
                           "create index `index name` on `table` (`column`);\n"
                           "insert into `table` values(`value1`,`value2`);\n"
                           "update `table` set column=value [where `column`=`value`];\n"
                           "delete from `table` [where `column`=`value`];\n"
                           "select [ * | `columns` ] from `table`;\n";
    session_event->set_response(response);
    exe_event->done_immediate();
  }
  break;
  case SCF_EXIT:
  {
    // do nothing
    const char *response = "Unsupported\n";
    session_event->set_response(response);
    exe_event->done_immediate();
  }
  break;
  default:
  {
    exe_event->done_immediate();
    LOG_ERROR("Unsupported command=%d\n", sql->flag);
  }
  }
}

void end_trx_if_need(Session *session, Trx *trx, bool all_right)
{
  if (!session->is_trx_multi_operation_mode())
  {
    if (all_right)
    {
      trx->commit();
    }
    else
    {
      trx->rollback();
    }
  }
}

// 这里没有对输入的某些信息做合法性校验，比如查询的列名、where条件中的列名等，没有做必要的合法性校验
// 需要补充上这一部分. 校验部分也可以放在resolve，不过跟execution放一起也没有关系
RC ExecuteStage::do_select(const char *db, Query *sql, SessionEvent *session_event)
{

  RC rc = RC::SUCCESS;
  Session *session = session_event->get_client()->session;
  Trx *trx = session->current_trx();
  const Selects &selects = sql->sstr.selection; //Selects语法树

  // 把所有的表和只跟这张表关联的condition都拿出来，生成最底层的select 执行节点
  // 把所有的表和只跟这张表关联的condition都拿出来，生成最底层的select 执行节点 [官方注释很到位]
  // 把所有的表和只跟这张表关联的condition都拿出来，生成最底层的select 执行节点 [官方注释很到位]
  std::vector<SelectExeNode *> select_nodes;
  //relations 多个表
  for (size_t i = 0; i < selects.relation_num; i++)
  {
    const char *table_name = selects.relations[i];
    SelectExeNode *select_node = new SelectExeNode;
    //创建执行计划
    rc = create_selection_executor(trx, selects, db, table_name, *select_node);
    if (rc != RC::SUCCESS)
    {
      //LOG_INFO("create_selection_executor failed rc=%d:%s", rc, strrc(rc));
      delete select_node;
      for (SelectExeNode *&tmp_node : select_nodes)
      {
        delete tmp_node;
      }
      //检查具体的错误:表不存在，字段不存在
      if (RC::SCHEMA_TABLE_NOT_EXIST == rc || RC::SCHEMA_FIELD_MISSING == rc)
      {
        session_event->set_response("FAILURE\n"); //返回结果
      }

      end_trx_if_need(session, trx, false);
      return rc;
    }
    select_nodes.push_back(select_node);
  }

  if (select_nodes.empty())
  {
    LOG_ERROR("No table given");
    end_trx_if_need(session, trx, false);
    return RC::SQL_SYNTAX;
  }
  //疑问：这是什么没看懂？？
  //我猜测读取行记录
  //后面：跟踪
  std::vector<TupleSet> tuple_sets;
  //一个表 一个TupleSet记录
  //TupleSet 存储 列属性 和行的value
  for (SelectExeNode *&node : select_nodes)
  {
    TupleSet tuple_set;
    rc = node->execute(tuple_set);
    //日期查询：
    if (rc != RC::SUCCESS && rc != RC::RECORD_NO_MORE_IDX_IN_MEM)
    {
      //LOG_INFO("execute failed rc=%d:%s", rc, strrc(rc));
      for (SelectExeNode *&tmp_node : select_nodes)
      {
        delete tmp_node;
      }
      end_trx_if_need(session, trx, false);
      return rc;
    }
    else 
    { 
      if(rc == RC::RECORD_NO_MORE_IDX_IN_MEM)
      {
        //tuple_set.get_tuple().clear();
        LOG_INFO(">>>>>execute failed rc=%d:%s", rc, strrc(rc));
      }
      tuple_sets.push_back(std::move(tuple_set));
    }
  }

  std::stringstream ss;

  // 当前只查询一张表，直接返回结果即可
  if (tuple_sets.size() == 1)
  {
    //单表：
    tuple_sets.front().print(ss);
    
    //题目：多表查询
  }
  else if (tuple_sets.size() == 2)
  {

    TupleSet twoSet;

    //添加列信息：
    //列信息: schema_ (type_ = INTS, table_name_ = "t1", field_name_ = "id")

    twoSet.set_schema(tuple_sets[1].get_schema());       //第一个表信息
    twoSet.add_tuple_schema(tuple_sets[0].get_schema()); // 第二个表信息
    //一个表 有2个字段，2个表 这里就四行记录

    //添加行信息
    //std::vector<Tuple> tuples_;
    //twoSet.add_tuple_value(tuple_sets[1].get_tuple());
    //twoSet.add_tuple_value(tuple_sets[0].get_tuple());

    twoSet.set_tuples_left(std::move(tuple_sets[1].get_tuple()));
    twoSet.set_tuples_right(std::move(tuple_sets[0].get_tuple()));

    //通过selects判断是否有联合查询不合适，因为这个是多个表查询，拆分单独一个表查询
    //虽然你看到过滤，但是没有认真从上到下阅读代码导致 ，不清楚上面逻辑
    //思路和时间都浪费了 120分钟

    /**if(selects.condition_num >0 )
      {
        std::vector<DefaultConditionFilter*> filter=select_nodes[0]->get_condition_filters();
        twoSet.set_filter(std::move(filter));
      }**/

    //这里假设只有一个join条件 并且查找返回字段和查询条件是一致的。

    bool isJoin = false;
    int joinIndex = 0;
    for (int i = 0; i < selects.condition_num; i++)
    {
      const Condition &condition = selects.conditions[i];
      if (condition.left_is_attr == 1 && condition.right_is_attr == 1 &&
          0 != strcmp(condition.left_attr.relation_name, condition.right_attr.relation_name) &&
          condition.comp == EQUAL_TO)
      {
        isJoin = true;
        joinIndex = i;
      }
    }
    
    twoSet.set_join(isJoin, joinIndex);
    twoSet.print(ss);
  }
  else
  {
    //不支持次操作
    LOG_INFO("No such three table");
  }

  for (SelectExeNode *&tmp_node : select_nodes)
  {
    delete tmp_node;
  }
  //case:查询不到返回列名字
  std::string queryresault = ss.str();

  session_event->set_response(ss.str());
  LOG_INFO(" query resault. %s", queryresault.c_str());

  end_trx_if_need(session, trx, true);
  return rc;
}

bool match_table(const Selects &selects, const char *table_name_in_condition, const char *table_name_to_match)
{
  if (table_name_in_condition != nullptr)
  {
    return 0 == strcmp(table_name_in_condition, table_name_to_match);
  }

  return selects.relation_num == 1;
}

static RC schema_add_field(Table *table, const char *field_name, TupleSchema &schema)
{  
  if (0 == strcmp("*", field_name))
  {
       TupleSchema::from_table(table, schema);
       return RC::SUCCESS;
  }
  const FieldMeta *field_meta = table->table_meta().field(field_name);
  if (nullptr == field_meta)
  {
    LOG_WARN("No such field. %s.%s", table->name(), field_name);
    return RC::SCHEMA_FIELD_MISSING;
  }
  //避免 t.id,t.id
  schema.add_if_not_exists(field_meta->type(), table->name(), field_meta->name());
  return RC::SUCCESS;
}

// 把所有的表和只跟这张表关联的condition都拿出来，生成最底层的select 执行节点
RC create_selection_executor(Trx *trx, const Selects &selects, const char *db, const char *table_name, SelectExeNode &select_node)
{
  // 列出跟这张表关联的Attr
  TupleSchema schema;
  //根据表名字 获取 表信息
  Table *table = DefaultHandler::get_default().find_table(db, table_name);
  if (nullptr == table)
  {
    LOG_WARN("No such table [%s] in db [%s]", table_name, db);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  for (int i = selects.attr_num - 1; i >= 0; i--)
  {
    const RelAttr &attr = selects.attributes[i];
    if (nullptr == attr.relation_name || 0 == strcmp(table_name, attr.relation_name))
    {  
      //count(*) count(id)
      //支持部分聚合函数
      if(attr.funtype == FunctionType::FUN_COUNT ||  
         attr.funtype == FunctionType::FUN_COUNT_ALL||
         attr.funtype == FunctionType::FUN_MAX ||
         attr.funtype == FunctionType::FUN_MIN ||
         attr.funtype == FunctionType::FUN_AVG )
      {
        LOG_WARN("this is window functions");
      }
      //select count(*) from t;
      if (0 == strcmp("*", attr.attribute_name))
      {
        if(attr.funtype == FunctionType::FUN_COUNT_ALL)
        {
          // 列出这张表第一个字段到 schema
          //
          TupleSchema::from_table_first(table, schema,attr.funtype);
        }else
        {
          // 列出这张表所有字段到 schema
           TupleSchema::from_table(table, schema);
           break;
        }
      }
      else
      {  
        LOG_WARN(" table [%s] in db [%s]", table_name, attr.attribute_name);
        //select count(id),count(name) from t;
        FunctionType ft=attr.funtype ;
        if(ft == FunctionType::FUN_COUNT || ft == FunctionType::FUN_MAX
          || ft == FunctionType::FUN_MIN || ft ==FunctionType::FUN_AVG)
        {
            // 列出这张表相关字段
            RC rc = schema_add_field_function(table, attr.attribute_name, schema,ft);
            if (rc != RC::SUCCESS)
            {
                return rc;
            }
        }else
        {
            // 列出这张表相关字段
            RC rc = schema_add_field(table, attr.attribute_name, schema);
            if (rc != RC::SUCCESS)
            {
                return rc;
            }
        }
       
      }
    }
  }

  // 找出仅与此表相关的过滤条件, 或者都是值的过滤条件
  //潜台词：
  // 1 t1.id=t2.id 左右都是属性名，并且表名都符合 [多表不符合]
  // 2 t1.age >10  左边是属性右边是值  table_name
  std::vector<DefaultConditionFilter *> condition_filters;
  for (size_t i = 0; i < selects.condition_num; i++)
  {
    const Condition &condition = selects.conditions[i];
    if ((condition.left_is_attr == 0 && condition.right_is_attr == 0) ||                                                                         // 两边都是值
        (condition.left_is_attr == 1 && condition.right_is_attr == 0 && match_table(selects, condition.left_attr.relation_name, table_name)) ||  // 左边是属性右边是值
        (condition.left_is_attr == 0 && condition.right_is_attr == 1 && match_table(selects, condition.right_attr.relation_name, table_name)) || // 左边是值，右边是属性名
        (condition.left_is_attr == 1 && condition.right_is_attr == 1 &&
         match_table(selects, condition.left_attr.relation_name, table_name) && match_table(selects, condition.right_attr.relation_name, table_name)) // 左右都是属性名，并且表名都符合
    )
    {
      DefaultConditionFilter *condition_filter = new DefaultConditionFilter();
      RC rc = condition_filter->init(*table, condition);
      //什么情况返回失败！！！
      if (rc != RC::SUCCESS)
      {
        LOG_INFO(" gen DefaultConditionFilter failed rc=%d:%s", rc, strrc(rc));
        delete condition_filter;
        for (DefaultConditionFilter *&filter : condition_filters)
        {
          delete filter;
        }
        return rc;
      }
      condition_filters.push_back(condition_filter);
    }
  }
   /**
   //03 过滤算法描述：
    //前提是多表查询
    //1 多表的寻找join 条件
    //2 选择 ==left ｜｜ right字段
    //3 调用schema_add_field
    // select t1.* , t2.name from t1,t2 where t1.id=t2.id;

    if (selects.relation_num > 1)
    {
      for (size_t i = 0; i < selects.condition_num; i++)
      {
        const Condition &condition = selects.conditions[i];
        if (
            (condition.left_is_attr == 1 && condition.right_is_attr == 1 &&
             condition.comp == EQUAL_TO) // 左右都是属性名，并且表名都符合
        )
        {

          if (match_table(selects, condition.left_attr.relation_name, table_name))
          {

            // 列出这张表相关字段
            RC rc = schema_add_field(table, condition.left_attr.attribute_name, schema);
            if (rc != RC::SUCCESS)
            {
              return rc;
            }
          }
          else if (match_table(selects, condition.right_attr.relation_name, table_name))
          {
            // 列出这张表相关字段
            RC rc = schema_add_field(table, condition.right_attr.attribute_name, schema);
            if (rc != RC::SUCCESS)
            {
              return rc;
            }
          }
          else
          {
            LOG_INFO(" 表不存在 ");
          }
        }
      }
    }**/

  return select_node.init(trx, table, std::move(schema), std::move(condition_filters));
}
//检查字段是否合法
static RC schema_add_field_function(Table *table, const char *field_name, TupleSchema &schema,FunctionType ft)
{  
  if(nullptr == field_name)
  { 
    LOG_WARN("schema_add_field_function");
    return RC::SCHEMA_FIELD_MISSING;
  }
  //select count(id) from t;
  //[更错错误 需要测试] 
  if (0 == strcmp("*", field_name))
  {    
      LOG_WARN("schema_add_field_function");
      return RC::SCHEMA_FIELD_MISSING;
  }
  const FieldMeta *field_meta = table->table_meta().field(field_name);
  if (nullptr == field_meta)
  {
    LOG_WARN("No such field. %s.%s", table->name(), field_name);
    return RC::SCHEMA_FIELD_MISSING;
  }
  //避免 t.id,t.id
  schema.add_if_not_exists(field_meta->type(), table->name(), field_meta->name(),ft);
  return RC::SUCCESS;
}
