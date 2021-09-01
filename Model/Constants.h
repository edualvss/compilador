#ifndef CONSTANTS_H
#define CONSTANTS_H

enum TokenId 
{
    EPSILON  = 0,
    DOLLAR   = 1,
    t_id = 2,
    t_if = 3,
    t_else = 4,
    t_while = 5,
    t_for = 6,
    t_int = 7,
    t_float = 8,
    t_char = 9,
    t_bool = 10,
    t_do = 11,
    t_main = 12,
    t_return = 13,
    t_void = 14,
    t_break = 15,
    t_printf = 16,
    t_scanf = 17,
    t_true = 18,
    t_false = 19,
    t_TOKEN_20 = 20, //"+"
    t_TOKEN_21 = 21, //"-"
    t_TOKEN_22 = 22, //"*"
    t_TOKEN_23 = 23, //"/"
    t_TOKEN_24 = 24, //"%"
    t_TOKEN_25 = 25, //"^"
    t_TOKEN_26 = 26, //"::"
    t_TOKEN_27 = 27, //"="
    t_TOKEN_28 = 28, //"<"
    t_TOKEN_29 = 29, //"<="
    t_TOKEN_30 = 30, //">"
    t_TOKEN_31 = 31, //">="
    t_TOKEN_32 = 32, //"=="
    t_TOKEN_33 = 33, //"!="
    t_and = 34,
    t_or = 35,
    t_not = 36,
    t_TOKEN_37 = 37, //"||"
    t_TOKEN_38 = 38, //"&&"
    t_TOKEN_39 = 39, //"!"
    t_numberInt = 40,
    t_numberFloat = 41,
    t_character = 42,
    t_stringText = 43,
    t_commentLine = 44,
    t_commentMultiLine = 45,
    t_TOKEN_46 = 46, //"("
    t_TOKEN_47 = 47, //")"
    t_TOKEN_48 = 48, //"{"
    t_TOKEN_49 = 49, //"}"
    t_TOKEN_50 = 50, //"["
    t_TOKEN_51 = 51, //"]"
    t_TOKEN_52 = 52, //";"
    t_TOKEN_53 = 53, //":"
    t_TOKEN_54 = 54, //",
};

const int STATES_COUNT = 47;

extern int SCANNER_TABLE[STATES_COUNT][256];

extern int TOKEN_STATE[STATES_COUNT];

extern int SPECIAL_CASES_INDEXES[56];

extern const char *SPECIAL_CASES_KEYS[20];

extern int SPECIAL_CASES_VALUES[20];

extern const char *SCANNER_ERROR[STATES_COUNT];

const int FIRST_SEMANTIC_ACTION = 98;

const int SHIFT  = 0;
const int REDUCE = 1;
const int ACTION = 2;
const int ACCEPT = 3;
const int GO_TO  = 4;
const int ERROR  = 5;

extern const int PARSER_TABLE[289][159][2];

extern const int PRODUCTIONS[115][2];

extern const char *PARSER_ERROR[289];

#endif
