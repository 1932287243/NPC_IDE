#include "sdb.h"
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_ADD, TK_SUB, TK_MULT, TK_DIV, TK_EQ, TK_NEQ, TK_AND, TK_OR, 
  TK_DEREF, TK_INTEGER, TK_HEX, TK_REG, TK_L_BRACKET, TK_R_BRACKET 
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {
  {"0x[0-9a-f]+", TK_HEX},                     // hex
  {"[0-9][0-9]*", TK_INTEGER},              // integer
  {"\\$[A-Za-z0-9]{1,3}", TK_REG},          // register
  {" +", TK_NOTYPE},                        // spaces
  {"\\+", TK_ADD},                          // plus
  {"-{1}", TK_SUB},                         // subtraction
  {"\\*", TK_MULT},                         // multiplication
  {"/", TK_DIV},                            // division
  {"\\(", TK_L_BRACKET},                    // left bracket
  {"\\)", TK_R_BRACKET},                    // right bracket
  {"==", TK_EQ},                            // equal
  {"!=", TK_NEQ},                           // not equal
  {"&&", TK_AND},                           // and
  {"\\|\\|", TK_OR}                         // or
};

#define NR_REGEX (int)(sizeof(rules) / sizeof(rules[0]))
static regex_t re[NR_REGEX] = {};
int parentheses_num = 0;        // The logarithm of parentheses
int sub_flag = 0;               // Negative Flag 
/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      printf("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[128] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //     i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_INTEGER:{
            tokens[nr_token].type = TK_INTEGER;
            if(sub_flag)
            {
              if(substr_len+1 < sizeof(tokens[nr_token].str))
              {
                memset(tokens[nr_token].str,'0', sizeof(tokens[nr_token].str));
                tokens[nr_token].str[0] = '-';
                strncpy(tokens[nr_token].str+1, substr_start, substr_len);
                tokens[nr_token].str[substr_len+1] = '\0';
              }
              else
                memset(tokens[nr_token].str,'0', sizeof(tokens[nr_token].str));
              sub_flag = 0;
            }
            else
            {
              if(substr_len < sizeof(tokens[nr_token].str))
              {
                memset(tokens[nr_token].str,'0', sizeof(tokens[nr_token].str));
                strncpy(tokens[nr_token].str, substr_start, substr_len);
                tokens[nr_token].str[substr_len] = '\0';
              }
              else
                memset(tokens[nr_token].str,'0', sizeof(tokens[nr_token].str));
            }
            nr_token++;
            break;
          }
          case TK_HEX:{
            tokens[nr_token].type = TK_HEX;
            if(substr_len < sizeof(tokens[nr_token].str))
            {
              memset(tokens[nr_token].str,'0', sizeof(tokens[nr_token].str));
              strncpy(tokens[nr_token].str, substr_start, substr_len);
              tokens[nr_token].str[substr_len] = '\0';
            }
            else
              memset(tokens[nr_token].str,'0', sizeof(tokens[nr_token].str));
            nr_token++;
            break;
          }
          case TK_REG:{
            tokens[nr_token].type = TK_REG;
            memset(tokens[nr_token].str,'0', sizeof(tokens[nr_token].str));
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            nr_token++;
            break;
          }
          // case TK_DEREF:{
          //   tokens[nr_token].type = TK_DEREF;
          //   memset(tokens[nr_token].str,'0', sizeof(tokens[nr_token].str));
          //   strncpy(tokens[nr_token].str, substr_start, substr_len);
          //   tokens[nr_token].str[substr_len] = '\0';
          //   nr_token++;
          //   break;
          // }
          case TK_ADD:{
            tokens[nr_token].type = TK_ADD;
            memset(tokens[nr_token].str,'0', sizeof(tokens[nr_token].str));
            nr_token++;
            break;
          }
          case TK_SUB:{
            tokens[nr_token].type = TK_SUB;
            if(tokens[nr_token-1].type == TK_R_BRACKET || tokens[nr_token-1].type == TK_INTEGER)
            {
              memset(tokens[nr_token].str,'0', sizeof(tokens[nr_token].str));
              nr_token++;
            }
            else
            {
              sub_flag = 1;
            }
            break;
          }
          case TK_MULT:{
            if((tokens[nr_token-1].type == TK_R_BRACKET || tokens[nr_token-1].type == TK_INTEGER) && nr_token != 0)
            {
              tokens[nr_token].type = TK_MULT;
              memset(tokens[nr_token].str,'0', sizeof(tokens[nr_token].str));
              nr_token++;
            }
            else
            {
              tokens[nr_token].type = TK_DEREF;
              memset(tokens[nr_token].str,'0', sizeof(tokens[nr_token].str));
              nr_token++;
            }
            break;
          }
          case TK_DIV:{
            tokens[nr_token].type = TK_DIV;
            memset(tokens[nr_token].str,'0', sizeof(tokens[nr_token].str));
            nr_token++;
            break;
          }
          case TK_L_BRACKET:{
            tokens[nr_token].type = TK_L_BRACKET;
            memset(tokens[nr_token].str,'0', sizeof(tokens[nr_token].str));
            nr_token++;
            break;
          }
          case TK_R_BRACKET:{
            tokens[nr_token].type = TK_R_BRACKET;
            memset(tokens[nr_token].str,'0', sizeof(tokens[nr_token].str));
            nr_token++;
            break;
          }
          case TK_EQ:{
            tokens[nr_token].type = TK_EQ;
            memset(tokens[nr_token].str,'0', sizeof(tokens[nr_token].str));
            nr_token++;
            break;
          }
          case TK_NEQ:{
            tokens[nr_token].type = TK_NEQ;
            memset(tokens[nr_token].str,'0', sizeof(tokens[nr_token].str));
            nr_token++;
            break;
          }
          case TK_AND:{
            tokens[nr_token].type = TK_AND;
            memset(tokens[nr_token].str,'0', sizeof(tokens[nr_token].str));
            nr_token++;
            break;
          }
          case TK_OR:{
            tokens[nr_token].type = TK_OR;
            memset(tokens[nr_token].str,'0', sizeof(tokens[nr_token].str));
            nr_token++;
            break;
          }
          case TK_NOTYPE:{
            tokens[nr_token].type = TK_NOTYPE;
            // nr_token++;
            break;
          }
          default: printf("regex no type!!!\n");
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

void recursion_check(int p, int q)
{
  int i;
  int j;
  for(i=p; i<=q; i++)
  {
    if(tokens[i].type == TK_L_BRACKET)
    {
      recursion_check(i+1, q);
      for(j=i; j<=q; j++)
      {
        if(tokens[j].type == TK_R_BRACKET && tokens[j].str[0] == '0')
        {
          parentheses_num++;
          tokens[i].str[0] = '0' + parentheses_num;
          tokens[j].str[0] = '0' + parentheses_num;
          return;
        }  
     }
    }
  }
}

bool check_parentheses(int p, int q)
{
  // recursion_check(p, q);
  for(int i=0; i<q; i++)
  {
    if(tokens[i].type == TK_L_BRACKET || tokens[i].type == TK_R_BRACKET)
    {
      if(tokens[i].str[0] == '0')
      {
        printf("parameter error");
      }
    }
  }
  if(tokens[p].type == TK_L_BRACKET && tokens[q].type == TK_R_BRACKET)
  {
    if(tokens[p].str[0] == tokens[q].str[0])
      return true;
    else
      return false;
  }
  else
  {
    return false;
  }
}

int getPrecedence(int type) {
    switch (type) {
        case TK_ADD:
        case TK_SUB:
            return 1;
        case TK_MULT:
        case TK_DIV:
            return 2;
        default:
            return 0;
    }
}

int findMainOperator(int p, int q) 
{
  int minPrecedence = 500; 
  int mainOperatorIndex = -1;
  int parenthesesDepth = 0;
  // (    ( 6 *   (   (( 93 )-9)   )   )    )-((42))
  for (int i = p; i < q; i++) 
  {
    if (tokens[i].type == TK_L_BRACKET) 
    {
        parenthesesDepth++;
    } 
    else if (tokens[i].type == TK_R_BRACKET) 
    {
        parenthesesDepth--;
    } 
    else if ((tokens[i].type>TK_NOTYPE && tokens[i].type<TK_INTEGER) && parenthesesDepth == 0) 
    {
      int precedence = getPrecedence(tokens[i].type);
      if (precedence < minPrecedence) 
      {
        minPrecedence = precedence;
        mainOperatorIndex = i;
      } 
      else if (precedence == minPrecedence) 
      {
        mainOperatorIndex = i;
      }
    }
  }
  return mainOperatorIndex;
}

uint32_t eval(int p, int q) {
  int val1=0, val2=0;
  if (p > q) 
  {
    /* Bad expression */
    printf("parameter error:p>q");
  }
  else if (p == q) 
  {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    if(tokens[p].type == TK_INTEGER)
      return atoi(tokens[p].str);
    else if (tokens[p].type == TK_HEX)
      return (uint32_t)strtol(tokens[p].str, NULL, 16);
    else if (tokens[p].type == TK_REG)
    {
      bool success;
      printf("print reg value failed\n");
      exit(-1);
      // return isa_reg_str2val(tokens[p].str+1, &success);
      // printf("reg=%s\tsuccess=%d\n", tokens[p].str+1, success);
    }
    else
      printf("Expression error");
  }
  else if (check_parentheses(p, q) == true)
  {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1);
  }
  else 
  {
    int op;
    op = findMainOperator(p, q);
    // printf("p=%d\tq=%d\top=%d\n", p, q, op);
    if(tokens[op].type!=TK_DEREF)
      val1 = eval(p, op - 1);
    val2 = eval(op + 1, q);

    switch (tokens[op].type) {
      case TK_ADD:    return val1 + val2;
      case TK_SUB:    return val1 - val2;
      case TK_MULT:   return val1 * val2;
      case TK_DIV:    return val1 / val2;
      case TK_EQ:     return val1 == val2;
      case TK_NEQ:    return val1 != val2;
      case TK_AND:    return val1 && val2;
      case TK_OR:     return val1 || val2;
      case TK_DEREF:  printf("read memory failed\n");
      default: assert(0);
    }
  }
  return 0;
}


uint32_t expr(char *e, bool *success) {
  parentheses_num = 0;
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  recursion_check(0, nr_token-1);
  /* TODO: Insert codes to evaluate the expression. */
  // TODO();
  // printf("line=%d\tnr_token=%d\n",__LINE__, nr_token);

  // int i;
  // for(i=0; i<nr_token; i++)
  // {
  //   // if(tokens[i].type == TK_L_BRACKET || tokens[i].type == TK_R_BRACKET)
  //     // printf("val=%c\n", tokens[i].str[0]);
  //     printf("type=%d\n", tokens[i].type);
  // }
   
  return eval(0, nr_token-1);
}
