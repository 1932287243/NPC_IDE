#include "sdb/expr.h"
#include "reg.h"

EXPR::EXPR()
{

}
EXPR::~EXPR()
{
    std::cout << "delete expr" << std::endl;
}

uint32_t EXPR::evaluate(const std::string &expression, bool &success) 
{
    parenthesesNum = 0;
    tokens.clear();

    if (!tokenize(expression)) {
        success = false;
        return 0;
    }

    recursionCheck(0, tokens.size() - 1);

    return eval(0, tokens.size() - 1);
    return 0;
}

void EXPR::initExpr()
{
    initRules();
    initRegex();
}

void EXPR::initRules() 
{
    rules = {
        {"0x[0-9a-f]+", TK_HEX},               // Hex
        {"[0-9][0-9]*", TK_INTEGER},           // Integer
        {"\\$[A-Za-z0-9]{1,3}", TK_REG},       // Register
        {" +", TK_NOTYPE},                     // Spaces
        {"\\+", TK_ADD},                       // Plus
        {"-{1}", TK_SUB},                      // Subtraction
        {"\\*", TK_MULT},                      // Multiplication
        {"/", TK_DIV},                         // Division
        {"\\(", TK_L_BRACKET},                 // Left Bracket
        {"\\)", TK_R_BRACKET},                 // Right Bracket
        {"==", TK_EQ},                         // Equal
        {"!=", TK_NEQ},                        // Not Equal
        {"&&", TK_AND},                        // And
        {"\\|\\|", TK_OR}                      // Or
    };
}

// 初始化正则表达式， 用于匹配输入表达式中的不同类型的符号和数值。
void EXPR::initRegex() 
{
    for (const auto &rule : rules) {
        regexPatterns.emplace_back(std::regex(rule.regex, std::regex_constants::extended));
    }
}

// 词法分析：匹配表达式中的数字和符号
bool EXPR::tokenize(const std::string &expression) 
{
    size_t position = 0;
    while (position < expression.length()) {
        bool matched = false;
        for (size_t i = 0; i < regexPatterns.size(); i++) {
            std::smatch match;
            if (std::regex_search(expression.begin() + position, expression.end(), match, regexPatterns[i]) && match.position(0) == 0) {
                std::string tokenStr = match.str(0);
                position += tokenStr.length();
                if (rules[i].tokenType != TK_NOTYPE) {
                    Token token = {rules[i].tokenType, tokenStr};
                    if(handleToken(token))
                    tokens.push_back(token);
                }
                matched = true;
                break;
            }
        }
        if (!matched) {
            std::cerr << "No match at position " << position << ": " << expression.substr(position) << std::endl;
            return false;
        }
    }
    return true;
}

// 处理得到token
bool EXPR::handleToken(Token &token) {
    std::string token_val = token.str;
    token.str.clear();
    switch (token.type) {
        case TK_INTEGER:{
            token.type = TK_INTEGER;
            if(sub_flag){
                token.str = "-" + token_val;
                sub_flag = false;
            }
            else{
                token.str = token_val;
            }
            break;
        }
        case TK_HEX:{
            token.type = TK_HEX;
            token.str = token_val;
            break;
        }
        case TK_REG:{
            token.type = TK_REG;
            token.str = token_val;
            break;
        }
        // case TK_DEREF:{
        //   token.type = TK_DEREF;
        // token.str = token.str;
        //   break;
        // }
        case TK_ADD:{
            token.type = TK_ADD;
            break;
        }
        case TK_SUB:{
            token.type = TK_SUB;
            if (!tokens.empty() && (tokens.back().type == TK_INTEGER || tokens.back().type == TK_R_BRACKET)) {
                token.type = TK_SUB;
            } else {
                sub_flag = true;
                return false;
            }
            break;
        }
        case TK_MULT:{
            if (!tokens.empty() && (tokens.back().type == TK_INTEGER || tokens.back().type == TK_R_BRACKET)) {
                token.type = TK_MULT;
            } else {
                token.type = TK_DEREF;
            }
            break;
        }
        case TK_DIV:{
            token.type = TK_DIV;
            break;
        }
        case TK_L_BRACKET:{
            token.type = TK_L_BRACKET;
            break;
        }
        case TK_R_BRACKET:{
            token.type = TK_R_BRACKET;
            break;
        }
        case TK_EQ:{
            token.type = TK_EQ;
            break;
        }
        case TK_NEQ:{
            token.type = TK_NEQ;
            break;
        }
        case TK_AND:{
            token.type = TK_AND;
            break;
        }
        case TK_OR:{
            token.type = TK_OR;
            break;
        }
        case TK_NOTYPE:{
            token.type = TK_NOTYPE;
            break;
        }
        default: printf("regex no type!!!\n");
    }
    return true;
}
    
// 递归寻找表达式里面的括号
void EXPR::recursionCheck(int p, int q) 
{
    for (int i = p; i <= q; i++) {
        if (tokens[i].type == TK_L_BRACKET) {
            recursionCheck(i + 1, q);
            for (int j = i; j <= q; j++) {
                if (tokens[j].type == TK_R_BRACKET && tokens[j].str.empty()) {
                    parenthesesNum++;
                    tokens[i].str = std::to_string(parenthesesNum);
                    tokens[j].str = std::to_string(parenthesesNum);
                    return;
                }
            }
        }
    }
}

// 检查在tokens中p和q索引处的左右括号是否一对
bool EXPR::checkParentheses(int p, int q) {
  // test
  // for(int i=0; i<q; i++)
  // {
  //     if(tokens[i].type == TK_L_BRACKET || tokens[i].type == TK_R_BRACKET)
  //     {
  //         if(tokens[i].str[0] == '0')
  //         {
  //             std::cout << "parameter error" << std::endl;
  //         }
  //     }
  // }

    if (tokens[p].type == TK_L_BRACKET && tokens[q].type == TK_R_BRACKET) {
        return tokens[p].str == tokens[q].str;
    }
    return false;
}

// 在寻找主操作符号的时候找到优先级最低的操作符号
int EXPR::getPrecedence(int type) 
{
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

// 找到主操作符号
int EXPR::findMainOperator(int p, int q) 
{
    int minPrecedence = 500; 
    int mainOperatorIndex = -1;
    int parenthesesDepth = 0;
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

uint32_t EXPR::eval(int p, int q) 
{
    int val1=0, val2=0;
    if (p > q) 
    {
        /* Bad expression */
        // throw std::invalid_argument("Bad expression");
        std::cout << "\033[;41mBad expression\033[0m\n";
        return 0;
    }
    else if (p == q) 
    {
        /* Single token.
        * For now this token should be a number.
        * Return the value of the number.
        */
        if(tokens[p].type == TK_INTEGER)
            return std::stoi(tokens[p].str);//std::atoi(tokens[p].str);
        else if (tokens[p].type == TK_HEX)
            return std::stoul(tokens[p].str, nullptr, 16);//(uint32_t)strtol(tokens[p].str, NULL, 16);
        else if (tokens[p].type == TK_REG)
        {
            return riscv32_reg->getGPRValue(tokens[p].str.substr(1));
        }
        else
            std::cout << "Expression error" << std::endl;
        }
        else if (checkParentheses(p, q) == true)
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
                case TK_DEREF:  std::cout << "read memory failed" << std::endl;//printf("read memory failed\n");
                default: throw std::invalid_argument("Unknown operator");//assert(0);
            }
        }
    return 0;
}

