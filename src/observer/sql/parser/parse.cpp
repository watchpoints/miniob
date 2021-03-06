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

#include <mutex>
#include "sql/parser/parse.h"
#include "rc.h"
#include "common/log/log.h"
#include "common/math/regex.h"
RC parse(char *st, Query *sqln);

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
void inserts_init_appends_rows_length(Inserts *inserts)
{
    if (nullptr == inserts)
    {
      return;
    }
    inserts->left_num++;

    LOG_INFO("inserts_init_appends_rows rows=%d", inserts->left_num);

    //inserts_init_appends_rows(&CONTEXT->ssql->sstr.insertion);
}

 void inserts_init_appends_rows_values(Inserts *inserts, Value values[], size_t value_num)
  {
    if (nullptr == inserts)
    {
      return;
    }
    assert(value_num <= sizeof(inserts->values) / sizeof(inserts->values[0]));

    int rows = inserts->left_num;
    InsertLeft &pcur = inserts->left_insert[rows]; //rows
    //pcur.relation_name = strdup(relation_name);
    

    for (size_t i = 0; i < value_num; i++)
    {
      pcur.values[i] = values[i];
      LOG_INFO("inserts_init_appends_rows_values rows=%d,cols=%d,type=%d", rows, i,  values[i].type);

    }
    pcur.value_num = value_num;
  }

   void inserts_init_table_name(Inserts *inserts,const char *relation_name)
  {
    for(size_t rows =0;rows <inserts->left_num;rows++)
    {
      inserts->left_insert[rows].relation_name =strdup(relation_name);
    }
  }
  
bool removeLastZero(char *numstr)
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
        else if ('.' == numstr[i]) // ????????????????????????
        {
            numstr[i] = '\0';
            break;
        }
        else // ???????????????????????????
        {
            break;
        }
    }

    return true;
}

void create_index_init_multi(CreateIndex *create_index, const char *index_name, 
                       const char *relation_name ) {
  create_index->index_name = strdup(index_name);
  create_index->relation_name = strdup(relation_name);
  //create_index->attribute_name = strdup(attr_name);
}
void create_index_append_attribute(CreateIndex *create_index, const char *attr_name) {
    create_index->attributes[create_index->attr_num++] = strdup(attr_name);;
}
void relation_attr_init(RelAttr *relation_attr, const char *relation_name, const char *attribute_name) {
  if (relation_name != nullptr) {
    relation_attr->relation_name = strdup(relation_name);
  } else {
    relation_attr->relation_name = nullptr;
  }
  LOG_INFO("relation_attr_init >>>>>>> attribute_name=%s, func_op=%d,com_op=%d",attribute_name,relation_attr->funtype,relation_attr->is_asc);
  relation_attr->attribute_name = strdup(attribute_name);
}
//??????????????? count???1???
void relation_attr_init_number(RelAttr *relation_attr, const char *relation_name, int attribute_name) {
  if (relation_name != nullptr) {
    relation_attr->relation_name = strdup(relation_name);
  } else {
    relation_attr->relation_name = nullptr;
  }
  //std::cout<< "relation_attr_init_number" <<attribute_name<<std::endl;
  std::string str = std::to_string(attribute_name);
  relation_attr->attribute_name = strdup(str.c_str());
}
void relation_attr_destroy(RelAttr *relation_attr) {
  free(relation_attr->relation_name);
  free(relation_attr->attribute_name);
  relation_attr->relation_name = nullptr;
  relation_attr->attribute_name = nullptr;
}

void value_init_integer(Value *value, int v) {
  value->type = INTS;
  value->data = malloc(sizeof(v));
  memcpy(value->data, &v, sizeof(v));
}

void value_init_float(Value *value, float v) {
  value->type = FLOATS;
  value->data = malloc(sizeof(v));
  memcpy(value->data, &v, sizeof(v));
}
/**
void value_init_float(Value *value, float v) {
   value->type = FLOATS;

    char str[1024];
    sprintf(str, "%.2f", v);
    std::cout<< "a: "<<v <<"str:"<<str<<std::endl;

    removeLastZero(str);
    std::cout <<"str:"<< str << std::endl;

    // ??????????????????????????????
    float num_float = std::stof(str);

    std::cout<< "num_float: "<<num_float<<std::endl;

   value->data = malloc(sizeof(num_float));
   memcpy(value->data, &num_float, sizeof(num_float));
}**/
//??????????????????????????????????????? ?????????????????? ??????????????????????????????
void value_init_string(Value *value, const char *v) {
     value->type = CHARS;
     value->data = strdup(v);
}
//?????????????????? ????????????null
void value_init_null_value(Value *value) {
     value->type = NULLVALUES;
     //value->data = nullptr;
     value->data = strdup("999");
     //null_value ????????????
     LOG_INFO("???????????????NULL?????? value_init_null_value");
}
void value_init_date(Value *value, int v) {
  value->type = DATES;
  value->data = malloc(sizeof(v));
  memcpy(value->data, &v, sizeof(v));
  //std::cout<<"value_init_date ="<<v <<std::endl;
}

void value_destroy(Value *value) {
  value->type = UNDEFINED;
  free(value->data);
  value->data = nullptr;
}

void condition_init(Condition *condition, CompOp comp, 
                    int left_is_attr, RelAttr *left_attr, Value *left_value,
                    int right_is_attr, RelAttr *right_attr, Value *right_value) {
  condition->comp = comp;
  condition->left_is_attr = left_is_attr;
  if (left_is_attr) {
    LOG_INFO("left_is_attr ...........");
    condition->left_attr = *left_attr;
  } else {
    condition->left_value = *left_value;
  }

  condition->right_is_attr = right_is_attr;
  if (right_is_attr) {
    LOG_INFO("right_is_attr ...........");
    condition->right_attr = *right_attr;
  } else {
    condition->right_value = *right_value;
  }
}
void condition_destroy(Condition *condition) {
  if (condition->left_is_attr) {
    relation_attr_destroy(&condition->left_attr);
  } else {
    value_destroy(&condition->left_value);
  }
  if (condition->right_is_attr) {
    relation_attr_destroy(&condition->right_attr);
  } else {
    value_destroy(&condition->right_value);
  }
}

void attr_info_init(AttrInfo *attr_info, const char *name, AttrType type, size_t length) {
  attr_info->name = strdup(name);
  attr_info->type = type;
  attr_info->length = length;
  
}
void attr_info_init_nullable(AttrInfo *attr_info, const char *name, AttrType type, size_t length,int nullable)
 {

  attr_info->name = strdup(name);
  attr_info->type = type;
  attr_info->length = length;
  attr_info->nullable = nullable; //0 ?????????null ????????????null
  //LOG_INFO("attr_info_init_nullable name:%s,nullable:%d",name,nullable);
}
void attr_info_destroy(AttrInfo *attr_info) {
  free(attr_info->name);
  attr_info->name = nullptr;
}

void selects_init(Selects *selects, ...);
void selects_append_attribute(Selects *selects, RelAttr *rel_attr) {
  selects->attributes[selects->attr_num++] = *rel_attr;
}
void selects_append_attribute_order_by(Selects *selects, RelAttr *rel_attr) {

   selects->attr_order_by[selects->attr_order_num++] = *rel_attr;
  LOG_INFO("relation_attr_init >>>>>>> attribute_name=%s, com_op=%d",rel_attr->attribute_name,rel_attr->is_asc);

   //selects->attr_order_by = *rel_attr; ?????????????????????
}
void selects_append_attribute_group_by(Selects *selects, RelAttr *rel_attr) {
  //selects->attr_group_by = *rel_attr;
  selects->attr_group_by[selects->attr_group_num++] = *rel_attr;
}
void selects_append_relation(Selects *selects, const char *relation_name) {
  selects->relations[selects->relation_num++] = strdup(relation_name);
}

void selects_append_conditions(Selects *selects, Condition conditions[], size_t condition_num) {
  assert(condition_num <= sizeof(selects->conditions)/sizeof(selects->conditions[0]));
  for (size_t i = 0; i < condition_num; i++) {
    selects->conditions[i] = conditions[i];
  }
  selects->condition_num = condition_num;
}

void selects_destroy(Selects *selects) {
  for (size_t i = 0; i < selects->attr_num; i++) {
    relation_attr_destroy(&selects->attributes[i]);
  }
  selects->attr_num = 0;

  for (size_t i = 0; i < selects->relation_num; i++) {
    free(selects->relations[i]);
    selects->relations[i] = NULL;
  }
  selects->relation_num = 0;

  for (size_t i = 0; i < selects->condition_num; i++) {
    condition_destroy(&selects->conditions[i]);
  }
  selects->condition_num = 0;

  //??????????????????????????????????????????bug ????????????
  selects->attr_order_num = 0;
  selects->attr_group_num = 0;
}

void inserts_init(Inserts *inserts, const char *relation_name, Value values[], size_t value_num) {
  assert(value_num <= sizeof(inserts->values)/sizeof(inserts->values[0]));

  inserts->relation_name = strdup(relation_name);
  for (size_t i = 0; i < value_num; i++) {
    inserts->values[i] = values[i];
  }
  inserts->value_num = value_num;
}
void inserts_destroy(Inserts *inserts) {
  free(inserts->relation_name);
  inserts->relation_name = nullptr;

  for (size_t i = 0; i < inserts->value_num; i++) {
    value_destroy(&inserts->values[i]);
  }
  inserts->value_num = 0;
}

void deletes_init_relation(Deletes *deletes, const char *relation_name) {
  deletes->relation_name = strdup(relation_name);
}

void deletes_set_conditions(Deletes *deletes, Condition conditions[], size_t condition_num) {
  assert(condition_num <= sizeof(deletes->conditions)/sizeof(deletes->conditions[0]));
  for (size_t i = 0; i < condition_num; i++) {
    deletes->conditions[i] = conditions[i];
  }
  deletes->condition_num = condition_num;
}
void deletes_destroy(Deletes *deletes) {
  for (size_t i = 0; i < deletes->condition_num; i++) {
    condition_destroy(&deletes->conditions[i]);
  }
  deletes->condition_num = 0;
  free(deletes->relation_name);
  deletes->relation_name = nullptr;
}

void updates_init(Updates *updates, const char *relation_name, const char *attribute_name,
                  Value *value, Condition conditions[], size_t condition_num) {
  updates->relation_name = strdup(relation_name);
  updates->attribute_name = strdup(attribute_name);
  updates->value = *value;

  assert(condition_num <= sizeof(updates->conditions)/sizeof(updates->conditions[0]));
  for (size_t i = 0; i < condition_num; i++) {
    updates->conditions[i] = conditions[i];
  }
  updates->condition_num = condition_num;
}

void updates_destroy(Updates *updates) {
  free(updates->relation_name);
  free(updates->attribute_name);
  updates->relation_name = nullptr;
  updates->attribute_name = nullptr;

  value_destroy(&updates->value);

  for (size_t i = 0; i < updates->condition_num; i++) {
    condition_destroy(&updates->conditions[i]);
  }
  updates->condition_num = 0;
}

void create_table_append_attribute(CreateTable *create_table, AttrInfo *attr_info) {
  create_table->attributes[create_table->attribute_count++] = *attr_info;
}
void create_table_init_name(CreateTable *create_table, const char *relation_name) {
  create_table->relation_name = strdup(relation_name);
}
void create_table_destroy(CreateTable *create_table) {
  for (size_t i = 0; i < create_table->attribute_count; i++) {
    attr_info_destroy(&create_table->attributes[i]);
  }
  create_table->attribute_count = 0;
  free(create_table->relation_name);
  create_table->relation_name = nullptr;
}

void drop_table_init(DropTable *drop_table, const char *relation_name) {
  drop_table->relation_name = strdup(relation_name);
}
void drop_table_destroy(DropTable *drop_table) {
  free(drop_table->relation_name);
  drop_table->relation_name = nullptr;
}

void create_index_init(CreateIndex *create_index, const char *index_name, 
                       const char *relation_name, const char *attr_name) {
  create_index->index_name = strdup(index_name);
  create_index->relation_name = strdup(relation_name);
  create_index->attribute_name = strdup(attr_name);
}
void create_index_destroy(CreateIndex *create_index) {
  free(create_index->index_name);
  free(create_index->relation_name);
  free(create_index->attribute_name);

  create_index->index_name = nullptr;
  create_index->relation_name = nullptr;
  create_index->attribute_name = nullptr;
}

void drop_index_init(DropIndex *drop_index, const char *index_name, 
                       const char *relation_name) {
  drop_index->index_name = strdup(index_name);
  drop_index->relation_name = strdup(relation_name);
  
}
void drop_index_destroy(DropIndex *drop_index) {
  free((char *)drop_index->index_name);
  drop_index->index_name = nullptr;
}

void desc_table_init(DescTable *desc_table, const char *relation_name) {
  desc_table->relation_name = strdup(relation_name);
}

void desc_table_destroy(DescTable *desc_table) {
  free((char *)desc_table->relation_name);
  desc_table->relation_name = nullptr;
}

void load_data_init(LoadData *load_data, const char *relation_name, const char *file_name) {
  load_data->relation_name = strdup(relation_name);

  if (file_name[0] == '\'' || file_name[0] == '\"') {
    file_name++;
  }
  char *dup_file_name = strdup(file_name);
  int len = strlen(dup_file_name);
  if (dup_file_name[len - 1] == '\'' || dup_file_name[len - 1] == '\"') {
    dup_file_name[len - 1] = 0;
  }
  load_data->file_name = dup_file_name;
}

void load_data_destroy(LoadData *load_data) {
  free((char *)load_data->relation_name);
  free((char *)load_data->file_name);
  load_data->relation_name = nullptr;
  load_data->file_name = nullptr;
}

void query_init(Query *query) {
  query->flag = SCF_ERROR;
  memset(&query->sstr, 0, sizeof(query->sstr));
}

Query *query_create() {
  Query *query = (Query *)malloc(sizeof(Query));
  if (nullptr == query) {
    LOG_ERROR("Failed to alloc memroy for query. size=%ld", sizeof(Query));
    return nullptr;
  }

  query_init(query);
  return query;
}

void query_reset(Query *query) {
  switch (query->flag) {
    case SCF_SELECT: {
      selects_destroy(&query->sstr.selection);
    }
    break;
    case SCF_INSERT: {
      inserts_destroy(&query->sstr.insertion);
    }
    break;
    case SCF_DELETE: {
      deletes_destroy(&query->sstr.deletion);
    }
    break;
    case SCF_UPDATE: {
      updates_destroy(&query->sstr.update);
    }
    break;
    case SCF_CREATE_TABLE: {
      create_table_destroy(&query->sstr.create_table);
    }
    break;
    case SCF_DROP_TABLE: {
      drop_table_destroy(&query->sstr.drop_table);
    }
    break;
    case SCF_CREATE_INDEX: {
      create_index_destroy(&query->sstr.create_index);
    }
    break;
    case SCF_CREATE_UNIQUE_INDEX: {
      create_index_destroy(&query->sstr.create_index);
    }
    break;
    case SCF_DROP_INDEX: {
      drop_index_destroy(&query->sstr.drop_index);
    }
    break;
    case SCF_SYNC: {

    }
    break;
    case SCF_SHOW_TABLES:
    break;

    case SCF_DESC_TABLE: {
      desc_table_destroy(&query->sstr.desc_table);
    }
    break;

    case SCF_LOAD_DATA: {
      load_data_destroy(&query->sstr.load_data);
    }
    break;
    case SCF_BEGIN:
    case SCF_COMMIT:
    case SCF_ROLLBACK:
    case SCF_HELP:
    case SCF_EXIT:
    case SCF_ERROR:
    break;
  }
}

void query_destroy(Query *query) {
  query_reset(query);
  free(query);
}
#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////

extern "C" int sql_parse(const char *st, Query  *sqls);

RC parse(const char *st, Query *sqln) {
  sql_parse(st, sqln);

  if (sqln->flag == SCF_ERROR)
  {
    std::cout<<"parse failed"<<st <<std::endl;
    return SQL_SYNTAX;
  }
  else
  {
    return SUCCESS;
  }
}