#define Already 0

#define TRUE    1
#define FALSE   0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Tokens
typedef enum
{
ID,
/// Data Types
INT,
FLOAT,
STRING,
VECTOR,
/// Condition Statement
IF,
ELSE,
/// Loop Statements
WHILE,
REPEAT,
FOR,
IN,
/// Function
FUNCTION,
CALLFUNC,
RETURN,
PRINT,
READLINE,
///Special Symbols
  LEFT_ASSIGN,
  RIGHT_ASSIGN,
  EQUAL_ASSIGN,
  OPEN_PAR,
  CLOSE_PAR,
  OPEN_BRACKET,
  CLOSE_BRACKET,
  OPEN_BRACE,
  CLOSE_BRACE,
  SEMICOLON,
  EQUAL_OP,
  INF_OP,
  SUP_OP,
  INF_EQ_OP,
  SUP_EQ_OP,
  PLUS_OP,
  MINUS_OP,
  MULT_OP,
  DIV_OP,
  COLON,

END
} TOKEN_CODE;


char* Token_String[] ={
"ID",
/// Data Types
"INT",
"FLOAT",
"STRING",
"VECTOR",
/// Condition Statement
"IF",
"ELSE",
/// Loop Statements
"WHILE",
"REPEAT",
"FOR",
"IN",
/// Function
"FUNCTION",
"CALLFUNC",
"RETURN",
"PRINT",
"READLINE",
///Special Symbols
"LEFT_ASSIGN",
"RIGHT_ASSIGN",
"EQUAL_ASSIGN",
"OPEN_PAR",
"CLOSE_PAR",
"OPEN_BRACKET",
"CLOSE_BRACKET",
"OPEN_BRACE",
"CLOSE_BRACE",
"SEMICOLON",
"EQUAL_OP",
"INF_OP",
"SUP_OP",
"INF_EQ_OP",
"SUP_EQ_OP",
"PLUS_OP",
"MINUS_OP",
"MULT_OP",
"DIV_OP",
"COLON",

"END"
};

/***** Declarations ******/

FILE *program;

////////// Symbol Table
#define lenValue    40
#define lenName     40
#define MaxSymbols  50

typedef struct{
    TOKEN_CODE  Type;
    char Name[lenName];
    char Value[lenValue];
}Symbol;

Symbol SymbTable[MaxSymbols];
int nSymbol = 0;
