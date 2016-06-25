#include "hobby.h"
#include "MEM.h"
#include "DBG.h"

/*
  hbb_function_define 函数定义
  char *identifier 函数名
  ParameterList *parameter_list 参数列表
  Block *block 代码块
  return;
*/
void hbb_function_define(char *identifier,
  ParameterList *parameter_list
  , Block *block){
    FunctionDefinition *func;
    HBB_InterPreter *inter;

    if (hbb_search_function(identifier)) {
        hbb_compile_error(FUNCTION_MULTIPLE_DEFINE_ERROR,
                          STRING_MESSAGE_ARGUMENT,"name",identifier
                          MESSAGE_ARGUMENT_END);
        return;
    }

    inter = hbb_get_current_interpreter();
    func = hbb_malloc(sizeof(FunctionDefinition));
    func->name = identifier;
    func->type = HOBBY_FUNCTION_DEFINITION;
    func->u.hobby_f.parameter = parameter_list;
    func->u.hobby_f.block = block;
    /* function list */
    func->next = inter.function_list;
    inter->function_list = func;
}

/*
  hbb_create_parameter 创建参数
  char *identifier 参数名
  return ParameterList* 参数列表
*/
ParameterList *hbb_create_parameter(char *identifier){
    ParameterList *parameterList;
    parameterList = hbb_malloc(sizeof(ParameterList));
    parameterList->name = identifier;
    parameterList->next = NULL;
    return parameterList;
}

/*
  hbb_chain_parameter 创建一个参数列表的指针
  ParameterList *list 参数列表
  char *identifier 参数名
  return *list    参数列表
*/
ParameterList *hbb_chain_parameter(ParameterList *list, char *identifier){
    ParameterList *pos;
    /* 尾指针 */
    for(pos = list; pos->next; pos = pos->next);
    pos->next = hbb_create_parameter(identifier);

    return list;
}

/*
  hbb_create_argument_list 创建表达式列表
  Expression* expression 表达式
  return ＊arg 表达式列表
*/
ArgumentList *hbb_create_argument_list(Expression *expression){
    ArgumentList *arg;

    arg = hbb_malloc(sizeof(ArgumentList));
    arg->expression = expression;
    arg->next = NULL;
    return arg;
}

/*
  hbb_chain_argument_list 创建表达式列表
  ArgumentList *list 之前的列表
  Expression *exp 表达式
  return *list 返回的表达式列表
 */
ArgumentList *hbb_chain_argument_list(ArgumentList *list, Expression *exp){
    ArgumentList *pos;
    for(pos = list; pos->next; pos = pos->next);
    pos->next = hbb_create_argument_list(exp);
    return list;
}

/*
  这东西类似声明块
  hbb_create_statement_list 创建声明列表
  Statement *statement 声明
  return statement_list 声明列表
*/
StatementList *hbb_create_statement_list(Statement *statement){
    StatementList *statement_list;

    statement_list = hbb_malloc(sizeof(Statement));
    statement_list->statement = statement;
    statement_list->next = NULL;
    return statement_list;
}

/*
  hbb_chain_statement_list 创建声明列表
  StatementList *list 前驱声明列表
  Statement *st 声明
  return list 返回声明列表
*/
StatementList *hbb_chain_statement_list(StatementList *list, Statement *st){
    StatementList *pos;

    if (list == NULL) {
        return hbb_create_statement_list(statement);
    }

    for(pos = list; pos->next; pos = pos->next);
    pos->next = hbb_create_statement_list(statement);
    return list;
}

/*
  hbb_alloc_expression 表达式开空间
  ExpressionType type 表达式类型
  return exp 表达式
*/
Expression *hbb_alloc_expression(ExpressionType type){
    Expression *exp;

    exp = hbb_malloc(sizeof(Expression));
    exp->type = type;
    exp->line_number = hbb_get_current_interpreter()->current_line_number;

    return exp;
}

/*
  hbb_create_assign_expression 赋值语句开空间
  char *variable 符号
  Expression *operand 表达式
  return exp 返回赋值语句
*/
Expression *hbb_create_assign_expression(char *variable,Expression *operand){
    Expression *exp;

    exp = hbb_alloc_expression(ASSIGN_EXPRESSION);
    exp->u.assign_expression.variable = variable;
    exp->u.assign_expression.operand = operand;

    return exp;
}

/*
  静态方法 convert_value_to_expression 值转换为表达式
  HBB_Value *value 传入的值
  return expr; 返回的表达式
*/
static Expression convert_value_to_expression(HBB_Value *value){
    Expression expr;
    if (value->type == HBB_INT_VALUE) {
        expr.type = INT_EXPRESSION;
        expr.u.int_value = value->u.int_value;
    }else if(value->type == HBB_DOUBLE_VALUE){
        expr.type = DOUBLE_EXPRESSION;
        expr.u.double_value = value->u.double_value;
    }else {
        /* boolean */
        DBG_assert(value->type == HBB_BOOLEAN_VALUE,
                  ("value->type.. %d\n", value->type));
        expr->type = BOOLEAN_EXPRESSION;
        expr->u.boolean_value = value->u.boolean_value;
    }
    return expr;
}

/*
  hbb_create_binary_expression 常量折叠
  ExpressionType type 表达式类型
  Expression *left 左表达式
  Expression *right 右表达式
  return exp; 返回表达式
*/
Expression *hbb_create_binary_expression(ExpressionType type,
                                      Expression *left,Expression *right){
    /* 如果左／右全是数字 开始常量折叠 */
    if ((left->type == INT_EXPRESSION || left->type == DOUBLE_EXPRESSION)
        && (right->type == INT_EXPRESSION || right->type == DOUBLE_EXPRESSION)) {
        HBB_Value value;
        value = hbb_eval_binary_expression(hbb_get_current_interpreter(),
                                          NULL, type, left, right);
        *left = convert_value_to_expression(&value);

        return left;
    } else {
        Expression *exp;
        exp = hbb_alloc_expression(type);
        exp->u.binary_expression.left = left;
        exp->u.binary_expression.right = right;
        return exp;
    }
}
