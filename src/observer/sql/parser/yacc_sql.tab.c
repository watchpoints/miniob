/* A Bison parser, made by GNU Bison 3.7.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.7"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 2 "yacc_sql.y"


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


#line 128 "yacc_sql.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "yacc_sql.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_SEMICOLON = 3,                  /* SEMICOLON  */
  YYSYMBOL_CREATE = 4,                     /* CREATE  */
  YYSYMBOL_DROP = 5,                       /* DROP  */
  YYSYMBOL_TABLE = 6,                      /* TABLE  */
  YYSYMBOL_TABLES = 7,                     /* TABLES  */
  YYSYMBOL_INDEX = 8,                      /* INDEX  */
  YYSYMBOL_SELECT = 9,                     /* SELECT  */
  YYSYMBOL_DESC = 10,                      /* DESC  */
  YYSYMBOL_SHOW = 11,                      /* SHOW  */
  YYSYMBOL_SYNC = 12,                      /* SYNC  */
  YYSYMBOL_INSERT = 13,                    /* INSERT  */
  YYSYMBOL_DELETE = 14,                    /* DELETE  */
  YYSYMBOL_UPDATE = 15,                    /* UPDATE  */
  YYSYMBOL_LBRACE = 16,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 17,                    /* RBRACE  */
  YYSYMBOL_COMMA = 18,                     /* COMMA  */
  YYSYMBOL_TRX_BEGIN = 19,                 /* TRX_BEGIN  */
  YYSYMBOL_TRX_COMMIT = 20,                /* TRX_COMMIT  */
  YYSYMBOL_TRX_ROLLBACK = 21,              /* TRX_ROLLBACK  */
  YYSYMBOL_INT_T = 22,                     /* INT_T  */
  YYSYMBOL_STRING_T = 23,                  /* STRING_T  */
  YYSYMBOL_FLOAT_T = 24,                   /* FLOAT_T  */
  YYSYMBOL_HELP = 25,                      /* HELP  */
  YYSYMBOL_EXIT = 26,                      /* EXIT  */
  YYSYMBOL_DOT = 27,                       /* DOT  */
  YYSYMBOL_INTO = 28,                      /* INTO  */
  YYSYMBOL_VALUES = 29,                    /* VALUES  */
  YYSYMBOL_FROM = 30,                      /* FROM  */
  YYSYMBOL_WHERE = 31,                     /* WHERE  */
  YYSYMBOL_AND = 32,                       /* AND  */
  YYSYMBOL_SET = 33,                       /* SET  */
  YYSYMBOL_ON = 34,                        /* ON  */
  YYSYMBOL_LOAD = 35,                      /* LOAD  */
  YYSYMBOL_DATA = 36,                      /* DATA  */
  YYSYMBOL_INFILE = 37,                    /* INFILE  */
  YYSYMBOL_EQ = 38,                        /* EQ  */
  YYSYMBOL_LT = 39,                        /* LT  */
  YYSYMBOL_GT = 40,                        /* GT  */
  YYSYMBOL_LE = 41,                        /* LE  */
  YYSYMBOL_GE = 42,                        /* GE  */
  YYSYMBOL_NE = 43,                        /* NE  */
  YYSYMBOL_DATE_T = 44,                    /* DATE_T  */
  YYSYMBOL_COUNT = 45,                     /* COUNT  */
  YYSYMBOL_MAX = 46,                       /* MAX  */
  YYSYMBOL_MIN = 47,                       /* MIN  */
  YYSYMBOL_AVG = 48,                       /* AVG  */
  YYSYMBOL_UNIQUE = 49,                    /* UNIQUE  */
  YYSYMBOL_NULLABLE = 50,                  /* NULLABLE  */
  YYSYMBOL_NULLVALUE = 51,                 /* NULLVALUE  */
  YYSYMBOL_NOT = 52,                       /* NOT  */
  YYSYMBOL_IS = 53,                        /* IS  */
  YYSYMBOL_TEXT_T = 54,                    /* TEXT_T  */
  YYSYMBOL_ORDER = 55,                     /* ORDER  */
  YYSYMBOL_GROUP = 56,                     /* GROUP  */
  YYSYMBOL_BY = 57,                        /* BY  */
  YYSYMBOL_ASC = 58,                       /* ASC  */
  YYSYMBOL_NUMBER = 59,                    /* NUMBER  */
  YYSYMBOL_FLOAT = 60,                     /* FLOAT  */
  YYSYMBOL_ID = 61,                        /* ID  */
  YYSYMBOL_PATH = 62,                      /* PATH  */
  YYSYMBOL_SSS = 63,                       /* SSS  */
  YYSYMBOL_STAR = 64,                      /* STAR  */
  YYSYMBOL_STRING_V = 65,                  /* STRING_V  */
  YYSYMBOL_YYACCEPT = 66,                  /* $accept  */
  YYSYMBOL_commands = 67,                  /* commands  */
  YYSYMBOL_command = 68,                   /* command  */
  YYSYMBOL_exit = 69,                      /* exit  */
  YYSYMBOL_help = 70,                      /* help  */
  YYSYMBOL_sync = 71,                      /* sync  */
  YYSYMBOL_begin = 72,                     /* begin  */
  YYSYMBOL_commit = 73,                    /* commit  */
  YYSYMBOL_rollback = 74,                  /* rollback  */
  YYSYMBOL_drop_table = 75,                /* drop_table  */
  YYSYMBOL_show_tables = 76,               /* show_tables  */
  YYSYMBOL_desc_table = 77,                /* desc_table  */
  YYSYMBOL_create_index = 78,              /* create_index  */
  YYSYMBOL_multi_list = 79,                /* multi_list  */
  YYSYMBOL_multi = 80,                     /* multi  */
  YYSYMBOL_create_unique_index = 81,       /* create_unique_index  */
  YYSYMBOL_drop_index = 82,                /* drop_index  */
  YYSYMBOL_create_table = 83,              /* create_table  */
  YYSYMBOL_attr_def_list = 84,             /* attr_def_list  */
  YYSYMBOL_attr_def = 85,                  /* attr_def  */
  YYSYMBOL_number = 86,                    /* number  */
  YYSYMBOL_type = 87,                      /* type  */
  YYSYMBOL_ID_get = 88,                    /* ID_get  */
  YYSYMBOL_insert = 89,                    /* insert  */
  YYSYMBOL_tuple = 90,                     /* tuple  */
  YYSYMBOL_tuple_list = 91,                /* tuple_list  */
  YYSYMBOL_value_list = 92,                /* value_list  */
  YYSYMBOL_value = 93,                     /* value  */
  YYSYMBOL_delete = 94,                    /* delete  */
  YYSYMBOL_update = 95,                    /* update  */
  YYSYMBOL_select = 96,                    /* select  */
  YYSYMBOL_select_attr = 97,               /* select_attr  */
  YYSYMBOL_attr_list = 98,                 /* attr_list  */
  YYSYMBOL_rel_list = 99,                  /* rel_list  */
  YYSYMBOL_group_list = 100,               /* group_list  */
  YYSYMBOL_order_by_attr = 101,            /* order_by_attr  */
  YYSYMBOL_order_by_attr_list = 102,       /* order_by_attr_list  */
  YYSYMBOL_group_by_attr = 103,            /* group_by_attr  */
  YYSYMBOL_group_by_attr_list = 104,       /* group_by_attr_list  */
  YYSYMBOL_where = 105,                    /* where  */
  YYSYMBOL_condition_list = 106,           /* condition_list  */
  YYSYMBOL_condition = 107,                /* condition  */
  YYSYMBOL_comOp = 108,                    /* comOp  */
  YYSYMBOL_load_data = 109                 /* load_data  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   341

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  66
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  44
/* YYNRULES -- Number of rules.  */
#define YYNRULES  130
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  295

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   320


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   147,   147,   149,   153,   154,   155,   156,   157,   158,
     159,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     169,   170,   174,   179,   184,   190,   196,   202,   208,   214,
     220,   228,   235,   237,   241,   247,   255,   262,   271,   273,
     277,   288,   299,   310,   323,   326,   327,   328,   329,   330,
     333,   343,   360,   372,   374,   377,   379,   384,   387,   390,
     394,   401,   411,   421,   440,   446,   452,   459,   466,   473,
     479,   485,   491,   497,   504,   506,   514,   523,   531,   539,
     547,   555,   563,   571,   581,   583,   587,   589,   593,   597,
     599,   608,   617,   625,   634,   636,   645,   654,   662,   672,
     674,   683,   692,   700,   709,   711,   720,   729,   737,   746,
     748,   752,   754,   759,   781,   801,   823,   846,   868,   890,
     914,   915,   916,   917,   918,   919,   920,   921,   922,   923,
     927
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "SEMICOLON", "CREATE",
  "DROP", "TABLE", "TABLES", "INDEX", "SELECT", "DESC", "SHOW", "SYNC",
  "INSERT", "DELETE", "UPDATE", "LBRACE", "RBRACE", "COMMA", "TRX_BEGIN",
  "TRX_COMMIT", "TRX_ROLLBACK", "INT_T", "STRING_T", "FLOAT_T", "HELP",
  "EXIT", "DOT", "INTO", "VALUES", "FROM", "WHERE", "AND", "SET", "ON",
  "LOAD", "DATA", "INFILE", "EQ", "LT", "GT", "LE", "GE", "NE", "DATE_T",
  "COUNT", "MAX", "MIN", "AVG", "UNIQUE", "NULLABLE", "NULLVALUE", "NOT",
  "IS", "TEXT_T", "ORDER", "GROUP", "BY", "ASC", "NUMBER", "FLOAT", "ID",
  "PATH", "SSS", "STAR", "STRING_V", "$accept", "commands", "command",
  "exit", "help", "sync", "begin", "commit", "rollback", "drop_table",
  "show_tables", "desc_table", "create_index", "multi_list", "multi",
  "create_unique_index", "drop_index", "create_table", "attr_def_list",
  "attr_def", "number", "type", "ID_get", "insert", "tuple", "tuple_list",
  "value_list", "value", "delete", "update", "select", "select_attr",
  "attr_list", "rel_list", "group_list", "order_by_attr",
  "order_by_attr_list", "group_by_attr", "group_by_attr_list", "where",
  "condition_list", "condition", "comOp", "load_data", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320
};
#endif

#define YYPACT_NINF (-135)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -135,   238,  -135,     2,    58,    83,   -47,    22,    57,    35,
      37,    13,    75,    85,    87,    88,   102,    70,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,  -135,    48,    63,   126,
      89,    90,   130,   132,   139,   141,    -2,  -135,   129,   157,
     161,  -135,   112,   114,   143,  -135,  -135,  -135,  -135,  -135,
     140,   163,   147,   121,   180,   152,    61,   138,   142,   144,
     -14,   -52,  -135,   146,  -135,  -135,   158,   159,   154,   125,
     155,   162,   167,  -135,   164,  -135,   185,   187,   196,   197,
     207,   216,   218,   219,   220,   223,    65,   222,   222,   226,
     229,    26,   252,   224,   228,  -135,   247,     4,   251,   209,
     265,   222,   222,   222,   222,   222,   222,    62,   211,   221,
     225,   -12,  -135,  -135,  -135,   227,   159,    21,   261,  -135,
    -135,  -135,     3,  -135,   188,   248,  -135,    21,   275,   155,
     266,  -135,  -135,  -135,  -135,  -135,     7,   230,   269,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,   270,   272,   273,   276,
     277,   278,   222,   222,   226,    15,   274,   229,   293,  -135,
     236,  -135,  -135,  -135,  -135,  -135,  -135,   246,  -135,    56,
     149,    26,  -135,   159,   239,   247,   296,   242,  -135,   253,
    -135,   284,   244,   222,   222,   222,   222,   222,   222,  -135,
    -135,  -135,   249,   250,   300,    21,   291,   261,  -135,   188,
    -135,  -135,  -135,   282,  -135,   248,   307,   308,  -135,  -135,
     295,  -135,   230,   297,   298,  -135,  -135,  -135,  -135,  -135,
    -135,   255,   256,  -135,   274,  -135,  -135,   215,   257,  -135,
    -135,  -135,  -135,   284,   310,   316,    -3,  -135,    55,  -135,
    -135,   294,  -135,  -135,  -135,  -135,  -135,   259,   262,  -135,
     304,   263,   264,  -135,   309,   267,   100,   153,  -135,   127,
     179,  -135,  -135,   268,  -135,   304,  -135,   304,   271,  -135,
     309,  -135,   309,   153,  -135,  -135,   179,  -135,  -135,  -135,
     304,  -135,   309,  -135,  -135
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     3,    21,
      20,    15,    16,    17,    18,     9,    10,    11,    12,    13,
      14,     8,     5,     7,     6,     4,    19,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    74,    64,     0,     0,
       0,    24,     0,     0,     0,    25,    26,    27,    23,    22,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    65,     0,    30,    29,     0,   109,     0,     0,
       0,     0,     0,    28,     0,    44,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    74,    74,    74,    84,
       0,     0,     0,     0,     0,    50,    38,     0,     0,     0,
       0,    74,    74,    74,    74,    74,    74,     0,     0,     0,
       0,     0,    75,    66,    67,     0,   109,     0,    53,    60,
      57,    58,     0,    59,     0,   111,    61,     0,     0,     0,
       0,    45,    46,    47,    48,    49,    41,     0,     0,    36,
      69,    68,    73,    70,    71,    72,     0,     0,     0,     0,
       0,     0,    74,    74,    84,    86,    55,     0,     0,   129,
       0,   120,   121,   122,   123,   124,   125,   126,   128,     0,
       0,     0,   110,   109,     0,    38,     0,     0,    43,     0,
      34,    32,     0,    74,    74,    74,    74,    74,    74,    76,
      77,    85,     0,     0,     0,     0,     0,    53,    51,     0,
     127,   115,   113,   116,   114,   111,     0,     0,    39,    37,
       0,    42,     0,     0,     0,    79,    78,    83,    80,    81,
      82,    89,    99,    63,    55,    52,    54,     0,     0,   112,
      62,   130,    40,    32,     0,     0,    94,    87,   104,    88,
      56,     0,   117,   118,    33,    31,    35,     0,     0,    90,
      94,     0,     0,   100,   104,     0,    94,    94,    91,   104,
     104,   101,   119,     0,    95,    94,    92,    94,     0,   105,
     104,   102,   104,    94,    96,    93,   104,   106,   103,    97,
      94,   107,   104,    98,   108
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,  -135,
    -135,  -135,  -135,    91,   104,  -135,  -135,  -135,   145,   192,
    -111,  -135,  -135,  -135,   166,   128,   103,  -126,  -135,  -135,
    -135,  -135,   -94,   172,  -135,  -135,    -6,  -135,  -108,  -121,
     123,   160,  -134,  -135
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,   223,   191,    29,    30,    31,   140,   106,
      88,   146,   107,    32,   128,   168,   206,   134,    33,    34,
      35,    48,    72,   126,   204,   247,   259,   249,   263,   102,
     182,   135,   179,    36
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     180,   166,   122,   123,   124,   165,   158,   169,    37,    97,
      38,   183,    98,   169,    49,   257,    70,   150,   151,   152,
     153,   154,   155,   187,   258,    71,   141,   142,   143,    50,
     170,    92,    93,    94,    95,   171,   172,   173,   174,   175,
     176,   171,   172,   173,   174,   175,   176,    96,   144,   162,
     177,    39,   163,   212,   214,   178,   177,   188,   145,   189,
      51,   178,   216,    52,    40,   169,    41,    53,   199,   200,
     202,   203,   129,   261,    54,   237,   220,   129,    55,   234,
     130,   131,   262,    70,   133,   130,   131,   132,    56,   133,
      57,    58,   121,   171,   172,   173,   174,   175,   176,   225,
     226,   227,   228,   229,   230,    59,    60,   129,   177,    61,
     169,   252,   260,   178,   264,   130,   131,   211,   257,   133,
      85,    85,    86,   156,    62,    87,   157,   273,    42,    43,
      44,    45,   275,   277,    63,   280,   282,   169,   171,   172,
     173,   174,   175,   176,    46,   261,    66,    47,    67,   290,
      64,    65,   292,   177,   278,    68,   271,    69,   178,    73,
      74,   279,   281,   169,    75,   171,   172,   173,   174,   175,
     176,   257,   287,    76,   288,    77,    78,    79,   291,    80,
     177,    81,    82,    83,   294,   178,    84,   100,   104,   169,
     101,   171,   172,   173,   174,   175,   176,   261,   169,    89,
     129,   109,   111,    90,   112,    91,   177,    99,   130,   131,
     213,   178,   133,   113,   114,   103,   105,   171,   172,   173,
     174,   175,   176,   108,   115,   110,   171,   172,   173,   174,
     175,   176,   177,   116,   117,   118,   119,   178,     2,   120,
      70,   177,     3,     4,   125,   127,   178,     5,     6,     7,
       8,     9,    10,    11,   268,   136,   138,    12,    13,    14,
     274,   276,   137,    15,    16,   139,   129,   147,   149,   284,
     148,   285,   159,    17,   130,   131,   251,   289,   133,   167,
     181,   184,   160,   186,   293,   192,   161,   193,   164,   194,
     195,   190,   205,   196,   197,   198,   208,   209,   210,   219,
     217,    85,   222,   233,   221,   224,   231,   232,   235,   238,
     240,   241,   242,   255,   244,   245,   246,   248,   253,   256,
     266,   265,   257,   267,   269,   270,   243,   261,   272,   283,
     218,   185,   286,   207,   254,   236,   201,   250,   239,     0,
       0,   215
};

static const yytype_int16 yycheck[] =
{
     134,   127,    96,    97,    98,   126,   117,    10,     6,    61,
       8,   137,    64,    10,    61,    18,    18,   111,   112,   113,
     114,   115,   116,    16,    27,    27,    22,    23,    24,     7,
      27,    45,    46,    47,    48,    38,    39,    40,    41,    42,
      43,    38,    39,    40,    41,    42,    43,    61,    44,    61,
      53,    49,    64,   179,   180,    58,    53,    50,    54,    52,
       3,    58,   183,    28,     6,    10,     8,    30,   162,   163,
      55,    56,    51,    18,    61,   209,   187,    51,     3,   205,
      59,    60,    27,    18,    63,    59,    60,    61,     3,    63,
       3,     3,    27,    38,    39,    40,    41,    42,    43,   193,
     194,   195,   196,   197,   198,     3,    36,    51,    53,    61,
      10,   237,   246,    58,   248,    59,    60,    61,    18,    63,
      59,    59,    61,    61,    61,    64,    64,    27,    45,    46,
      47,    48,   266,   267,     8,   269,   270,    10,    38,    39,
      40,    41,    42,    43,    61,    18,    16,    64,    16,   283,
      61,    61,   286,    53,    27,    16,   264,    16,    58,    30,
       3,   269,   270,    10,     3,    38,    39,    40,    41,    42,
      43,    18,   280,    61,   282,    61,    33,    37,   286,    16,
      53,    34,    61,     3,   292,    58,    34,    29,    63,    10,
      31,    38,    39,    40,    41,    42,    43,    18,    10,    61,
      51,    34,    17,    61,    17,    61,    53,    61,    59,    60,
      61,    58,    63,    17,    17,    61,    61,    38,    39,    40,
      41,    42,    43,    61,    17,    61,    38,    39,    40,    41,
      42,    43,    53,    17,    16,    16,    16,    58,     0,    16,
      18,    53,     4,     5,    18,    16,    58,     9,    10,    11,
      12,    13,    14,    15,   260,     3,    28,    19,    20,    21,
     266,   267,    38,    25,    26,    18,    51,    16,     3,   275,
      61,   277,    61,    35,    59,    60,    61,   283,    63,    18,
      32,     6,    61,    17,   290,    16,    61,    17,    61,    17,
      17,    61,    18,    17,    17,    17,     3,    61,    52,     3,
      61,    59,    18,     3,    51,    61,    57,    57,    17,    27,
       3,     3,    17,     3,    17,    17,    61,    61,    61,     3,
      61,    27,    18,    61,    61,    61,   222,    18,    61,    61,
     185,   139,    61,   167,   243,   207,   164,   234,   215,    -1,
      -1,   181
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    67,     0,     4,     5,     9,    10,    11,    12,    13,
      14,    15,    19,    20,    21,    25,    26,    35,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    81,
      82,    83,    89,    94,    95,    96,   109,     6,     8,    49,
       6,     8,    45,    46,    47,    48,    61,    64,    97,    61,
       7,     3,    28,    30,    61,     3,     3,     3,     3,     3,
      36,    61,    61,     8,    61,    61,    16,    16,    16,    16,
      18,    27,    98,    30,     3,     3,    61,    61,    33,    37,
      16,    34,    61,     3,    34,    59,    61,    64,    86,    61,
      61,    61,    45,    46,    47,    48,    61,    61,    64,    61,
      29,    31,   105,    61,    63,    61,    85,    88,    61,    34,
      61,    17,    17,    17,    17,    17,    17,    16,    16,    16,
      16,    27,    98,    98,    98,    18,    99,    16,    90,    51,
      59,    60,    61,    63,    93,   107,     3,    38,    28,    18,
      84,    22,    23,    24,    44,    54,    87,    16,    61,     3,
      98,    98,    98,    98,    98,    98,    61,    64,    86,    61,
      61,    61,    61,    64,    61,   105,    93,    18,    91,    10,
      27,    38,    39,    40,    41,    42,    43,    53,    58,   108,
     108,    32,   106,    93,     6,    85,    17,    16,    50,    52,
      61,    80,    16,    17,    17,    17,    17,    17,    17,    98,
      98,    99,    55,    56,   100,    18,    92,    90,     3,    61,
      52,    61,    93,    61,    93,   107,   105,    61,    84,     3,
      86,    51,    18,    79,    61,    98,    98,    98,    98,    98,
      98,    57,    57,     3,    93,    17,    91,   108,    27,   106,
       3,     3,    17,    80,    17,    17,    61,   101,    61,   103,
      92,    61,    93,    61,    79,     3,     3,    18,    27,   102,
     108,    18,    27,   104,   108,    27,    61,    61,   102,    61,
      61,   104,    61,    27,   102,   108,   102,   108,    27,   104,
     108,   104,   108,    61,   102,   102,    61,   104,   104,   102,
     108,   104,   108,   102,   104
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    66,    67,    67,    68,    68,    68,    68,    68,    68,
      68,    68,    68,    68,    68,    68,    68,    68,    68,    68,
      68,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    79,    80,    81,    82,    83,    84,    84,
      85,    85,    85,    85,    86,    87,    87,    87,    87,    87,
      88,    89,    90,    91,    91,    92,    92,    93,    93,    93,
      93,    94,    95,    96,    97,    97,    97,    97,    97,    97,
      97,    97,    97,    97,    98,    98,    98,    98,    98,    98,
      98,    98,    98,    98,    99,    99,   100,   100,   100,   101,
     101,   101,   101,   101,   102,   102,   102,   102,   102,   103,
     103,   103,   103,   103,   104,   104,   104,   104,   104,   105,
     105,   106,   106,   107,   107,   107,   107,   107,   107,   107,
     108,   108,   108,   108,   108,   108,   108,   108,   108,   108,
     109
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     2,     2,     2,     2,     4,     3,
       3,    10,     0,     3,     1,    10,     6,     8,     0,     3,
       5,     2,     4,     3,     1,     1,     1,     1,     1,     1,
       1,     7,     4,     0,     3,     0,     3,     1,     1,     1,
       1,     5,     8,     8,     1,     2,     4,     4,     5,     5,
       5,     5,     5,     5,     0,     3,     5,     5,     6,     6,
       6,     6,     6,     6,     0,     3,     0,     3,     3,     0,
       2,     3,     4,     5,     0,     3,     4,     5,     6,     0,
       2,     3,     4,     5,     0,     3,     4,     5,     6,     0,
       3,     0,     3,     3,     3,     3,     3,     5,     5,     7,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     1,
       8
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (scanner, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, void *scanner)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (scanner);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, void *scanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep, scanner);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule, void *scanner)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)], scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, void *scanner)
{
  YYUSE (yyvaluep);
  YYUSE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (void *scanner)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 22: /* exit: EXIT SEMICOLON  */
#line 174 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    }
#line 1445 "yacc_sql.tab.c"
    break;

  case 23: /* help: HELP SEMICOLON  */
#line 179 "yacc_sql.y"
                   {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    }
#line 1453 "yacc_sql.tab.c"
    break;

  case 24: /* sync: SYNC SEMICOLON  */
#line 184 "yacc_sql.y"
                   {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
#line 1461 "yacc_sql.tab.c"
    break;

  case 25: /* begin: TRX_BEGIN SEMICOLON  */
#line 190 "yacc_sql.y"
                        {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
#line 1469 "yacc_sql.tab.c"
    break;

  case 26: /* commit: TRX_COMMIT SEMICOLON  */
#line 196 "yacc_sql.y"
                         {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
#line 1477 "yacc_sql.tab.c"
    break;

  case 27: /* rollback: TRX_ROLLBACK SEMICOLON  */
#line 202 "yacc_sql.y"
                           {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
#line 1485 "yacc_sql.tab.c"
    break;

  case 28: /* drop_table: DROP TABLE ID SEMICOLON  */
#line 208 "yacc_sql.y"
                            {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, (yyvsp[-1].string));
    }
#line 1494 "yacc_sql.tab.c"
    break;

  case 29: /* show_tables: SHOW TABLES SEMICOLON  */
#line 214 "yacc_sql.y"
                          {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
#line 1502 "yacc_sql.tab.c"
    break;

  case 30: /* desc_table: DESC ID SEMICOLON  */
#line 220 "yacc_sql.y"
                      {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, (yyvsp[-1].string));
    }
#line 1511 "yacc_sql.tab.c"
    break;

  case 31: /* create_index: CREATE INDEX ID ON ID LBRACE multi multi_list RBRACE SEMICOLON  */
#line 229 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			//create_index_init(&CONTEXT->ssql->sstr.create_index, $3, $5, $7);
			create_index_init_multi(&CONTEXT->ssql->sstr.create_index, (yyvsp[-7].string), (yyvsp[-5].string));
		}
#line 1521 "yacc_sql.tab.c"
    break;

  case 33: /* multi_list: COMMA multi multi_list  */
#line 237 "yacc_sql.y"
                              { 
	  }
#line 1528 "yacc_sql.tab.c"
    break;

  case 34: /* multi: ID  */
#line 242 "yacc_sql.y"
        {	
		create_index_append_attribute(&CONTEXT->ssql->sstr.create_index,(yyvsp[0].string));
	 }
#line 1536 "yacc_sql.tab.c"
    break;

  case 35: /* create_unique_index: CREATE UNIQUE INDEX ID ON ID LBRACE ID RBRACE SEMICOLON  */
#line 248 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_CREATE_UNIQUE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, (yyvsp[-6].string), (yyvsp[-4].string), (yyvsp[-2].string));
		}
#line 1545 "yacc_sql.tab.c"
    break;

  case 36: /* drop_index: DROP INDEX ID ON ID SEMICOLON  */
#line 256 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, (yyvsp[-3].string), (yyvsp[-1].string));
		}
#line 1554 "yacc_sql.tab.c"
    break;

  case 37: /* create_table: CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE SEMICOLON  */
#line 263 "yacc_sql.y"
                {
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			// CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length;
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, (yyvsp[-5].string));
			//临时变量清零	
			CONTEXT->value_length = 0;
		}
#line 1566 "yacc_sql.tab.c"
    break;

  case 39: /* attr_def_list: COMMA attr_def attr_def_list  */
#line 273 "yacc_sql.y"
                                   {    }
#line 1572 "yacc_sql.tab.c"
    break;

  case 40: /* attr_def: ID_get type LBRACE number RBRACE  */
#line 278 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, (yyvsp[-3].number), (yyvsp[-1].number));
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name =(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type = $2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length = $4;
			CONTEXT->value_length++;
		}
#line 1587 "yacc_sql.tab.c"
    break;

  case 41: /* attr_def: ID_get type  */
#line 289 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init_nullable(&attribute, CONTEXT->id, (yyvsp[0].number), 4,0);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name=(char*)malloc(sizeof(char));
			// stcrpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type=$2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length=10; // default attribute length
			CONTEXT->value_length++;
		}
#line 1602 "yacc_sql.tab.c"
    break;

  case 42: /* attr_def: ID_get type NOT NULLVALUE  */
#line 300 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init_nullable(&attribute, CONTEXT->id, (yyvsp[-2].number), 4,0);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name=(char*)malloc(sizeof(char));
			// stcrpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type=$2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length=10; // default attribute length
			CONTEXT->value_length++;
		}
#line 1617 "yacc_sql.tab.c"
    break;

  case 43: /* attr_def: ID_get type NULLABLE  */
#line 311 "yacc_sql.y"
                {
			AttrInfo attribute;
			attr_info_init_nullable(&attribute, CONTEXT->id, (yyvsp[-1].number), 4,1);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name=(char*)malloc(sizeof(char));
			// stcrpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type=$2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length=10; // default attribute length
			CONTEXT->value_length++;
		}
#line 1632 "yacc_sql.tab.c"
    break;

  case 44: /* number: NUMBER  */
#line 323 "yacc_sql.y"
                       {(yyval.number) = (yyvsp[0].number);}
#line 1638 "yacc_sql.tab.c"
    break;

  case 45: /* type: INT_T  */
#line 326 "yacc_sql.y"
              { (yyval.number)=INTS; }
#line 1644 "yacc_sql.tab.c"
    break;

  case 46: /* type: STRING_T  */
#line 327 "yacc_sql.y"
                  { (yyval.number)=CHARS; }
#line 1650 "yacc_sql.tab.c"
    break;

  case 47: /* type: FLOAT_T  */
#line 328 "yacc_sql.y"
                 { (yyval.number)=FLOATS; }
#line 1656 "yacc_sql.tab.c"
    break;

  case 48: /* type: DATE_T  */
#line 329 "yacc_sql.y"
                    { (yyval.number)=DATES; }
#line 1662 "yacc_sql.tab.c"
    break;

  case 49: /* type: TEXT_T  */
#line 330 "yacc_sql.y"
                    { (yyval.number)=TEXTS; }
#line 1668 "yacc_sql.tab.c"
    break;

  case 50: /* ID_get: ID  */
#line 334 "yacc_sql.y"
        {
		char *temp=(yyvsp[0].string); 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
#line 1677 "yacc_sql.tab.c"
    break;

  case 51: /* insert: INSERT INTO ID VALUES tuple tuple_list SEMICOLON  */
#line 344 "yacc_sql.y"
        {
			// CONTEXT->values[CONTEXT->value_length++] = *$6;

			CONTEXT->ssql->flag=SCF_INSERT;//"insert";
			// CONTEXT->ssql->sstr.insertion.relation_name = $3;
			// CONTEXT->ssql->sstr.insertion.value_num = CONTEXT->value_length;
			// for(i = 0; i < CONTEXT->value_length; i++){
			// 	CONTEXT->ssql->sstr.insertion.values[i] = CONTEXT->values[i];
      // }
			//inserts_init(&CONTEXT->ssql->sstr.insertion, $3, CONTEXT->values, CONTEXT->value_length);
			inserts_init_table_name(&CONTEXT->ssql->sstr.insertion, (yyvsp[-4].string));

      //临时变量清零
      CONTEXT->value_length=0;
    }
#line 1697 "yacc_sql.tab.c"
    break;

  case 52: /* tuple: LBRACE value value_list RBRACE  */
#line 361 "yacc_sql.y"
        {
       //inserts_init(&CONTEXT->ssql->sstr.insertion, $3, CONTEXT->values, CONTEXT->value_length);

	   	inserts_init_appends_rows_values(&CONTEXT->ssql->sstr.insertion,CONTEXT->values,CONTEXT->value_length);

		//插入一行记录，长度增加一行
		inserts_init_appends_rows_length(&CONTEXT->ssql->sstr.insertion);
		CONTEXT->value_length = 0;


	}
#line 1713 "yacc_sql.tab.c"
    break;

  case 54: /* tuple_list: COMMA tuple tuple_list  */
#line 374 "yacc_sql.y"
                              { 
	  }
#line 1720 "yacc_sql.tab.c"
    break;

  case 56: /* value_list: COMMA value value_list  */
#line 379 "yacc_sql.y"
                              { 
  		// CONTEXT->values[CONTEXT->value_length++] = *$2;
	  }
#line 1728 "yacc_sql.tab.c"
    break;

  case 57: /* value: NUMBER  */
#line 384 "yacc_sql.y"
          {	
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].number));
		}
#line 1736 "yacc_sql.tab.c"
    break;

  case 58: /* value: FLOAT  */
#line 387 "yacc_sql.y"
          {
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].floats));
		}
#line 1744 "yacc_sql.tab.c"
    break;

  case 59: /* value: SSS  */
#line 390 "yacc_sql.y"
         {
			(yyvsp[0].string) = substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
  		value_init_string(&CONTEXT->values[CONTEXT->value_length++], (yyvsp[0].string));
		}
#line 1753 "yacc_sql.tab.c"
    break;

  case 60: /* value: NULLVALUE  */
#line 395 "yacc_sql.y"
        {
  		value_init_null_value(&CONTEXT->values[CONTEXT->value_length++]);
	}
#line 1761 "yacc_sql.tab.c"
    break;

  case 61: /* delete: DELETE FROM ID where SEMICOLON  */
#line 402 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, (yyvsp[-2].string));
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;	
    }
#line 1773 "yacc_sql.tab.c"
    break;

  case 62: /* update: UPDATE ID SET ID EQ value where SEMICOLON  */
#line 412 "yacc_sql.y"
                {
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			Value *value = &CONTEXT->values[0];
			updates_init(&CONTEXT->ssql->sstr.update, (yyvsp[-6].string), (yyvsp[-4].string), value, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
#line 1785 "yacc_sql.tab.c"
    break;

  case 63: /* select: SELECT select_attr FROM ID rel_list where group_list SEMICOLON  */
#line 422 "yacc_sql.y"
                {
			// CONTEXT->ssql->sstr.selection.relations[CONTEXT->from_length++]=$4;
			selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-4].string));

			selects_append_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->conditions, CONTEXT->condition_length);

			CONTEXT->ssql->flag=SCF_SELECT;//"select";
			// CONTEXT->ssql->sstr.selection.attr_num = CONTEXT->select_length;

			//临时变量清零
			CONTEXT->condition_length=0;
			CONTEXT->from_length=0;
			CONTEXT->select_length=0;
			CONTEXT->value_length = 0;
	}
#line 1805 "yacc_sql.tab.c"
    break;

  case 64: /* select_attr: STAR  */
#line 440 "yacc_sql.y"
         {  
			RelAttr attr;
			attr.funtype=FUN_NO;
			relation_attr_init(&attr, NULL, "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1816 "yacc_sql.tab.c"
    break;

  case 65: /* select_attr: ID attr_list  */
#line 446 "yacc_sql.y"
                   {
			RelAttr attr;
			attr.funtype=FUN_NO;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1827 "yacc_sql.tab.c"
    break;

  case 66: /* select_attr: ID DOT ID attr_list  */
#line 452 "yacc_sql.y"
                              {
			RelAttr attr;
			attr.funtype=FUN_NO;
			//单表才有聚合
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1839 "yacc_sql.tab.c"
    break;

  case 67: /* select_attr: ID DOT STAR attr_list  */
#line 459 "yacc_sql.y"
                                {
			RelAttr attr;
			attr.funtype=FUN_NO;
			//单表才有聚合
			relation_attr_init(&attr, (yyvsp[-3].string),  "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1851 "yacc_sql.tab.c"
    break;

  case 68: /* select_attr: COUNT LBRACE STAR RBRACE attr_list  */
#line 466 "yacc_sql.y"
                                             {
			RelAttr attr;
			attr.funtype=FUN_COUNT_ALL;
			relation_attr_init(&attr, NULL,  "*");
			
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1863 "yacc_sql.tab.c"
    break;

  case 69: /* select_attr: COUNT LBRACE ID RBRACE attr_list  */
#line 473 "yacc_sql.y"
                                           {
			RelAttr attr;
			attr.funtype=FUN_COUNT;
			relation_attr_init(&attr, NULL,(yyvsp[-2].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1874 "yacc_sql.tab.c"
    break;

  case 70: /* select_attr: MAX LBRACE ID RBRACE attr_list  */
#line 479 "yacc_sql.y"
                                         {
			RelAttr attr;
			attr.funtype=FUN_MAX;
			relation_attr_init(&attr, NULL,(yyvsp[-2].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1885 "yacc_sql.tab.c"
    break;

  case 71: /* select_attr: MIN LBRACE ID RBRACE attr_list  */
#line 485 "yacc_sql.y"
                                         {
			RelAttr attr;
			attr.funtype=FUN_MIN;
			relation_attr_init(&attr, NULL,(yyvsp[-2].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1896 "yacc_sql.tab.c"
    break;

  case 72: /* select_attr: AVG LBRACE ID RBRACE attr_list  */
#line 491 "yacc_sql.y"
                                         {
			RelAttr attr;
			attr.funtype=FUN_AVG;
			relation_attr_init(&attr, NULL,(yyvsp[-2].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1907 "yacc_sql.tab.c"
    break;

  case 73: /* select_attr: COUNT LBRACE number RBRACE attr_list  */
#line 497 "yacc_sql.y"
                                               {
			RelAttr attr;
			attr.funtype=FUN_COUNT_ALL;
			relation_attr_init_number(&attr, NULL,(yyvsp[-2].number));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
#line 1918 "yacc_sql.tab.c"
    break;

  case 75: /* attr_list: COMMA ID attr_list  */
#line 506 "yacc_sql.y"
                         {
			RelAttr attr;
			attr.funtype=FUN_NO;
			relation_attr_init(&attr, NULL, (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
#line 1931 "yacc_sql.tab.c"
    break;

  case 76: /* attr_list: COMMA ID DOT ID attr_list  */
#line 514 "yacc_sql.y"
                                {
			RelAttr attr;
			attr.funtype=FUN_NO;
			//单表才有聚合
			relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string));
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
#line 1945 "yacc_sql.tab.c"
    break;

  case 77: /* attr_list: COMMA ID DOT STAR attr_list  */
#line 523 "yacc_sql.y"
                                       {
			RelAttr attr;
			attr.funtype=FUN_NO;
			relation_attr_init(&attr, (yyvsp[-3].string), "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
#line 1958 "yacc_sql.tab.c"
    break;

  case 78: /* attr_list: COMMA COUNT LBRACE STAR RBRACE attr_list  */
#line 531 "yacc_sql.y"
                                                   {
			RelAttr attr;
			attr.funtype=FUN_COUNT_ALL;
			relation_attr_init(&attr, NULL,  "*");
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
#line 1971 "yacc_sql.tab.c"
    break;

  case 79: /* attr_list: COMMA COUNT LBRACE ID RBRACE attr_list  */
#line 539 "yacc_sql.y"
                                                 {
		RelAttr attr;
		attr.funtype=FUN_COUNT;
		relation_attr_init(&attr, NULL,  (yyvsp[-2].string));
		selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	// CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
	// CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
	}
#line 1984 "yacc_sql.tab.c"
    break;

  case 80: /* attr_list: COMMA MAX LBRACE ID RBRACE attr_list  */
#line 547 "yacc_sql.y"
                                               {
		RelAttr attr;
		attr.funtype=FUN_MAX;
		relation_attr_init(&attr, NULL,  (yyvsp[-2].string));
		selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	// CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
	// CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
	}
#line 1997 "yacc_sql.tab.c"
    break;

  case 81: /* attr_list: COMMA MIN LBRACE ID RBRACE attr_list  */
#line 555 "yacc_sql.y"
                                               {
		RelAttr attr;
		attr.funtype=FUN_MIN;
		relation_attr_init(&attr, NULL,  (yyvsp[-2].string));
		selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	// CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
	// CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
	}
#line 2010 "yacc_sql.tab.c"
    break;

  case 82: /* attr_list: COMMA AVG LBRACE ID RBRACE attr_list  */
#line 563 "yacc_sql.y"
                                               {
		RelAttr attr;
		attr.funtype=FUN_AVG;
		relation_attr_init(&attr, NULL,  (yyvsp[-2].string));
		selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
	// CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
	// CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
	}
#line 2023 "yacc_sql.tab.c"
    break;

  case 83: /* attr_list: COMMA COUNT LBRACE number RBRACE attr_list  */
#line 571 "yacc_sql.y"
                                                     {
			RelAttr attr;
			attr.funtype=FUN_COUNT_ALL;
			relation_attr_init_number(&attr, NULL, (yyvsp[-2].number)); 
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
#line 2036 "yacc_sql.tab.c"
    break;

  case 85: /* rel_list: COMMA ID rel_list  */
#line 583 "yacc_sql.y"
                        {	
				selects_append_relation(&CONTEXT->ssql->sstr.selection, (yyvsp[-1].string));
		  }
#line 2044 "yacc_sql.tab.c"
    break;

  case 87: /* group_list: ORDER BY order_by_attr  */
#line 590 "yacc_sql.y"
        {

	}
#line 2052 "yacc_sql.tab.c"
    break;

  case 88: /* group_list: GROUP BY group_by_attr  */
#line 594 "yacc_sql.y"
        {
	}
#line 2059 "yacc_sql.tab.c"
    break;

  case 90: /* order_by_attr: ID order_by_attr_list  */
#line 600 "yacc_sql.y"
  {
		RelAttr attr;
		attr.funtype=FUN_ORDER_BY;
		attr.is_asc =ORDER_ASC;
		//默认升序(asc)
		relation_attr_init(&attr, NULL, (yyvsp[-1].string)); 
		selects_append_attribute_order_by(&CONTEXT->ssql->sstr.selection, &attr);
  }
#line 2072 "yacc_sql.tab.c"
    break;

  case 91: /* order_by_attr: ID comOp order_by_attr_list  */
#line 609 "yacc_sql.y"
        {
		RelAttr attr;
		attr.funtype=FUN_ORDER_BY;
		attr.is_asc =CONTEXT->comp;
		//默认升序(asc)
		relation_attr_init(&attr, NULL, (yyvsp[-2].string)); 
		selects_append_attribute_order_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2085 "yacc_sql.tab.c"
    break;

  case 92: /* order_by_attr: ID DOT ID order_by_attr_list  */
#line 618 "yacc_sql.y"
        {
		RelAttr attr;
		attr.funtype=FUN_ORDER_BY;
		attr.is_asc =ORDER_ASC;
		relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string)); 
		selects_append_attribute_order_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2097 "yacc_sql.tab.c"
    break;

  case 93: /* order_by_attr: ID DOT ID comOp order_by_attr_list  */
#line 626 "yacc_sql.y"
        {
		RelAttr attr;
		attr.funtype=FUN_ORDER_BY;
		attr.is_asc =CONTEXT->comp;
		relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string)); 
		selects_append_attribute_order_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2109 "yacc_sql.tab.c"
    break;

  case 95: /* order_by_attr_list: COMMA ID order_by_attr_list  */
#line 637 "yacc_sql.y"
        {
		RelAttr attr;
		attr.funtype=FUN_ORDER_BY;
		attr.is_asc =ORDER_ASC;
		//默认升序(asc)
		relation_attr_init(&attr, NULL, (yyvsp[-1].string)); 
		selects_append_attribute_order_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2122 "yacc_sql.tab.c"
    break;

  case 96: /* order_by_attr_list: COMMA ID comOp order_by_attr_list  */
#line 646 "yacc_sql.y"
        {
		RelAttr attr;
		attr.funtype=FUN_ORDER_BY;
		attr.is_asc =CONTEXT->comp;
		//默认升序(asc)
		relation_attr_init(&attr, NULL, (yyvsp[-2].string)); 
		selects_append_attribute_order_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2135 "yacc_sql.tab.c"
    break;

  case 97: /* order_by_attr_list: COMMA ID DOT ID order_by_attr_list  */
#line 655 "yacc_sql.y"
        {
		RelAttr attr;
		attr.funtype=FUN_ORDER_BY;
		attr.is_asc =ORDER_ASC;
		relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string)); 
		selects_append_attribute_order_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2147 "yacc_sql.tab.c"
    break;

  case 98: /* order_by_attr_list: COMMA ID DOT ID comOp order_by_attr_list  */
#line 663 "yacc_sql.y"
        {
		RelAttr attr;
		attr.funtype=FUN_ORDER_BY;
		attr.is_asc =CONTEXT->comp;
		relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string)); 
		selects_append_attribute_order_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2159 "yacc_sql.tab.c"
    break;

  case 100: /* group_by_attr: ID group_by_attr_list  */
#line 675 "yacc_sql.y"
  {
		RelAttr attr;
		attr.funtype=FUN_GROUP_BY;
		attr.is_asc =ORDER_ASC;
		//默认升序(asc)
		relation_attr_init(&attr, NULL, (yyvsp[-1].string)); 
		selects_append_attribute_group_by(&CONTEXT->ssql->sstr.selection, &attr);
  }
#line 2172 "yacc_sql.tab.c"
    break;

  case 101: /* group_by_attr: ID comOp group_by_attr_list  */
#line 684 "yacc_sql.y"
        {
		RelAttr attr;
		attr.funtype=FUN_GROUP_BY;
		attr.is_asc =CONTEXT->comp;
		//默认升序(asc)
		relation_attr_init(&attr, NULL, (yyvsp[-2].string)); 
		selects_append_attribute_group_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2185 "yacc_sql.tab.c"
    break;

  case 102: /* group_by_attr: ID DOT ID group_by_attr_list  */
#line 693 "yacc_sql.y"
        {
		RelAttr attr;
		attr.funtype=FUN_GROUP_BY;
		attr.is_asc =ORDER_ASC;
		relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string)); 
		selects_append_attribute_group_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2197 "yacc_sql.tab.c"
    break;

  case 103: /* group_by_attr: ID DOT ID comOp group_by_attr_list  */
#line 701 "yacc_sql.y"
        {
		RelAttr attr;
		attr.funtype=FUN_GROUP_BY;
		attr.is_asc =CONTEXT->comp;
		relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string)); 
		selects_append_attribute_group_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2209 "yacc_sql.tab.c"
    break;

  case 105: /* group_by_attr_list: COMMA ID group_by_attr_list  */
#line 712 "yacc_sql.y"
        {
		RelAttr attr;
		attr.funtype=FUN_GROUP_BY;
		attr.is_asc =ORDER_ASC;
		//默认升序(asc)
		relation_attr_init(&attr, NULL, (yyvsp[-1].string)); 
		selects_append_attribute_group_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2222 "yacc_sql.tab.c"
    break;

  case 106: /* group_by_attr_list: COMMA ID comOp group_by_attr_list  */
#line 721 "yacc_sql.y"
        {
		RelAttr attr;
		attr.funtype=FUN_GROUP_BY;
		attr.is_asc =CONTEXT->comp;
		//默认升序(asc)
		relation_attr_init(&attr, NULL, (yyvsp[-2].string)); 
		selects_append_attribute_group_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2235 "yacc_sql.tab.c"
    break;

  case 107: /* group_by_attr_list: COMMA ID DOT ID group_by_attr_list  */
#line 730 "yacc_sql.y"
        {
		RelAttr attr;
		attr.funtype=FUN_GROUP_BY;
		attr.is_asc =ORDER_ASC;
		relation_attr_init(&attr, (yyvsp[-3].string), (yyvsp[-1].string)); 
		selects_append_attribute_group_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2247 "yacc_sql.tab.c"
    break;

  case 108: /* group_by_attr_list: COMMA ID DOT ID comOp group_by_attr_list  */
#line 738 "yacc_sql.y"
        {
		RelAttr attr;
		attr.funtype=FUN_GROUP_BY;
		attr.is_asc =CONTEXT->comp;
		relation_attr_init(&attr, (yyvsp[-4].string), (yyvsp[-2].string)); 
		selects_append_attribute_group_by(&CONTEXT->ssql->sstr.selection, &attr);
	}
#line 2259 "yacc_sql.tab.c"
    break;

  case 110: /* where: WHERE condition condition_list  */
#line 748 "yacc_sql.y"
                                     {	
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2267 "yacc_sql.tab.c"
    break;

  case 112: /* condition_list: AND condition condition_list  */
#line 754 "yacc_sql.y"
                                   {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
#line 2275 "yacc_sql.tab.c"
    break;

  case 113: /* condition: ID comOp value  */
#line 760 "yacc_sql.y"
                {
			RelAttr left_attr;
			left_attr.funtype=FUN_NO;
			relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));

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
#line 2301 "yacc_sql.tab.c"
    break;

  case 114: /* condition: value comOp value  */
#line 782 "yacc_sql.y"
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
#line 2325 "yacc_sql.tab.c"
    break;

  case 115: /* condition: ID comOp ID  */
#line 802 "yacc_sql.y"
                {
			RelAttr left_attr;
			left_attr.funtype=FUN_NO;
			relation_attr_init(&left_attr, NULL, (yyvsp[-2].string));
			RelAttr right_attr;
			right_attr.funtype=FUN_NO;
			relation_attr_init(&right_attr, NULL, (yyvsp[0].string));

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
#line 2351 "yacc_sql.tab.c"
    break;

  case 116: /* condition: value comOp ID  */
#line 824 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];
			RelAttr right_attr;
			right_attr.funtype=FUN_NO;
			relation_attr_init(&right_attr, NULL, (yyvsp[0].string));

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
#line 2378 "yacc_sql.tab.c"
    break;

  case 117: /* condition: ID DOT ID comOp value  */
#line 847 "yacc_sql.y"
                {
			RelAttr left_attr;
			left_attr.funtype=FUN_NO;
			relation_attr_init(&left_attr, (yyvsp[-4].string), (yyvsp[-2].string));
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
#line 2404 "yacc_sql.tab.c"
    break;

  case 118: /* condition: value comOp ID DOT ID  */
#line 869 "yacc_sql.y"
                {
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];

			RelAttr right_attr;
			right_attr.funtype=FUN_NO;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string));

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
#line 2430 "yacc_sql.tab.c"
    break;

  case 119: /* condition: ID DOT ID comOp ID DOT ID  */
#line 891 "yacc_sql.y"
                {
			RelAttr left_attr;
			left_attr.funtype=FUN_NO;
			relation_attr_init(&left_attr, (yyvsp[-6].string), (yyvsp[-4].string));
			RelAttr right_attr;
			right_attr.funtype=FUN_NO;
			relation_attr_init(&right_attr, (yyvsp[-2].string), (yyvsp[0].string));

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
#line 2455 "yacc_sql.tab.c"
    break;

  case 120: /* comOp: EQ  */
#line 914 "yacc_sql.y"
             { CONTEXT->comp = EQUAL_TO; }
#line 2461 "yacc_sql.tab.c"
    break;

  case 121: /* comOp: LT  */
#line 915 "yacc_sql.y"
         { CONTEXT->comp = LESS_THAN; }
#line 2467 "yacc_sql.tab.c"
    break;

  case 122: /* comOp: GT  */
#line 916 "yacc_sql.y"
         { CONTEXT->comp = GREAT_THAN; }
#line 2473 "yacc_sql.tab.c"
    break;

  case 123: /* comOp: LE  */
#line 917 "yacc_sql.y"
         { CONTEXT->comp = LESS_EQUAL; }
#line 2479 "yacc_sql.tab.c"
    break;

  case 124: /* comOp: GE  */
#line 918 "yacc_sql.y"
         { CONTEXT->comp = GREAT_EQUAL; }
#line 2485 "yacc_sql.tab.c"
    break;

  case 125: /* comOp: NE  */
#line 919 "yacc_sql.y"
         { CONTEXT->comp = NOT_EQUAL; }
#line 2491 "yacc_sql.tab.c"
    break;

  case 126: /* comOp: IS  */
#line 920 "yacc_sql.y"
             { CONTEXT->comp = IS_NULL; }
#line 2497 "yacc_sql.tab.c"
    break;

  case 127: /* comOp: IS NOT  */
#line 921 "yacc_sql.y"
                 { CONTEXT->comp = IS_NOT_NULL; }
#line 2503 "yacc_sql.tab.c"
    break;

  case 128: /* comOp: ASC  */
#line 922 "yacc_sql.y"
              { CONTEXT->comp = ORDER_ASC; }
#line 2509 "yacc_sql.tab.c"
    break;

  case 129: /* comOp: DESC  */
#line 923 "yacc_sql.y"
               { CONTEXT->comp = ORDER_DESC; }
#line 2515 "yacc_sql.tab.c"
    break;

  case 130: /* load_data: LOAD DATA INFILE SSS INTO TABLE ID SEMICOLON  */
#line 928 "yacc_sql.y"
                {
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, (yyvsp[-1].string), (yyvsp[-4].string));
		}
#line 2524 "yacc_sql.tab.c"
    break;


#line 2528 "yacc_sql.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (scanner, YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, scanner);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (scanner, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 933 "yacc_sql.y"

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
