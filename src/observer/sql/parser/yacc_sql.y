
%{

#include "sql/parser/parse_defs.h"
#include "sql/parser/yacc_sql.tab.h"
#include "sql/parser/lex.yy.h"
// #include "common/log/log.h" // 包含C++中的头文件

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct ParserContext {
  Query * ssql;
  size_t select_length;
  size_t condition_length;
  size_t from_length;
  size_t value_length;
  Value values[MAX_NUM];
  Condition conditions[MAX_NUM];
  CompOp comp;
	char id[MAX_NUM];
} ParserContext;

//获取子串
char *substr(const char *s,int n1,int n2)/*从s中提取下标为n1~n2的字符组成一个新字符串，然后返回这个新串的首地址*/
{
  char *sp = malloc(sizeof(char) * (n2 - n1 + 2));
  int i, j = 0;
  for (i = n1; i <= n2; i++) {
    sp[j++] = s[i];
  }
  sp[j] = 0;
  return sp;
}

void yyerror(yyscan_t scanner, const char *str)
{
  ParserContext *context = (ParserContext *)(yyget_extra(scanner));
  query_reset(context->ssql);
  context->ssql->flag = SCF_ERROR;
  context->condition_length = 0;
  context->from_length = 0;
  context->select_length = 0;
  context->value_length = 0;
  context->ssql->sstr.insertion.value_num = 0;
  printf("parse sql failed. error=%s", str);
}

ParserContext *get_context(yyscan_t scanner)
{
  return (ParserContext *)yyget_extra(scanner);
}

#define CONTEXT get_context(scanner)

%}

%define api.pure full
%lex-param { yyscan_t scanner }
%parse-param { void *scanner }

//标识tokens
%token  SEMICOLON
        CREATE
        DROP
        TABLE
        TABLES
        INDEX
        SELECT
        DESC
        SHOW
        SYNC
        INSERT
        DELETE
        UPDATE
        LBRACE
        RBRACE
        COMMA
        TRX_BEGIN
        TRX_COMMIT
        TRX_ROLLBACK
        INT_T
        STRING_T
        FLOAT_T
        HELP
        EXIT
        DOT //QUOTE
        INTO
        VALUES
        FROM
        WHERE
        AND
        SET
        ON
        LOAD
        DATA
        INFILE
        EQ
        LT
        GT
        LE
        GE
        NE
		DATE_T
		COUNT
		MAX
		MIN
		AVG
		UNIQUE
		NULLABLE
		NULLVALUE
		NOT
		IS
		TEXT_T
		ORDER
		GROUP
		BY
		ASC

%union {
  struct _Attr *attr;
  struct _Condition *condition1;
  struct _Value *value1;
  char *string;
  int number;
  float floats;
  char *position;
}

%token <number> NUMBER
%token <floats> FLOAT 
%token <string> ID
%token <string> PATH
%token <string> SSS
%token <string> STAR
%token <string> STRING_V
//非终结符

%type <number> type;
%type <condition1> condition;
%type <value1> value;
%type <number> number;

%%

commands:		//commands or sqls. parser starts here.
    /* empty */
    | commands command
    ;

command:
	  select  
	| insert
	| update
	| delete
	| create_table
	| drop_table
	| show_tables
	| desc_table
	| create_index
	| create_unique_index		
	| drop_index
	| sync
	| begin
	| commit
	| rollback
	| load_data
	| help
	| exit
    ;

exit:			
    EXIT SEMICOLON {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    };

help:
    HELP SEMICOLON {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    };

sync:
    SYNC SEMICOLON {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
    ;

begin:
    TRX_BEGIN SEMICOLON {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
    ;

commit:
    TRX_COMMIT SEMICOLON {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
    ;

rollback:
    TRX_ROLLBACK SEMICOLON {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
    ;

drop_table:		/*drop table 语句的语法解析树*/
    DROP TABLE ID SEMICOLON {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, $3);
    };

show_tables:
    SHOW TABLES SEMICOLON {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
    ;

desc_table:
    DESC ID SEMICOLON {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, $2);
    }
    ;

create_index:		/*create index 语句的语法解析树*/
    //CREATE INDEX ID ON ID LBRACE ID RBRACE SEMICOLON 
	  CREATE INDEX ID ON ID LBRACE multi multi_list RBRACE SEMICOLON 
		{
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			//create_index_init(&CONTEXT->ssql->sstr.create_index, $3, $5, $7);
			create_index_init_multi(&CONTEXT->ssql->sstr.create_index, $3, $5);
		}
    ;
multi_list:
    /* empty */
    | COMMA multi multi_list  { 
	  }
    ;
multi:
    ID 
	{	
		create_index_append_attribute(&CONTEXT->ssql->sstr.create_index,$1);
	 }
    ;
create_unique_index:		/*create unique index 语句的语法解析树*/
    CREATE UNIQUE INDEX ID ON ID LBRACE ID RBRACE SEMICOLON 
		{
			CONTEXT->ssql->flag = SCF_CREATE_UNIQUE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, $4, $6, $8);
		}
    ;

drop_index:			/*drop index 语句的语法解析树*/
    DROP INDEX ID  ON ID SEMICOLON 
		{
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, $3, $5);
		}
    ;
create_table:		/*create table 语句的语法解析树*/
    CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE SEMICOLON 
		{
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			// CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length;
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, $3);
			//临时变量清零	
			CONTEXT->value_length = 0;
		}
    ;
attr_def_list:
    /* empty */
    | COMMA attr_def attr_def_list {    }
    ;
    
attr_def:
    ID_get type LBRACE number RBRACE 
		{
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, $2, $4);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name =(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type = $2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length = $4;
			CONTEXT->value_length++;
		}
    |ID_get type
		{
			AttrInfo attribute;
			attr_info_init_nullable(&attribute, CONTEXT->id, $2, 4,0);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name=(char*)malloc(sizeof(char));
			// stcrpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type=$2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length=10; // default attribute length
			CONTEXT->value_length++;
		}
	 |ID_get type NOT NULLVALUE
		{
			AttrInfo attribute;
			attr_info_init_nullable(&attribute, CONTEXT->id, $2, 4,0);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name=(char*)malloc(sizeof(char));
			// stcrpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type=$2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length=10; // default attribute length
			CONTEXT->value_length++;
		}
	|ID_get type NULLABLE
		{
			AttrInfo attribute;
			attr_info_init_nullable(&attribute, CONTEXT->id, $2, 4,1);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name=(char*)malloc(sizeof(char));
			// stcrpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type=$2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length=10; // default attribute length
			CONTEXT->value_length++;
		}
    ;
number:
		NUMBER {$$ = $1;}
		;
type:
	INT_T { $$=INTS; }
       | STRING_T { $$=CHARS; }
       | FLOAT_T { $$=FLOATS; }
	   | DATE_T { $$=DATES; }
	   | TEXT_T { $$=TEXTS; }
       ;
ID_get:
	ID 
	{
		char *temp=$1; 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
	;

	
insert:				/*insert   语句的语法解析树*/
    //INSERT INTO ID VALUES LBRACE value value_list RBRACE SEMICOLON 
	INSERT INTO ID VALUES tuple tuple_list SEMICOLON 
	{
			// CONTEXT->values[CONTEXT->value_length++] = *$6;

			CONTEXT->ssql->flag=SCF_INSERT;//"insert";
			// CONTEXT->ssql->sstr.insertion.relation_name = $3;
			// CONTEXT->ssql->sstr.insertion.value_num = CONTEXT->value_length;
			// for(i = 0; i < CONTEXT->value_length; i++){
			// 	CONTEXT->ssql->sstr.insertion.values[i] = CONTEXT->values[i];
      // }
			//inserts_init(&CONTEXT->ssql->sstr.insertion, $3, CONTEXT->values, CONTEXT->value_length);
			inserts_init_table_name(&CONTEXT->ssql->sstr.insertion, $3);

      //临时变量清零
      CONTEXT->value_length=0;
    };
tuple:
	LBRACE value value_list RBRACE
	{
       //inserts_init(&CONTEXT->ssql->sstr.insertion, $3, CONTEXT->values, CONTEXT->value_length);

	   	inserts_init_appends_rows_values(&CONTEXT->ssql->sstr.insertion,CONTEXT->values,CONTEXT->value_length);

		//插入一行记录，长度增加一行
		inserts_init_appends_rows_length(&CONTEXT->ssql->sstr.insertion);
		CONTEXT->value_length = 0;


	};
tuple_list:
  /* empty */
    | COMMA tuple tuple_list  { 
	  }
    ;
value_list:
    /* empty */
    | COMMA value value_list  { 
  		// CONTEXT->values[CONTEXT->value_length++] = *$2;
	  }
    ;
value:
    NUMBER{	
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], $1);
		}
    |FLOAT{
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], $1);
		}
    |SSS {
			$1 = substr($1,1,strlen($1)-2);
  		value_init_string(&CONTEXT->values[CONTEXT->value_length++], $1);
		}
	|NULLVALUE 
	{
  		value_init_null_value(&CONTEXT->values[CONTEXT->value_length++]);
	}
    ;
    
delete:		/*  delete 语句的语法解析树*/
    DELETE FROM ID where SEMICOLON 
		{
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, $3);
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;	
    }
    ;
update:			/*  update 语句的语法解析树*/
    UPDATE ID SET ID EQ value where SEMICOLON
		{
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			Value *value = &CONTEXT->values[0];
			updates_init(&CONTEXT->ssql->sstr.update, $2, $4, value, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
    ;
select:				/*  select 语句的语法解析树*/
    SELECT select_attr FROM ID rel_list where group_list SEMICOLON
		{
			// CONTEXT->ssql->sstr.selection.relations[CONTEXT->from_length++]=$4;
			selects_append_relation(&CONTEXT->ssql->sstr.selection, $4);

			selects_append_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->conditions, CONTEXT->condition_length);

			CONTEXT->ssql->flag=SCF_SELECT;//"select";
			// CONTEXT->ssql->sstr.selection.attr_num = CONTEXT->select_length;

			//临时变量清零
			CONTEXT->condition_length=0;
			CONTEXT->from_length=0;
			CONTEXT->select_length=0;
			CONTEXT->value_length = 0;
	}
	;

select_attr:
    STAR {  
			RelAttr attr;
			attr.funtype=FUN_NO;
			relation_attr_init(&attr, NULL, "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
    | ID attr_list {
			RelAttr attr;
			attr.funtype=FUN_NO;
			relation_attr_init(&attr, NULL, $1);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
  	| ID DOT ID attr_list {
			RelAttr attr;
			attr.funtype=FUN_NO;
			//单表才有聚合
			relation_attr_init(&attr, $1, $3);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
	| ID DOT STAR attr_list {
			RelAttr attr;
			attr.funtype=FUN_NO;
			//单表才有聚合
			relation_attr_init(&attr, $1,  "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
	| COUNT LBRACE STAR RBRACE attr_list {
			RelAttr attr;
			attr.funtype=FUN_COUNT_ALL;
			relation_attr_init(&attr, NULL,  "*");
			
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
	| COUNT LBRACE ID RBRACE attr_list {
			RelAttr attr;
			attr.funtype=FUN_COUNT;
			relation_attr_init(&attr, NULL,$3);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
	| COUNT LBRACE ID DOT ID RBRACE attr_list {
			RelAttr attr;
			attr.funtype=FUN_COUNT;
			relation_attr_init(&attr, $3,$5);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
	| MAX LBRACE ID RBRACE attr_list {
			RelAttr attr;
			attr.funtype=FUN_MAX;
			relation_attr_init(&attr, NULL,$3);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
	| MAX LBRACE ID DOT ID RBRACE attr_list {
			RelAttr attr;
			attr.funtype=FUN_MAX;
			relation_attr_init(&attr, $3,$5);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
	| MIN LBRACE ID RBRACE attr_list {
			RelAttr attr;
			attr.funtype=FUN_MIN;
			relation_attr_init(&attr, NULL,$3);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
	| MIN LBRACE ID DOT ID RBRACE attr_list {
			RelAttr attr;
			attr.funtype=FUN_MIN;
			relation_attr_init(&attr, $3,$5);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
	| AVG LBRACE ID RBRACE attr_list {
			RelAttr attr;
			attr.funtype=FUN_AVG;
			relation_attr_init(&attr, NULL,$3);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
	| AVG LBRACE ID DOT ID  RBRACE attr_list {
			RelAttr attr;
			attr.funtype=FUN_AVG;
			relation_attr_init(&attr, $3,$5);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
	| COUNT LBRACE number RBRACE attr_list {
			RelAttr attr;
			attr.funtype=FUN_COUNT_ALL;
			relation_attr_init_number(&attr, NULL,$3);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
    ;
attr_list:
    /* empty */
    | COMMA ID attr_list {
			RelAttr attr;
			attr.funtype=FUN_NO;
			relation_attr_init(&attr, NULL, $2);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
    | COMMA ID DOT ID attr_list {
			RelAttr attr;
			attr.funtype=FUN_NO;
			//单表才有聚合
			relation_attr_init(&attr, $2, $4);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
	 | COMMA ID DOT STAR attr_list {
			RelAttr attr;
			attr.funtype=FUN_NO;
			relation_attr_init(&attr, $2, "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
	| COMMA COUNT LBRACE STAR RBRACE attr_list {
			RelAttr attr;
			attr.funtype=FUN_COUNT_ALL;
			relation_attr_init(&attr, NULL,  "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
	| COMMA COUNT LBRACE ID RBRACE attr_list {
		RelAttr attr;
		attr.funtype=FUN_COUNT;
		relation_attr_init(&attr, NULL,  $4);
		selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	// CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
	// CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
	}
	| COMMA COUNT LBRACE ID DOT ID RBRACE attr_list {
		RelAttr attr;
		attr.funtype=FUN_COUNT;
		relation_attr_init(&attr, $4,$6);
		selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	}
	| COMMA MAX LBRACE ID RBRACE attr_list {
		RelAttr attr;
		attr.funtype=FUN_MAX;
		relation_attr_init(&attr, NULL,  $4);
		selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	// CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
	// CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
	}
	| COMMA MAX LBRACE ID DOT ID RBRACE attr_list {
		RelAttr attr;
		attr.funtype=FUN_MAX;
		relation_attr_init(&attr,$4,$6);
		selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	}
	| COMMA MIN LBRACE ID RBRACE attr_list {
		RelAttr attr;
		attr.funtype=FUN_MIN;
		relation_attr_init(&attr, NULL,  $4);
		selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	// CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
	// CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
	}
	| COMMA MIN LBRACE ID DOT ID  RBRACE attr_list {
		RelAttr attr;
		attr.funtype=FUN_MIN;
		relation_attr_init(&attr, $4, $6);
		selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	}
	| COMMA AVG LBRACE ID RBRACE attr_list {
		RelAttr attr;
		attr.funtype=FUN_AVG;
		relation_attr_init(&attr, NULL,  $4);
		selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	// CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
	// CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
	}
	| COMMA AVG LBRACE ID DOT ID RBRACE attr_list {
		RelAttr attr;
		attr.funtype=FUN_AVG;
		relation_attr_init(&attr, $4, $6);
		selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	}
	| COMMA COUNT LBRACE number RBRACE attr_list {
			RelAttr attr;
			attr.funtype=FUN_COUNT_ALL;
			relation_attr_init_number(&attr, NULL, $4); 
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
  	;

rel_list:
    /* empty */
    | COMMA ID rel_list {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, $2);
		  }
    ;
group_list:
	/* empty */
	| ORDER BY order_by_attr
	{

	}
	| GROUP BY group_by_attr
	{
	}
	;
order_by_attr:
 /* empty */ 
  | ID order_by_attr_list 
  {
		RelAttr attr;
		attr.funtype=FUN_ORDER_BY;
		attr.is_asc =ORDER_ASC;
		//默认升序(asc)
		relation_attr_init(&attr, NULL, $1); 
		selects_append_attribute_order_by(&CONTEXT->ssql->sstr.selection, &attr);
  }
  | ID ASC order_by_attr_list
	{
		RelAttr attr;
		attr.funtype=FUN_ORDER_BY;
		attr.is_asc =ORDER_ASC;
		//默认升序(asc)
		relation_attr_init(&attr, NULL, $1); 
		selects_append_attribute_order_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
   | ID DESC order_by_attr_list
	{
		RelAttr attr;
		attr.funtype=FUN_ORDER_BY;
		attr.is_asc =ORDER_DESC;
		//默认升序(asc)
		relation_attr_init(&attr, NULL, $1); 
		selects_append_attribute_order_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
   | ID DOT ID order_by_attr_list
	{
		RelAttr attr;
		attr.funtype=FUN_ORDER_BY;
		attr.is_asc =ORDER_ASC;
		relation_attr_init(&attr, $1, $3); 
		selects_append_attribute_order_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
	| ID DOT ID ASC order_by_attr_list
	{
		RelAttr attr;
		attr.funtype=FUN_ORDER_BY;
		attr.is_asc =ORDER_ASC;
		relation_attr_init(&attr, $1, $3); 
		selects_append_attribute_order_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
	| ID DOT ID DESC order_by_attr_list
	{
		RelAttr attr;
		attr.funtype=FUN_ORDER_BY;
		attr.is_asc =ORDER_DESC;
		relation_attr_init(&attr, $1, $3); 
		selects_append_attribute_order_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
  ;
  order_by_attr_list:
    /* empty */
    | COMMA ID order_by_attr_list 
	{
		RelAttr attr;
		attr.funtype=FUN_ORDER_BY;
		attr.is_asc =ORDER_ASC;
		//默认升序(asc)
		relation_attr_init(&attr, NULL, $2); 
		selects_append_attribute_order_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
	| COMMA ID ASC order_by_attr_list
	{
		RelAttr attr;
		attr.funtype=FUN_ORDER_BY;
		attr.is_asc =ORDER_ASC;
		//默认升序(asc)
		relation_attr_init(&attr, NULL, $2); 
		selects_append_attribute_order_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
	| COMMA ID DESC order_by_attr_list
	{
		RelAttr attr;
		attr.funtype=FUN_ORDER_BY;
		attr.is_asc =ORDER_DESC;
		//默认升序(asc)
		relation_attr_init(&attr, NULL, $2); 
		selects_append_attribute_order_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
	| COMMA ID DOT ID order_by_attr_list
	{
		RelAttr attr;
		attr.funtype=FUN_ORDER_BY;
		attr.is_asc =ORDER_ASC;
		relation_attr_init(&attr, $2, $4); 
		selects_append_attribute_order_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
	| COMMA ID DOT ID ASC order_by_attr_list
	{
		RelAttr attr;
		attr.funtype=FUN_ORDER_BY;
		attr.is_asc =ORDER_ASC;
		relation_attr_init(&attr, $2, $4); 
		selects_append_attribute_order_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
	| COMMA ID DOT ID DESC order_by_attr_list
	{
		RelAttr attr;
		attr.funtype=FUN_ORDER_BY;
		attr.is_asc =ORDER_DESC;
		relation_attr_init(&attr, $2, $4); 
		selects_append_attribute_order_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
	;

group_by_attr:
 /* empty */ 
  | ID group_by_attr_list 
  {
		RelAttr attr;
		attr.funtype=FUN_GROUP_BY;
		attr.is_asc =ORDER_ASC;
		//默认升序(asc)
		relation_attr_init(&attr, NULL, $1); 
		selects_append_attribute_group_by(&CONTEXT->ssql->sstr.selection, &attr);
  }
  | ID comOp group_by_attr_list
	{
		RelAttr attr;
		attr.funtype=FUN_GROUP_BY;
		attr.is_asc =CONTEXT->comp;
		//默认升序(asc)
		relation_attr_init(&attr, NULL, $1); 
		selects_append_attribute_group_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
   | ID DOT ID group_by_attr_list
	{
		RelAttr attr;
		attr.funtype=FUN_GROUP_BY;
		attr.is_asc =ORDER_ASC;
		relation_attr_init(&attr, $1, $3); 
		selects_append_attribute_group_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
	| ID DOT ID comOp group_by_attr_list
	{
		RelAttr attr;
		attr.funtype=FUN_GROUP_BY;
		attr.is_asc =CONTEXT->comp;
		relation_attr_init(&attr, $1, $3); 
		selects_append_attribute_group_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
  ;
  group_by_attr_list:
    /* empty */
    | COMMA ID group_by_attr_list 
	{
		RelAttr attr;
		attr.funtype=FUN_GROUP_BY;
		attr.is_asc =ORDER_ASC;
		//默认升序(asc)
		relation_attr_init(&attr, NULL, $2); 
		selects_append_attribute_group_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
	| COMMA ID comOp group_by_attr_list
	{
		RelAttr attr;
		attr.funtype=FUN_GROUP_BY;
		attr.is_asc =CONTEXT->comp;
		//默认升序(asc)
		relation_attr_init(&attr, NULL, $2); 
		selects_append_attribute_group_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
	| COMMA ID DOT ID group_by_attr_list
	{
		RelAttr attr;
		attr.funtype=FUN_GROUP_BY;
		attr.is_asc =ORDER_ASC;
		relation_attr_init(&attr, $2, $4); 
		selects_append_attribute_group_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
	| COMMA ID DOT ID comOp group_by_attr_list
	{
		RelAttr attr;
		attr.funtype=FUN_GROUP_BY;
		attr.is_asc =CONTEXT->comp;
		relation_attr_init(&attr, $2, $4); 
		selects_append_attribute_group_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
	;
where:
    /* empty */ 
    | WHERE condition condition_list {	
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
    ;
condition_list:
    /* empty */
    | AND condition condition_list {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
    ;
condition:
    ID comOp value 
		{
			RelAttr left_attr;
			left_attr.funtype=FUN_NO;
			relation_attr_init(&left_attr, NULL, $1);

			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$ = ( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;
			// $$->left_attr.relation_name = NULL;
			// $$->left_attr.attribute_name= $1;
			// $$->comp = CONTEXT->comp;
			// $$->right_is_attr = 0;
			// $$->right_attr.relation_name = NULL;
			// $$->right_attr.attribute_name = NULL;
			// $$->right_value = *$3;

		}
		|value comOp value 
		{
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 2];
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$ = ( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 0;
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=NULL;
			// $$->left_value = *$1;
			// $$->comp = CONTEXT->comp;
			// $$->right_is_attr = 0;
			// $$->right_attr.relation_name = NULL;
			// $$->right_attr.attribute_name = NULL;
			// $$->right_value = *$3;

		}
		|ID comOp ID 
		{
			RelAttr left_attr;
			left_attr.funtype=FUN_NO;
			relation_attr_init(&left_attr, NULL, $1);
			RelAttr right_attr;
			right_attr.funtype=FUN_NO;
			relation_attr_init(&right_attr, NULL, $3);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=$1;
			// $$->comp = CONTEXT->comp;
			// $$->right_is_attr = 1;
			// $$->right_attr.relation_name=NULL;
			// $$->right_attr.attribute_name=$3;

		}
    |value comOp ID
		{
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];
			RelAttr right_attr;
			right_attr.funtype=FUN_NO;
			relation_attr_init(&right_attr, NULL, $3);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;

			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 0;
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=NULL;
			// $$->left_value = *$1;
			// $$->comp=CONTEXT->comp;
			
			// $$->right_is_attr = 1;
			// $$->right_attr.relation_name=NULL;
			// $$->right_attr.attribute_name=$3;
		
		}
    |ID DOT ID comOp value
		{
			RelAttr left_attr;
			left_attr.funtype=FUN_NO;
			relation_attr_init(&left_attr, $1, $3);
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;

			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;
			// $$->left_attr.relation_name=$1;
			// $$->left_attr.attribute_name=$3;
			// $$->comp=CONTEXT->comp;
			// $$->right_is_attr = 0;   //属性值
			// $$->right_attr.relation_name=NULL;
			// $$->right_attr.attribute_name=NULL;
			// $$->right_value =*$5;			
							
    }
    |value comOp ID DOT ID
		{
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];

			RelAttr right_attr;
			right_attr.funtype=FUN_NO;
			relation_attr_init(&right_attr, $3, $5);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 0;//属性值
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=NULL;
			// $$->left_value = *$1;
			// $$->comp =CONTEXT->comp;
			// $$->right_is_attr = 1;//属性
			// $$->right_attr.relation_name = $3;
			// $$->right_attr.attribute_name = $5;
									
    }
    |ID DOT ID comOp ID DOT ID
		{
			RelAttr left_attr;
			left_attr.funtype=FUN_NO;
			relation_attr_init(&left_attr, $1, $3);
			RelAttr right_attr;
			right_attr.funtype=FUN_NO;
			relation_attr_init(&right_attr, $5, $7);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;		//属性
			// $$->left_attr.relation_name=$1;
			// $$->left_attr.attribute_name=$3;
			// $$->comp =CONTEXT->comp;
			// $$->right_is_attr = 1;		//属性
			// $$->right_attr.relation_name=$5;
			// $$->right_attr.attribute_name=$7;
    }	
    ;

comOp:
  	  EQ { CONTEXT->comp = EQUAL_TO; }
    | LT { CONTEXT->comp = LESS_THAN; }
    | GT { CONTEXT->comp = GREAT_THAN; }
    | LE { CONTEXT->comp = LESS_EQUAL; }
    | GE { CONTEXT->comp = GREAT_EQUAL; }
    | NE { CONTEXT->comp = NOT_EQUAL; }
	| IS { CONTEXT->comp = IS_NULL; }
	| IS NOT { CONTEXT->comp = IS_NOT_NULL; }
    ;

load_data:
		LOAD DATA INFILE SSS INTO TABLE ID SEMICOLON
		{
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, $7, $4);
		}
		;
%%
//_____________________________________________________________________
extern void scan_string(const char *str, yyscan_t scanner);

int sql_parse(const char *s, Query *sqls){
	ParserContext context;
	memset(&context, 0, sizeof(context));

	yyscan_t scanner;
	yylex_init_extra(&context, &scanner);
	context.ssql = sqls;
	scan_string(s, scanner);
	int result = yyparse(scanner);
	yylex_destroy(scanner);
	return result;
}
