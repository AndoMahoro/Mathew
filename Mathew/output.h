#ifndef OUTPUT
#error "You must define OUTPUT macro before include this file"
#endif

//outputs
/*--------------------------- tool outputs -------------------------------*/
OUTPUT(OUTPUT_HEAD = 0,                  "dummy")
OUTPUT(REPRINT_INPUT_FILE,               "/* ���´�ӡ�����ļ� \"%s\" ����Ϣ\n*  ����:\n")
OUTPUT(NO_USED_RULE,                     "δʹ�õĲ���ʽ:\n")
OUTPUT(USED_RULE_EPSILON_SYMBOLS,        "��ʹ�õĲ���ʽ�����Ƴ���(epsilon)�ķ���:\n")
OUTPUT(SYMBOL_AT_RULE,                   "�� %d ������ʽ�ķ���: %s \n")
OUTPUT(NT_SYMBOLS_FIRST_SET,             "���ս�����׷���:\n")
OUTPUT(USED_RULE,                        "��ʹ�õĲ���ʽ:\n")
OUTPUT(STATE_NUM,                        "�������״̬��Ŀ: %d\n") 
OUTPUT(VERSION,                          "���޵�ǰ�汾 version 1.0\n") 
/*--------------------------- action outputs -------------------------------*/
OUTPUT(CONFLICT_COUNT,                   "������ͻ�Ĵ���Ϊ: %d\n")
/*--------------------------- Other errors -------------------------------*/
OUTPUT(OUTPUT_ID_OUT_RANGE,              "�Ҳ��������͵������Ϣ\n")
OUTPUT(OUTPUT_FORMAT_ERR,                "�����Ϣ��ʽ����,λ������ %d \n")
OUTPUT(OUTPUT_TAIL,                      "dummy")