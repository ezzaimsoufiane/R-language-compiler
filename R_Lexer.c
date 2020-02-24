#include "globals.h"

/****** Variables ******/

char CurrentChar;
int newline;
int line = 1;
Symbol  ActualSymb;
Symbol  Token;

/// Get the next character
char NextChar(){
    CurrentChar = getc(program);
    return (CurrentChar);
}

/// ERRORSyntax Function
void ERRORSyntax(char* ErrorText){
    printf("\n\n %s \t", ErrorText);
    printf("Syntax Error ");
    printf("\t in line : %d \n\n" , line);
    exit(1);
}

/// Get the next Token
void NextToken(){
    ActualSymb = Token;
    // Clear Token
    memset(Token.Name, '\0', lenValue);
    memset(Token.Value, '\0', lenValue);

    // Ignore all type of separators
    ignoreSeparators();

    if(CurrentChar == EOF){ Token.Type = END; return; }
    else if( isNumber() ){}
    else if( isWord() ){}
    else if( isSpecial() ){}
    else{ ERRORSyntax("Invalid Token"); }

    //Print Tokens
    if(newline == TRUE)
        printf("\n");
    printf("%s\t", Token_String[Token.Type]);

return;
}
/**
    Ignore Whitespaces and Newlines and Comments
**/
void ignoreSeparators(){
    newline = FALSE;
    // if there is Whitespaces or newlines
    while( CurrentChar == ' ' || CurrentChar == '\t' || CurrentChar == '\n' || CurrentChar == '#'){
        if(CurrentChar == '\n'){
            line++;
            newline = TRUE;
        }
        else if(CurrentChar == '#'){
            while( CurrentChar != '\n' && CurrentChar != EOF){
                NextChar();
                if(CurrentChar == '\n'){
                    line++;
                }

            }
        }
        NextChar();
    }
}

/**
    INT:    digit+
    FLOAT:  digit+ . digit*
**/
int isNumber(){
    // If it isn't a character
    if( !isdigit(CurrentChar) )
        return FALSE;

    // Constructing our Number Symbol
    int position = 0;
    Token.Value[position]= CurrentChar;
    NextChar();
    //Reach the end of the number
    while ( isdigit(CurrentChar) ){
        Token.Value[++position] = CurrentChar;
        NextChar();
    }
    Token.Type = INT;
    // If it is a FLOAT
    if(CurrentChar == '.'){
        Token.Value[++position] = CurrentChar;
        Token.Type = FLOAT;
        NextChar();
        while ( isdigit(CurrentChar) ){
            Token.Value[++position] = CurrentChar;
            NextChar();
        }
    }
    // A number doesn't contain an alphabet
    if( isalpha(CurrentChar) )  ERRORSyntax("Invalid Number");

    // We can find multiple end of the operator
    int charNormal=FALSE;
    char ignoreThis[] = {' ', ',', '(', ')', '\n', EOF, '+','-','*','/', '{', '}',';', '\t'};

    for(int j=0; j<14; j++){
        if(CurrentChar == ignoreThis[j]){
            charNormal = TRUE;
        }
    }
    if(charNormal == FALSE)
        ERRORSyntax("Invalid Number");

    // SUCCES END
    return TRUE;
}

/**
    Word:
**/
int isWord(){
    // If the first of the word is a letter
    if( !isalpha(CurrentChar) )
        return FALSE;

    char* instName[]=       {"while",   "repeat", "for", "in" , "if", "else", "function", "return", "print", "readline"};
    TOKEN_CODE instToken[]= {WHILE,     REPEAT,    FOR,  IN,    IF,   ELSE,   FUNCTION,     RETURN,   PRINT,  READLINE};

    //Construct our word
    int position = 0;
    char word[40];
    memset(word, '\0', 40);
    word[position++] = CurrentChar;
    // If it is a vector c( , , , )
    if(CurrentChar == 'c'){
        NextChar();
        if(CurrentChar == '('){
            Token.Type = VECTOR;
            return TRUE;
        }
    }else{
        NextChar();
    }
    // Constructing the word
    while (isalnum(CurrentChar) || CurrentChar == '_'){
        word[position++] = CurrentChar;
        NextChar();
    }

    // Verify if it is an instruction
    for(int i=0; i < 9; i++){
        if (!strcmp(word, instName[i])){
            Token.Type = instToken[i];
            return TRUE;
        }
    }

    // Verify if it is a Call Function
    if(CurrentChar == '('){
        Token.Type = CALLFUNC;
        strcpy(Token.Name, word);
        return TRUE;
       }

    // If not all of this, it is just a an ID
    Token.Type = ID;
    strcpy(Token.Name, word);

    //SUCCES END
    return TRUE;
}

int Operator(){
    char operators[] = {'+','-','*','/'};
    for(int i=0; i<4; i++){
        if(CurrentChar == operators[i]){
            NextChar();
            return TRUE;
        }
    }
    return FALSE;
}

/// Verify if it's a special character
int isSpecial(){
    // Special :  <- | -> | = | ( | ) | { | } | ; | == | < | > | <= | >= | + | - | * | /
    char* specialChars[] =       {   "<-",        "->",        "=",        "(",        ")",      "{",          "}",          ";",      "==",     "<",    ">",    "<=",      ">=",      "+",    "-",       "*",    "/",      "[",            "]",        ","};
    TOKEN_CODE specialToken[] =   {LEFT_ASSIGN, RIGHT_ASSIGN, EQUAL_ASSIGN, OPEN_PAR, CLOSE_PAR, OPEN_BRACE, CLOSE_BRACE, SEMICOLON, EQUAL_OP, INF_OP, SUP_OP, INF_EQ_OP, SUP_EQ_OP, PLUS_OP, MINUS_OP, MULT_OP, DIV_OP, OPEN_BRACKET, CLOSE_BRACKET, COLON};

    // Construct our special Character
    char special[3];
    special[0] = CurrentChar;
    special[1] = '\0';

    // Verify if it is a String
    if(CurrentChar == '\"'){
        //Constructing our String
        char text[100];
        memset(text, '\0', 100);
        int position = 0;
        // Reach the end of the string
        NextChar();
        while(CurrentChar != '\"'){
            if(CurrentChar == EOF || CurrentChar == '\n' )
                ERRORSyntax("String not completed");

            text[position++] = CurrentChar;
            NextChar();
        }
        NextChar();
        Token.Type = STRING;
        strcpy(Token.Value, text);

        return TRUE;
    }

      // if it's composed by two characters like those == | <= | >= | <- | ->
      NextChar();
    if(CurrentChar == '=' || CurrentChar == '-' || CurrentChar == '>' ){
        special[1] = CurrentChar;
        special[2]= '\0';
    }

    // Search for which special symbol is it
    for(int i = 0; i < 20 ; i++){
        if( !strcmp(specialChars[i], special) ){
            Token.Type = specialToken[i];
            strcpy(Token.Value, special);
            // IF there are two characters in the symbol we need to go to the next character
            // if not it's already done before
            if(special[1] != '\0'){
                NextChar();
            }
            return TRUE;
        }
    }
    return FALSE;
}
