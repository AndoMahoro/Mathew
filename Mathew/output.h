#ifndef OUTPUT
#error "You must define OUTPUT macro before include this file"
#endif

//outputs
/*--------------------------- tool outputs -------------------------------*/
OUTPUT(OUTPUT_HEAD = 0,                  "dummy")
OUTPUT(REPRINT_INPUT_FILE,               "/* 重新打印输入文件 \"%s\" 的信息\n*  符号:\n")
OUTPUT(NO_USED_RULE,                     "未使用的产生式:\n")
OUTPUT(USED_RULE_EPSILON_SYMBOLS,        "被使用的产生式中能推出空(epsilon)的符号:\n")
OUTPUT(SYMBOL_AT_RULE,                   "第 %d 个产生式的符号: %s \n")
OUTPUT(NT_SYMBOLS_FIRST_SET,             "非终结符的首符集:\n")
OUTPUT(USED_RULE,                        "被使用的产生式:\n")
OUTPUT(STATE_NUM,                        "分析表的状态数目: %d\n") 
OUTPUT(VERSION,                          "马修当前版本 version 1.0\n") 
/*--------------------------- action outputs -------------------------------*/
OUTPUT(CONFLICT_COUNT,                   "产生冲突的次数为: %d\n")
/*--------------------------- Other errors -------------------------------*/
OUTPUT(OUTPUT_ID_OUT_RANGE,              "找不到此类型的输出消息\n")
OUTPUT(OUTPUT_FORMAT_ERR,                "输出消息格式出错,位于索引 %d \n")
OUTPUT(OUTPUT_TAIL,                      "dummy")