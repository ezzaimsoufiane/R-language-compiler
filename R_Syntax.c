#include "R_Lexer.c"
#include "R_Semantic.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEFT    0
#define RIGHT   1
#define EQUAL   2

#define NORMAL 0
#define CONDIT 1

#define ISID        2
#define ISCALLFUNC  3
#define ISDATA      4
#define ISOPERATION 5


/** GRAMMAR for R language

Prog :      [expr | Loop | Function | Decision]*

expr:       id <- ( id | CallFunction | data )  [ operator (id | CallFunction | data) ]*
            | id <- function
            | ( id | CallFunction | data )  [ operator (id | CallFunction | data) ]* -> id

operation:   ( id | CallFunction | data ) operator (id | CallFunction | data )

Operator :  == | < | > | + | - | * | /

AssignOp :  <- | = | ->

data     :  INT | FLOAT | STRING | VECTOR

Decision:   if ( condition ) { expr }
            | if ( condition ) { expr } else { expr }

Loop:       while ( condition ) { expr }
            |   repeat { expr }
            |   for ( ID in vector ) { expr }

vector :    c( id [, id]* )

Function:       function ( None | id [, id ]* ) { expr | return }

CallFunc    id ( None | data [,data]* )

**/

/**Declarations**/

Symbol tempSymbol;
int IDorDataorCallFuncorOP;

/// Tokens Test
int TestToken(TOKEN_CODE TokenToTest){
    if(Token.Type != TokenToTest)
        return FALSE;
    // SUCCES END
    NextToken();
    return TRUE;
}

/// End Expr
void EndExpr(){
    if(newline == TRUE){
        return;
    }
    else if(TestToken(SEMICOLON))   {
        return;
    }
    else{
        ERRORSyntax("End of Expression");
    }
}

/// No newline
void NoNewline(){
    if(newline == TRUE) ERRORSyntax("Invalid New Line");
}


/**
    Prog :      expr | Loop | Function | Decision
**/
int main(){

    program = fopen("testing.txt", "r");
    if(program == NULL){
        perror("Error while opening the file");
        exit(1);
    }
    else{
        NextChar();
        NextToken();

        while(Token.Type != END){
            if(isLoop()){}
            else if(isDecision()){}
            else if(isFunction()){}
            else if(isExpr()){}
            else{
                ERRORSyntax("\nInvalid Expression\n");
            }
        }
        fclose(program);
    }
    printf("\n\n\n\n\n");
    printSymbolTable();
    printf("\n\n\n\t\t\t\t*********** FIN DE PROGRAMME  *************\n\n\n");
    return 0;
}

/**
    expr:       id <- ( id | CallFunction | data )  [ operator (id | CallFunction | data) ]*
                | id <- function
                | id -> id
                | function -> id
                | ( id | CallFunction | data )  [ operator (id | CallFunction | data) ]* -> id
**/
int isExpr(){
    // We need to determine with what the expression begin
    IDorDataorCallFuncorOP = isID_CallFunc_Data_Operation();
    /// ID
    if(IDorDataorCallFuncorOP == ISID){
        // Keep the id Symbol
        Symbol IDSymbol = ActualSymb;

        // id -> id
        if(isAssignOp(RIGHT)){
            if(!isID()) ERRORSyntax("Incorrect assign data");
            SemanticID(IDSymbol, ActualSymb);
            // SUCCES END
            EndExpr();
            return TRUE;
        }
        else if(isAssignOp(LEFT) || isAssignOp(EQUAL) ){
            // Construct our Symbol to assigned it to id
            int operation = FALSE;
            int n = 0;
            Symbol assignLeft;
            memset(assignLeft.Name, '\0', lenValue);
            memset(assignLeft.Value, '\0', lenValue);
            //id <- function
            if( isFunction() ){
                return TRUE;
            }
            //id <- ( id | CallFunction | data )  [ operator (id | CallFunction | data) ]*
            IDorDataorCallFuncorOP = isID_CallFunc_Data_Operation();

            if(IDorDataorCallFuncorOP == ISID){
                SemanticID(tempSymbol, IDSymbol);
            }
            else if(IDorDataorCallFuncorOP == ISDATA) {
                SemanticData(tempSymbol, IDSymbol);
            }
            else if(IDorDataorCallFuncorOP == ISCALLFUNC){
            }
            else if(IDorDataorCallFuncorOP == ISOPERATION){
                SemanticData(tempSymbol, IDSymbol);
            }
            EndExpr();
            // SUCCES END
            return TRUE;
        }
    }
    /// Data or Operation
    else if( IDorDataorCallFuncorOP == ISDATA ){
        NoNewline();
        if(!isAssignOp(RIGHT)) ERRORSyntax("Incorrect Assign Data");

        if(!isID()) ERRORSyntax("Incorrect Assign Data");

        SemanticData(tempSymbol,ActualSymb);
        // SUCCES END
        EndExpr();
        return TRUE;
    }
    else if( IDorDataorCallFuncorOP == ISOPERATION ){

        if(!isAssignOp(RIGHT)) ERRORSyntax("Incorrect Assign Data");
        NoNewline();
        if(!isID()) ERRORSyntax("Incorrect Assign Data");

        SemanticData(tempSymbol, ActualSymb);
        // SUCCES END
        EndExpr();
        return TRUE;
    }
    /// CallFucntion
    else if(IDorDataorCallFuncorOP == ISCALLFUNC ){

        if(isAssignOp(RIGHT)){
            NoNewline();
            if(!isID()) ERRORSyntax("Incorrect Assign Data");
        }
        else if(isAssignOp(LEFT)){
            ERRORSyntax("Incorrect Assign Call Function");
        }
        // SUCCES END
        EndExpr();
        return TRUE;
    }
    else{return FALSE;}
}

/**
    Decision:   if ( condition ) { expr }
                | if ( condition ) { expr } else { expr }
**/
int isDecision(){

    if(!TestToken(IF)) return FALSE;
    NoNewline();
    if(!TestToken(OPEN_PAR)) ERRORSyntax("Open Parentheses");
    NoNewline();
    if(!isCondition()) ERRORSyntax("Invalid Condition");
    if(!TestToken(CLOSE_PAR)) ERRORSyntax("Close Parentheses");
    if(!TestToken(OPEN_BRACE)) ERRORSyntax("Open Brace");
    while(isExpr());
    if(!TestToken(CLOSE_BRACE)) ERRORSyntax("Close Brace");

    // ELSE Statement
    if(TestToken(ELSE)){
        if(!TestToken(OPEN_BRACE)) ERRORSyntax("Open Brace");
        while(isExpr());
        if(!TestToken(CLOSE_BRACE)) ERRORSyntax("Close Brace");
    }
    // SUCCES END
    return TRUE;
}

/** Function:       function ( None | id [, id ]* ) { expr | return }
                    function -> id
**/
int isFunction(){

    if(!TestToken(FUNCTION)) return FALSE;
    NoNewline();
    if(!TestToken(OPEN_PAR)) ERRORSyntax("Invalid Function");
    NoNewline();
    if(isID()){
        NoNewline();
        while(TestToken(COLON)){
            NoNewline();
            if(!isID())
                ERRORSyntax("Args Function");
            NoNewline();
        }
    }
    NoNewline();
    if(!TestToken(CLOSE_PAR)) ERRORSyntax("Args Function");
    if(!TestToken(OPEN_BRACE)) ERRORSyntax("Invalid Function");
    while(isReturn() || isExpr() );
    if(! TestToken(CLOSE_BRACE)) ERRORSyntax("Invalid Function");

    if(isAssignOp(RIGHT)) {
        NoNewline();
        if(!isID()) ERRORSyntax("Incorrect Assign Function");
    }
    // SUCCES END
    EndExpr();
    return TRUE;
}

int isReturn(){
    if( !TestToken(RETURN)) return FALSE;
    NoNewline();
    IDorDataorCallFuncorOP = isID_CallFunc_Data_Operation();
    if( IDorDataorCallFuncorOP != ISID  && IDorDataorCallFuncorOP != ISDATA   &&    IDorDataorCallFuncorOP != ISOPERATION )
        ERRORSyntax("Bad return");

    // SUCCES END
    EndExpr();
    return TRUE;
}

/**
    isLoop:     for ( ID in vector ) { expr }
                |   while condition { expr }
                |   repeat { expr }
**/
int isLoop(){

    switch(Token.Type){

    /// for ( ID in vector ) { expr }
    case FOR:
        NextToken();
        NoNewline();
        if(!TestToken(OPEN_PAR)) ERRORSyntax("Invalid For");
        NoNewline();
        if(!isID()) ERRORSyntax("ID in For");
        NoNewline();
        if(!TestToken(IN)) ERRORSyntax("IN in For");
        NoNewline();
        if( ! (isVector() || isID()) ) ERRORSyntax("Not vector in For");
        NoNewline();
        if(!TestToken(CLOSE_PAR)) ERRORSyntax("Invalid For");

        if(!TestToken(OPEN_BRACE)) ERRORSyntax("Invalid For");
        while(isExpr());
        if(!TestToken(CLOSE_BRACE)) ERRORSyntax("Invalid For");

        // SUCCES END
        return TRUE;
        break;

    /// while ( condition ) { expr }
    case WHILE:
        NextToken();
        NoNewline();
        if(!TestToken(OPEN_PAR)) ERRORSyntax("Invalid While");
        NoNewline();
        if(!isCondition()) ERRORSyntax("Not Condition");
        NoNewline();
        if(!TestToken(CLOSE_PAR)) ERRORSyntax("Invalid While");
        if(!TestToken(OPEN_BRACE)) ERRORSyntax("Invalid While");
        while(isExpr());
        if(!TestToken(CLOSE_BRACE)) ERRORSyntax("Invalid While");

        // SUCCES END
        return TRUE;
        break;

    /// repeat { expr }
    case REPEAT:
        NextToken();
        if(!TestToken(OPEN_BRACE)) ERRORSyntax("Invalid Repeat loop");
        while(isExpr());
        if(!TestToken(CLOSE_BRACE)) ERRORSyntax("Invalid Repeat loop");

        // SUCCES END
        return TRUE;
        break;
    /// Not a Loop statement
    default: return FALSE;
    }
}
/** is Vector
    vector : c( id [, id]* )
**/
int isVector(){
    if(!TestToken(VECTOR)) return FALSE;
    if(!TestToken(OPEN_PAR)) ERRORSyntax("Invalid Vector");
    if(!isData()) ERRORSyntax("Invalid Vector");
    while(TestToken(COLON))
        if(!isData())   ERRORSyntax("Invalid Vector");
    if(!TestToken(CLOSE_PAR)) ERRORSyntax("Invalid Vector");
    return TRUE;
}

/** ID **/
int isID(){
    if(!TestToken(ID)) return FALSE;
    // SUCCES END
    return TRUE;
}

/** <- -> = **/
int isAssignOp(int assignop){

    switch(assignop){
    case LEFT: if(!TestToken(LEFT_ASSIGN)) return FALSE;
        break;
    case RIGHT: if(!TestToken(RIGHT_ASSIGN)) return FALSE;
        break;
    case EQUAL: if(!TestToken(EQUAL_ASSIGN)) return FALSE;
        break;
    }
    // SUCCES END
    return TRUE;
}

/** condition:   ( id | CallFunction | data ) operator (id | CallFunction | data ) **/
int isCondition(){

    if(! ( isID() || isCallFunction() || isData() ) ) ERRORSyntax("Incorrect Condition");
    NoNewline();
    if(!  isOperator(CONDIT)) ERRORSyntax("Incorrect Operator for Condition");
    NoNewline();
    if(! ( isID() || isCallFunction() || isData() ) ) ERRORSyntax("Incorrect Condition");

    // SUCCES END
    return TRUE;
}

/**
isID_CallFunc_Data_Operation :  ( id | CallFunction | data )  [operator (id | CallFunction | data) ]*
**/
int isID_CallFunc_Data_Operation(){

    int a = 0 ,b = 0, DeclaredID = TRUE;
    int cntPar = 0;
    int operation = FALSE;
    memset(tempSymbol.Name, '\0', lenValue);
    memset(tempSymbol.Value, '\0', lenValue);

    // Verify if there is an open parentheses
    while(TestToken(OPEN_PAR)) {
        NoNewline();
        strcat(tempSymbol.Value , ActualSymb.Value);
        cntPar++;
    }

    if(isID()){
        IDorDataorCallFuncorOP = ISID;
        NoNewline();
        // Verify if it's undeclared in case we have an operation
        if( ( a= isIDDeclared(ActualSymb) ) == UNDECLARED)
                DeclaredID = FALSE;
        else{
            tempSymbol.Type = SymbTable[a].Type;
            strcat(tempSymbol.Value , SymbTable[a].Value);
        }
    }
    else if(isCallFunction()){IDorDataorCallFuncorOP = ISCALLFUNC;}
    else if(isData()){
        strcat(tempSymbol.Value , ActualSymb.Value);
        tempSymbol.Type = ActualSymb.Type;
        IDorDataorCallFuncorOP = ISDATA;
    }
    else {return FALSE;}

    // Verify if we have a close parentheses
    while(TestToken(CLOSE_PAR)) {
        NoNewline();
        strcat(tempSymbol.Value , ActualSymb.Value);
        cntPar--;
    }

    // Verify if it's an Operation
    while(isOperator(NORMAL) == TRUE){
        NoNewline();
        //Verify if the previous ID is already declared
        if(DeclaredID == FALSE) ERRORSemantic("Undeclared Variable");

        // Add the Operator
        strcat(tempSymbol.Value, ActualSymb.Value);

        // Open parentheses after the operator
        while(TestToken(OPEN_PAR)) {
            NoNewline();
            strcat(tempSymbol.Value , ActualSymb.Value);
            cntPar++;
        }

        if(isID() ){
            //Verify if it's declared and it's type
            if( ( b= isIDDeclared(ActualSymb) ) == UNDECLARED) ERRORSemantic("Undeclared Id");
            if(SymbTable[b].Type != tempSymbol.Type) ERRORSemantic("Type mismatch");

            strcat(tempSymbol.Value , SymbTable[b].Value);

        }else if(isData()){
            if(ActualSymb.Type != tempSymbol.Type)  ERRORSemantic("Type mismatch");
            strcat(tempSymbol.Value , ActualSymb.Value);
        }else if(isCallFunction()){

        }else{
            ERRORSyntax("Operation");
        }

        while(TestToken(CLOSE_PAR)){
            NoNewline();
            strcat(tempSymbol.Value , ActualSymb.Value);
            cntPar--;
        }

        operation = TRUE;
    }

    if(cntPar != 0) ERRORSyntax("Error in Parentheses");

    // If not operation
    if(!operation) return IDorDataorCallFuncorOP;
    // If it's an operation
    return ISOPERATION;
}

/** Operator : == | < | <= | > | >= | + | - | * | /    **/
int isOperator(int type){
    ///Normal Operator
    if(type == NORMAL){
        if( !( TestToken(PLUS_OP)|| TestToken(MINUS_OP) || TestToken(DIV_OP) || TestToken(MULT_OP) ) )  return FALSE;
    }
    ///Condition Operator
    else if(type == CONDIT){
        if( !( TestToken(EQUAL_OP) || TestToken(INF_OP) || TestToken(INF_EQ_OP) || TestToken(SUP_OP) || TestToken(SUP_EQ_OP) ) )  return FALSE;
    }

    // SUCCES END
    return TRUE;
}

/** CallFunc: callfunc( data | [,data]* ) **/
int isCallFunction(){
    if(!TestToken(CALLFUNC))    return FALSE;
    NoNewline();
    if(!TestToken(OPEN_PAR)) ERRORSyntax("Invalid CallFunction");
    NoNewline();
    // Arguments
    if(isData() || isID()){
        NoNewline();
        while(TestToken(COLON)){
            NoNewline();
            if(! (isData() || isID()) ) ERRORSyntax("Invalid Args for CallFunction");
        }
    }
    NoNewline();
    if(!TestToken(CLOSE_PAR)) ERRORSyntax("Invalid CallFunction");
    // SUCCES END
    return TRUE;
}

/** data:       INT | FLOAT | STRING | VECTOR   **/
int isData(){
    if(!(TestToken(INT) || TestToken(FLOAT) || TestToken(STRING) || isVector() ) )  return FALSE;
    // SUCCES END
    return TRUE;
}
